#include "code.h"

#include <stdio.h>
#include <stdlib.h> // srand, rand
#include <string.h>
#include "time.h"

#define TEST(x) try { \
    test_##x(); \
} catch (std::exception &e) { \
    std::cerr << "test_"#x" failed\n" << e.what() << "\n"; return 1; \
} \
std::cout << "test_"#x" passed\n";

#define TESTSHOULDFAIL(x) try { \
    test_##x(); \
    std::cerr << "test_"#x" FAILED.  it executed without error; when it should have thrown an exception.\n"; return 1; \
} catch (std::exception &e) { \
    std::cerr << "test_"#x" passed, as it threw an exception (and it was supposed to).\n";\
}

void test_encode_decode() {
    Code code;
    code.encode("main.cc");
    code.from("main.cc-decoder");
    code.decode("main.cc-encoded");
    printf("check that main.cc == main.cc-encoded-decoded\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));
    TEST(encode_decode);    
    return 0;
}

