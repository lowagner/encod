#ifndef CODE_H
#define CODE_H
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <vector>

struct EncodeCharacter {
    int count;
    std::vector<uint8_t> encoder;

    EncodeCharacter() {
        count = 0;
    }
    EncodeCharacter(int count_) {
        count = count_;
    }
};

struct CountCharacter {
    int32_t character;
    int count, bins;
    
    CountCharacter(int32_t character_=0, int count_=0, int bins_=0) {
        character = character_;
        count = count_;
        bins = bins_;
    }

    CountCharacter(const CountCharacter &c) {
        character = c.character;
        count = c.count;
        bins = c.bins;
    }
};

bool compare_counts(CountCharacter &e, CountCharacter &f);

class Code {
    std::map<int32_t, EncodeCharacter> _map;
    std::vector<CountCharacter> _counts;
    int _final_count; // number of all characters, including duplicates
    int32_t _decoder[256];
    bool _clear_previous;

public:
    Code() {
        _clear_previous = false;
    }
    ~Code() {
        //for (auto iterate : _map) {
        //    iterate.second.encoder.clear();
        //}
        //_map.clear();
        //_counts.clear();
    }
    
    void add(int32_t character);
    int finalize();
    int randomize();
    
    int size(); // number of characters in the mapping

    uint8_t encode(int32_t character);
    int32_t decode(uint8_t character);
    
    void print(FILE *stream = stdout);
};


#endif

