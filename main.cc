#include "code.h"
#include "utf8.h"
#include <stdio.h>
#include <stdlib.h> // srand, rand
#include <string.h>
#include "time.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Usage:\n encod file_name:  encodes file with name file_name\n");
        printf(" encod \"some string\":  encodes that string\n");
        return 1;
    }
    srand(time(NULL));
    int a = 1;
    while (a < argc) {
        if (strcmp(argv[a], "-d") == 0 || strcmp(argv[a], "--decoder") == 0) {
            ++a; // read in argv[a] as the decoder file, if it exists, or return 1 for error
            if (a >= argc) {
                fprintf(stderr, "expecting decoder file at argument %d, got nothing (also need an encoded file after that)\n", a); 
                return 1;
            }
            FILE *file = fopen(argv[a], "r");
            if (!file) {
                fprintf(stderr, "expecting decoder file at argument %d, got %s\n", a, argv[a]); 
                return 1;
            }
            int i=-1;
            int32_t decoder[256];
            char line[BUFSIZ];
            while (fgets(line, sizeof(line), file)) {
                int j=0;
                int32_t u;
                while ( (u=utf8_next_character(line, &j)) ) {
                    decoder[++i] = u;
                }
            }
            fclose(file);

            ++a; // read in argv[a] as the encoded file, and decode it
            if (a >= argc) {
                fprintf(stderr, "expecting encoded file at argument %d, got nothing\n", a); 
                return 1;
            }
            file = fopen(argv[a], "r");
            if (!file) {
                fprintf(stderr, "expecting encoded file at argument %d, got %s\n", a, argv[a]); 
                return 1;
            }
           
            // setup a file to save decoded message to:
            char filename[BUFSIZ];
            snprintf(filename, sizeof(filename), "%s-decoded", argv[a]);
            FILE *save = fopen(filename, "w");
            if (!save) { 
                printf("Could not open %s,\nwriting decoded message to stdout[\n", filename);
                save = stdout;
            } else {
                printf("Writing decoded message to %s\n", filename);
            }
            int c;
            while ( (c=fgetc(file)) >= 0) {
                char w[4];
                utf8_from_int(w, decoder[c]);
                fprintf(save, "%s", w);
            }
            fclose(file); // done reading in file
            if (save == stdout) {
                save = NULL;
                printf("]End decoded message\n  (leave out the ']End decoded message')\n");
            } else {
                fclose(save);
            }
        } else {
            Code code;
            // open a file to encode
            FILE *file = fopen(argv[a], "r");
            if (file) {
                printf("Reading file %s\n", argv[a]);
                char line[BUFSIZ]; /* BUFSIZ is defined if you include stdio.h */
                while (fgets(line, sizeof(line), file)) {
                    int i=0;
                    int32_t u;
                    while ( (u=utf8_next_character(line, &i)) ) {
                        code.add(u);
                    }
                }
            } else {
                // argv[a] is not a file, interpret it as some characters to encode 
                printf("Counting %s\n", argv[a]);
                int i=0;
                int32_t u;
                while ( (u=utf8_next_character(argv[a], &i)) ) {
                    code.add(u);
                }
            }

            // common stuff
            printf("Got %d distinct characters\n", code.size());
            if (code.finalize()) { // had an error
                // skipping the rest...
            } else if (file) {
                // go back to start of file:
                fseek(file, 0, SEEK_SET); 
                char filename[BUFSIZ];

                // write decoder file:
                snprintf(filename, sizeof(filename), "%s-decoder", argv[a]);
                FILE *save = fopen(filename, "w");
                if (save) {
                    printf("Writing decoder to %s\n", filename);
                    code.print(save);
                    fclose(save);
                } else {
                    printf("Decoder follows the newline[\n");
                    code.print();
                    printf("]Decoder finished!\n  (leave out the ']Decoder finished')\n");
                }

                // write encoded file:
                snprintf(filename, sizeof(filename), "%s-encoded", argv[a]);
                save = fopen(filename, "w");
                if (!save) { 
                    printf("Could not open %s,\nwriting encoded message to stdout[\n", filename);
                    save = stdout;
                } else {
                    printf("Writing encoded message to %s\n", filename);
                }
                char line[BUFSIZ];
                while (fgets(line, sizeof(line), file)) {
                    // read file into line buffer:
                    int i=0;
                    int32_t u;
                    while ( (u=utf8_next_character(line, &i)) ) {
                        // write each character into save
                        fputc(code.encode(u), save);
                    }
                }
                if (save == stdout) {
                    save = NULL;
                    printf("]End encoded message\n  (leave out the ']End encoded message')\n");
                } else {
                    fclose(save);
                }
                fclose(file);
            } else {
                printf("Decoder follows the newline[\n");
                code.print();
                printf("]Decoder finished!\n  (leave out the ']Decoder finished')\n");
                printf("Encoded message follows newline[\n");
                int i=0;
                int32_t u;
                while ( (u=utf8_next_character(argv[a], &i)) ) {
                    fputc(code.encode(u), stdout);
                }
                printf("]Encoded message finished!\n");
            }
        }
        ++a;
    }
    return 0;
}

