#include <fwoop_argparser.h>

#include <gtest/gtest.h>

TEST(ArgParser, parsePositionalArgs)
{
    // GIVEN
    const char *argv[] = {"./test", "foo", "3"};
    int argc = 3;
    fwoop::ArgParser parser(argv, argc);

    // WHEN
    parser.addPositionalArg("arg1", "fish", "arg1 help");
    parser.addPositionalArg("arg2", 44, "arg2 is helpful");
    parser.addPositionalArg("arg3", 88, "arg3 is unhelpful");
    auto ec = parser.parse();

    // THEN
    ASSERT_FALSE(ec) << ec.message();
    EXPECT_EQ("foo", parser.getPositionalArg<std::string>("arg1"));
    EXPECT_EQ(3, parser.getPositionalArg<int>("arg2"));
    EXPECT_EQ(88, parser.getPositionalArg<int>("arg3"));
}

TEST(ArgParser, parseNamedArgs)
{
    // GIVEN
    const char *argv[] = {"./test", "-arg1", "fizz", "-a", "9000", "-b"};
    int argc = 6;
    fwoop::ArgParser parser(argv, argc);

    // WHEN
    parser.addNamedArg("arg1", "arg1", "fish", "arg1 help");
    parser.addNamedArg("arg2", "arg2", 44, "arg2 is helpful");
    parser.addNamedArg("arg3", "a", 88, "arg3 is unhelpful");
    parser.addNamedArg("barg", "b", false, "barg barg barg");
    auto ec = parser.parse();

    // THEN
    ASSERT_FALSE(ec) << ec.message();
    EXPECT_EQ("fizz", parser.getNamedArg<std::string>("arg1"));
    EXPECT_EQ(44, parser.getNamedArg<int>("arg2"));
    EXPECT_EQ(9000, parser.getNamedArg<int>("arg3"));
    EXPECT_TRUE(parser.getNamedArg<bool>("barg"));
}

TEST(ArgParser, parseAllArgs)
{
    // GIVEN

    // WHEN

    // THEN
}

TEST(ArgParser, help)
{
    // GIVEN

    // WHEN

    // THEN
}
