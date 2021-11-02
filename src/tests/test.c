#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <greatest.h>

#include "test_url_parsing.h"

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(url_parsing_suite);
    GREATEST_MAIN_END();
}