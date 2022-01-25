#include <fwoop_tokenizer.h>

#include <gtest/gtest.h>

TEST(TokenizerTest, Constructor)
{
    // GIVEN

    // WHEN

    // THEN
    EXPECT_TRUE(true);
}

TEST(TokenizerTest, Iterator)
{
    // GIVEN
    fwoop::Tokenizer tokenizer("foo,bar,pop,fizz", ',');

    // WHEN
    fwoop::Tokenizer::Iterator itr = tokenizer.begin();

    // THEN
    EXPECT_EQ(*itr, "foo");
    ++itr;
    EXPECT_NE(itr, tokenizer.end());
    EXPECT_EQ(*itr, "bar");
    ++itr;
    EXPECT_NE(itr, tokenizer.end());
    EXPECT_EQ(*itr, "pop");
    ++itr;
    EXPECT_NE(itr, tokenizer.end());
    EXPECT_EQ(*itr, "fizz");
    ++itr;
    EXPECT_EQ(itr, tokenizer.end());
}
