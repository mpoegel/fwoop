#include <cstring>
#include <fwoop_array.h>
#include <gtest/gtest.h>

using namespace fwoop;

TEST(Array, extend)
{
    // GIVEN
    Array arr1(16);
    memcpy(*arr1, "hello world", 11);
    arr1.shrink(11);
    Array arr2(1);
    arr2[0] = '!';
    Array arr3(5);
    memcpy(*arr3, "hello", 5);

    // WHEN
    arr1.extend(arr2);

    // THEN
    EXPECT_EQ(arr1.toString(), "hello world!");
    EXPECT_EQ(arr1.size(), 12);

    // WHEN
    arr1.extend(arr3);
    EXPECT_EQ(arr1.toString(), "hello world!hello");
    EXPECT_EQ(arr1.size(), 17);
}

TEST(Array, append)
{
    // GIVEN
    Array arr(16);
    arr.shrink(0);

    // WHEN
    arr.append("hello");
    arr.append(" ");
    arr.append("world!");
    arr.append("hello");

    // THEN
    EXPECT_EQ(arr.toString(), "hello world!hello");
    EXPECT_EQ(arr.size(), 17);
}

TEST(Array, toHex)
{
    // GIVEN
    Array arr(16);
    arr.shrink(0);

    // WHEN
    arr.append("hello");

    // THEN
    EXPECT_EQ(arr.toHex(), "68 65 6c 6c 6f");
}
