#define TE_MAIN
#include <te/TestEngine.h>

TEST_CASE(testOne) {
    CHECK(1);
}
TEST_CASE(testTwo) {
    CHECK(0);
}

