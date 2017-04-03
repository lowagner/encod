#include "code.h"

#include <stdio.h>
#include <stdlib.h> // srand, rand
#include <string.h>
#include "time.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Usage:\n");
        printf(" %s filename                             # encodes file\n", argv[0]);
        printf(" %s -d decoder_filename encoded_filename # decodes filename\n", argv[0]);
        return 1;
    }
    srand(time(NULL));
    int a = 1;
    while (a < argc) {
        if (strcmp(argv[a], "-d") == 0 || strcmp(argv[a], "--decoder") == 0) {
            if (++a >= argc)
                throw std::out_of_range("expecting decoder file at next argument, as well as a file after that to decode"); 
            if (a + 1 >= argc)
                throw std::out_of_range("expecting encoded file: -d [decoder_file] [file to decode]");
            
            Code code(argv[a]); // get in the decoder file
            code.decode(argv[++a]); // and decode file at next argument position
        } else {
            Code code;
            code.encode(argv[a]); // encode file at current arg
        }
        ++a;
    }
    return 0;
}

