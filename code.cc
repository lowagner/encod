#include "code.h"
#include "utf8.h"
#include <algorithm> // sort

bool compare_counts(CountCharacter &a, CountCharacter &b) {
    return (a.count < b.count);
}

int Code::randomize() {
    if (_clear_previous)
    for (auto iterate : _map) {
        iterate.second.encoder.clear();
    }
    std::random_shuffle(_decoder, _decoder+256);
    for (int i=0; i<256; ++i) {
        int32_t c = _decoder[i];
        try {
            EncodeCharacter &ec = _map.at(c);
            ec.encoder.push_back((uint8_t)i);
        } catch (const std::out_of_range &ob) {
            fprintf(stderr, "expecting %d but didn't find it in map\n", c); 
            return 1;
        }
    }
    _clear_previous = true;
    return 0;
}

int Code::finalize() {
    if (_map.size() > 256) {
        fprintf(stderr, "cannot write a byte-sized alphabet for this set of characters\n");
        return 1;
    }
    _final_count = 0;
    if (_clear_previous) {
        _counts.clear();
    }
    for (auto iterate : _map) {
        // first -> int32_t, second -> CountEncoder
        _counts.emplace_back(iterate.first, iterate.second.count);
        _final_count += iterate.second.count;
    }
    std::sort(_counts.begin(), _counts.end(), compare_counts);
    if (_counts.size() == 256) {
        for (int i=0; i<256; ++i) {
            _decoder[i] = _counts[i].character;
        }
        return randomize();
    }
    int sum_to_256 = 0; 
    int i_max = _counts.size()-1;
    int i=i_max;
    for (; i>=0; --i) {
        _counts[i].bins = 256 * _counts[i].count / _final_count;
        if (_counts[i].bins <= 1) {
            break;
        }
        sum_to_256 += _counts[i].bins;
    }
    int i_min = i+1;
    for (; i>=0; --i) {
        _counts[i].bins = 1;
        ++sum_to_256;
    }
    int diff = sum_to_256 - 256;
    while (diff)
    if (diff < 0) {
        // easy case -- finish it immediately!
        _counts[i_max].bins -= diff;
        diff = 0;
        break;
    } else {
        int original_diff = diff;
        for (int i=i_max; i>=i_min; --i) {
            int subtract = original_diff * _counts[i].count / _final_count;
            if (subtract == 0) {
                subtract = 1;
            }
            if (_counts[i].bins - subtract < 1) {
                i_min = i+1;
                break;
            } else {
                _counts[i].bins -= subtract;
                diff -= subtract;
            }
        }
        if (diff > 0) {
            // push it in the right direction if nothing else
            --_counts[i_max].bins;
            --diff;
        }
    }
    int cumulative = -1;
    int count = 0;
    int count_max = _counts.size();
    for (i=0; i<256; ++i) {
        _decoder[i] = _counts[count].character;
        if (count < count_max - 1 && i >= cumulative + _counts[count].bins)
            cumulative += _counts[count++].bins;
    }
    return randomize();
}

void Code::print(FILE *stream) {
    if (_map.size() > 256) {
        fprintf(stderr, "cannot write byte-sized alphabet for code\n");
        return;
    }
    for (int i=0; i<256; ++i) {
        char w[4];
        int nc = utf8_from_int(w, _decoder[i]);
        if (nc == 0) {
            fprintf(stderr, "had errors writing code header!\n");
            return;
        }
        for (int j=0; j<nc; ++j) {
            fputc(w[j], stream);
        }
    }
}

void Code::add(int32_t character) {
    try {
        ++_map.at(character).count;
    } catch (const std::out_of_range &ob) {
        _map[character] = EncodeCharacter(1);
    }
}

uint8_t Code::encode(int32_t character) {
    // this will fail if character is not in the set:
    EncodeCharacter &ec = _map.at(character);
    return ec.encoder[rand()%ec.encoder.size()];
}

int32_t Code::decode(uint8_t character) {
    return _decoder[character];
}

int Code::size() {
    return _map.size();
}
