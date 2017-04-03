#ifndef CODE_H
#define CODE_H
#include <stdint.h>
#include <vector>
#include <iostream>
#include <algorithm> // sort

#include <gmpxx.h>
typedef mpz_class BigInt;

class Code {
public:
    Code () {
        clear();
    }
    Code (const char* filename) {
        from(filename);
    }

    void from(const char* filename) {
        // read in decoder from filename
        FILE *f = fopen(filename, "r");
        if (!f)
            throw std::invalid_argument("not a valid filename for an decoder file");
        size_t result = fread(_decoder, 1, 256, f);
        fclose(f);
        if (result != 256) {
            throw std::invalid_argument("invalid decoder file, not large enough.");
        }
        make_encoding();
    }

    void add(char character) {
        ++_counts[(uint8_t)character];
    }

    char encode(char character) {
        std::vector<char> &possible_encodings = _encoders[(uint8_t)character]; 
        if (!possible_encodings.size())
            throw std::out_of_range("invalid character, not in encoding");

        return possible_encodings[rand()%possible_encodings.size()];
    }
    char decode(char character) {
        return (char)_decoder[(uint8_t)character];
    }
   
    friend std::ostream &operator << (std::ostream &os, const Code &c) {
        // this output may be invalid utf8
        for (int i=0; i<256; ++i)
            os << (char)c._decoder[i];
        return os;
    }

    void encode(const char *filename);
    void decode(const char *filename);

    void clear() {
        for (int i=0; i<256; ++i)
            _counts[i] = 0;
        for (int i=0; i<256; ++i)
            _sorted_chars[i] = i;
    }
    void finalize();

protected:
    uint8_t _decoder[256];
    uint8_t _sorted_chars[256];
    std::vector<char> _encoders[256];
    int _bins[256]; // number of spots allocated to each character
    BigInt _counts[256];

    void make_encoding();
    void randomize_decoder() {
        std::random_shuffle(_decoder, _decoder+256);
    }
    bool compare_counts(uint8_t &a, uint8_t &b) {
        return (_counts[a] > _counts[b]);
    }
};


#endif
