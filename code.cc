#include "code.h"

void Code::finalize() {
    BigInt final_count = 0; // number of all characters, including duplicates
    int unique = 0;
    for (int i=0; i<256; ++i) {
        if (!_counts[i])
            continue;
        ++unique;
        final_count += _counts[i];
    }

    for (int i=1; i<256; ++i) {
        for (int j=i-1; j>=0; --j)
            if (_counts[_sorted_chars[j+1]] > _counts[_sorted_chars[j]])
                std::swap(_sorted_chars[j], _sorted_chars[j+1]);
    }

    if (unique == 256) {
        for (int i=0; i<256; ++i)
            _decoder[i] = i;
        randomize_decoder();
        make_encoding();
        return;
    }

    int sum_to_256 = 0; 
    int i=0;
    for (; i<256; ++i) {
        uint8_t c = _sorted_chars[i];
        BigInt bin = _counts[c]*256 / final_count;
        _bins[c] = bin.get_ui();
        if (_bins[c] <= 1)
            break;
        sum_to_256 += _bins[c];
    }

    int i_max = i; // everything at this point and beyond can't be modified by the later algorithm
    for (; i<256; ++i) {
        uint8_t c = _sorted_chars[i];
        if (!_counts[c])
            break;
        _bins[c] = 1;
        ++sum_to_256;
    }

    int diff = sum_to_256 - 256;
    while (diff)
    if (diff < 0) {
        // easy case -- finish it immediately!
        _bins[_sorted_chars[0]] -= diff;
        diff = 0;
        break;
    } else {
        int original_diff = diff;
        for (int i=0; i<i_max; ++i) {
            uint8_t c = _sorted_chars[i];
            int subtract;
            {
                BigInt _subtract =_counts[c] * original_diff / final_count;
                subtract = _subtract.get_ui();
            }
            if (subtract == 0) {
                subtract = 1;
            }
            if (_counts[c] - subtract < 1) {
                --i_max;
                break;
            } else {
                _bins[_sorted_chars[i]] -= subtract;
                diff -= subtract;
            }
        }
        if (diff > 0) {
            // push it in the right direction if nothing else
            --_bins[_sorted_chars[0]];
            --diff;
        }
    }

    int count = _bins[_sorted_chars[0]];
    int index = 0;
    for (i=0; i<255; ++i) {
        _decoder[i] = _sorted_chars[index];
        if (--count <= 0)
            count = _bins[_sorted_chars[++index]];
    }
    _decoder[i] = _sorted_chars[index];

    randomize_decoder();
    make_encoding();
}

void Code::encode(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file)
        throw std::invalid_argument("invalid file to encode");
    clear();
    size_t read_bytes;
    char line[BUFSIZ]; 
    while (1) {
        read_bytes = fread(line, 1, sizeof(line), file);
        for (int i=0; i<read_bytes; ++i)
            add(line[i]);
        if (read_bytes < sizeof(line))
            break;
    }
    finalize();
    
    char filename_output[BUFSIZ];
    // write decoder file:
    snprintf(filename_output, sizeof(filename_output), "%s-decoder", filename);
    FILE *save = fopen(filename_output, "w");
    if (!save) {
        fclose(file);
        throw std::invalid_argument("couldn't open decoder file for output");
    }
    size_t write_bytes = fwrite(_decoder, 1, 256, save);
    fclose(save);
    if (write_bytes != 256) {
        fclose(file);
        throw std::invalid_argument("couldn't write decoder file for output");
    }

    // write encoded file:
    snprintf(filename_output, sizeof(filename_output), "%s-encoded", filename);
    save = fopen(filename_output, "w");
    if (!save) { 
        fclose(file);
        throw std::invalid_argument("couldn't open encoded file for output");
    }

    // go back to start of file:
    fseek(file, 0, SEEK_SET); 
    
    while (1) {
        // read file into line buffer:
        read_bytes = fread(line, 1, sizeof(line), file);
        if (!read_bytes)
            break;
        for (int i=0; i<read_bytes; ++i)
            line[i] = encode(line[i]);
        write_bytes = fwrite(line, 1, read_bytes, save);
        if (write_bytes != read_bytes) {
            fclose(save);
            fclose(file);
            throw std::out_of_range("write error");
        }
        if (read_bytes < sizeof(line))
            break;
    }
    fclose(save);
    fclose(file);
}

void Code::decode(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file)
        throw std::invalid_argument("expecting encoded file");
   
    // setup a file to save decoded message to:
    char filename_output[BUFSIZ];
    snprintf(filename_output, sizeof(filename_output), "%s-decoded", filename);
    FILE *save = fopen(filename_output, "w");
    if (!save) { 
        fclose(file);
        throw std::invalid_argument("couldn't open file to write decoded message");
    }
    
    char line[BUFSIZ];
    while (1) {
        // read file into line buffer:
        size_t read_bytes = fread(line, 1, sizeof(line), file);
        if (!read_bytes)
            break;
        for (int i=0; i<read_bytes; ++i)
            line[i] = decode(line[i]);
        size_t write_bytes = fwrite(line, 1, read_bytes, save);
        if (write_bytes != read_bytes) {
            fclose(save);
            fclose(file);
            throw std::out_of_range("write error");
        }
        if (read_bytes < sizeof(line))
            break;
    }
    fclose(save);
    fclose(file);
}

void Code::make_encoding() {
    for (int i=0; i<256; ++i)
        _encoders[i].clear();
    
    for (int i=0; i<256; ++i)
        _encoders[_decoder[i]].push_back(i);
}
