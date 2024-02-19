#include <cstring>
#include <fwoop_array.h>
#include <gtest/gtest.h>

using namespace fwoop;

TEST(Array, extend)
{
    // GIVEN
    Array arr1(16);
    memcpy(*arr1, "hello world", 11);
    arr1.enlarge(11);
    Array arr2(1);
    arr2[0] = '!';
    arr2.enlarge(1);
    Array arr3(5);
    memcpy(*arr3, "hello", 5);
    arr3.enlarge(5);

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

    // WHEN
    arr.append("hello");
    arr.append(" ");
    arr.append("world!");
    arr.append("hello");

    // THEN
    EXPECT_EQ(arr.toString(), "hello world!hello");
    EXPECT_EQ(arr.size(), 17);
}

TEST(Array, subArray)
{
    // GIVEN
    Array arr(16);
    arr.append("hello world");

    // WHEN
    Array arr1 = arr.subArray(0, 5);
    Array arr2 = arr.subArray(6, 11);

    // THEN
    EXPECT_EQ(arr1.toString(), "hello");
    EXPECT_EQ(arr1.size(), 5);
    EXPECT_EQ(arr2.toString(), "world");
    EXPECT_EQ(arr2.size(), 5);
}

TEST(Array, toHex)
{
    // GIVEN
    Array arr(16);

    // WHEN
    arr.append("hello");

    // THEN
    EXPECT_EQ(arr.toHex(), "68 65 6c 6c 6f");
}
