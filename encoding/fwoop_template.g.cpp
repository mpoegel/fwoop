#include <fwoop_template.h>
#include <fwoop_log.h>

#include <gtest/gtest.h>

TEST(Template, encodeVariable)
{
    // GIVEN
    uint8_t *input = (uint8_t*)" foobar ";
    unsigned int length = 8;
    fwoop::Template::Context_t context;
    context["foobar"] = fwoop::Template::Variable_t(42);
    fwoop::Template t(input, length);
    unsigned int encodingLength = 0;
    uint8_t *expected = (uint8_t*)" 42 ";
    unsigned int expectedLength = 4;

    // WHEN
    uint8_t *encoding = t.encode(context, encodingLength);

    // THEN
    EXPECT_NE(encoding, nullptr);
    ASSERT_EQ(expectedLength, encodingLength);
    for (unsigned int i = 0; i < encodingLength; ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;
}


TEST(Template, encodeConditionalTrue)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"if foobar then popfizz endif";
    unsigned int length = 28;
    fwoop::Template::Context_t context;
    context["foobar"] = fwoop::Template::Variable_t(true);
    fwoop::Template t(input, length);
    unsigned int encodingLength = 0;
    uint8_t *expected = (uint8_t*)"popfizz";
    unsigned int expectedLength = 7;

    // WHEN
    uint8_t *encoding = t.encode(context, encodingLength);

    // THEN
    EXPECT_NE(encoding, nullptr);

    fwoop::Log::Debug(std::string((char*)encoding, encodingLength));

    ASSERT_EQ(expectedLength, encodingLength);
    for (unsigned int i = 0; i < encodingLength; ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;
}

TEST(Template, encodeConditionalFalse)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"if foobar then popfizz endif";
    unsigned int length = 28;
    fwoop::Template::Context_t context;
    context["foobar"] = fwoop::Template::Variable_t(false);
    fwoop::Template t(input, length);
    unsigned int encodingLength = 0;
    uint8_t *expected = (uint8_t*)"";
    unsigned int expectedLength = 0;

    // WHEN
    uint8_t *encoding = t.encode(context, encodingLength);

    // THEN
    EXPECT_NE(encoding, nullptr);

    fwoop::Log::Debug(std::string((char*)encoding, encodingLength));

    ASSERT_EQ(expectedLength, encodingLength);
    delete[] encoding;
}

TEST(Template, encodeConditionalFalseElse)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"if foobar then popfizz else hotfuzz endif";
    unsigned int length = 41;
    fwoop::Template::Context_t context;
    context["foobar"] = fwoop::Template::Variable_t(false);
    fwoop::Template t(input, length);
    unsigned int encodingLength = 0;
    uint8_t *expected = (uint8_t*)"hotfuzz";
    unsigned int expectedLength = 7;

    // WHEN
    uint8_t *encoding = t.encode(context, encodingLength);

    // THEN
    EXPECT_NE(encoding, nullptr);

    fwoop::Log::Debug(std::string((char*)encoding, encodingLength));

    ASSERT_EQ(expectedLength, encodingLength);
    for (unsigned int i = 0; i < encodingLength; ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;
}

TEST(Template, encodeConditionalStringTrue)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"if foobar == pop then popfizz endif";
    unsigned int length = 35;
    fwoop::Template::Context_t context;
    context["foobar"] = fwoop::Template::Variable_t("pop");
    fwoop::Template t(input, length);
    unsigned int encodingLength = 0;
    uint8_t *expected = (uint8_t*)"popfizz";
    unsigned int expectedLength = 7;

    // WHEN
    uint8_t *encoding = t.encode(context, encodingLength);

    // THEN
    EXPECT_NE(encoding, nullptr);

    fwoop::Log::Debug(std::string((char*)encoding, encodingLength));

    ASSERT_EQ(expectedLength, encodingLength);
    for (unsigned int i = 0; i < encodingLength; ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;
}
