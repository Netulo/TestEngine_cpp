#include <te/TestEngine.h>

TEST_CASE(testOne) {
    CHECK(1);
}
TEST_CASE(testThree) {
    REQUIRE(2 == 2);
    CHECK_EQ(2 + 2, 4);
    CHECK_EQ(2 + 1, 4);
    CHECK_THROWS(1, std::out_of_range);

}