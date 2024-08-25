#include <fwoop_random.h>
#include <gtest/gtest.h>

TEST(Random, getInt)
{
    // GIVEN

    // WHEN
    uint16_t r1 = fwoop::Random::getUInt16();
    uint32_t r2 = fwoop::Random::getUInt16();
    std::cerr << r1 << " " << r2 << '\n';

    // THEN
    ASSERT_LT(0, r1);
    ASSERT_LT(0, r2);
}
