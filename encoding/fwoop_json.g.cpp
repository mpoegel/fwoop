#include <fwoop_json.h>
#include <fwoop_log.h>

#include <gtest/gtest.h>

TEST(JSON, basicString)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"{ \"foo\": \"bar\" }";
    uint32_t length = 16;
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject(input, length, bytesParsed);

    // THEN
    EXPECT_EQ(length, bytesParsed);
    std::optional<std::string> val = json.get<std::string>("foo");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ("bar", val.value());
}

TEST(JSON, basicTypes)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"{ \"foo\": \"bar\", \"total\" : 24 , \"chance\": 0.30, \"winner\": true }";
    uint32_t length = 63;
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject(input, length, bytesParsed);

    // THEN
    EXPECT_EQ(length, bytesParsed);

    std::optional<std::string> fooVal = json.getString("foo");
    ASSERT_TRUE(fooVal.has_value());
    EXPECT_EQ("bar", fooVal.value());

    std::optional<int> totalVal = json.getInt("total");
    ASSERT_TRUE(totalVal.has_value());
    EXPECT_EQ(24, totalVal.value());

    std::optional<double> chanceVal = json.getDouble("chance");
    ASSERT_TRUE(chanceVal.has_value());
    EXPECT_DOUBLE_EQ(0.30, chanceVal.value());

    std::optional<bool> winnerVal = json.getBool("winner");
    ASSERT_TRUE(winnerVal.has_value());
    EXPECT_TRUE(winnerVal.value());
}

TEST(JSON, array)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"{ \"foo\": [ \"bar\", \"pop\", \"fizz\"] }";
    uint32_t length = 34;
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject(input, length, bytesParsed);

    // THEN
    EXPECT_EQ(length, bytesParsed);
    std::shared_ptr<fwoop::JsonArray> val = json.getArray("foo");
    ASSERT_TRUE(val.get());
    std::optional<std::string> first = val->get<std::string>(0);
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ("bar", first.value());
}

TEST(JSON, nestedObject)
{
    // GIVEN
    uint8_t *input = (uint8_t*)"{ \"foo\": {\"bar\": \"pop\"} }";
    uint32_t length = 25;
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject(input, length, bytesParsed);

    // THEN
    EXPECT_EQ(length, bytesParsed);
    std::shared_ptr<fwoop::JsonObject> val = json.getObject("foo");
    ASSERT_TRUE(val.get());
    std::optional<std::string> first = val->get<std::string>("bar");
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ("pop", first.value());
}
