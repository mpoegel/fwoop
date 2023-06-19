#include <cstdint>
#include <fwoop_json.h>
#include <fwoop_log.h>

#include <gtest/gtest.h>
#include <optional>

TEST(JSON, basicString)
{
    // GIVEN
    uint8_t *input = (uint8_t *)"{ \"foo\": \"bar\" }";
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
    uint8_t *input = (uint8_t *)"{ \"foo\": \"bar\", \"total\" : 24 , \"chance\": 0.30, \"winner\": true }";
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
    uint8_t *input = (uint8_t *)"{ \"foo\": [ \"bar\", \"pop\", \"fizz\"] }";
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
    uint8_t *input = (uint8_t *)"{ \"foo\": {\"bar\": \"pop\"} }";
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

TEST(JSON, arrayOfObjects)
{
    // GIVEN
    uint8_t *input = (uint8_t *)"{ \"foo\": [{\"bar\": \"pop\"}] }";
    uint32_t length = 27;
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject(input, length, bytesParsed);

    // THEN
    EXPECT_EQ(length, bytesParsed);
    std::shared_ptr<fwoop::JsonArray> val = json.getArray("foo");
    ASSERT_TRUE(val.get());
    std::shared_ptr<fwoop::JsonObject> first = val->getObject(0);
    ASSERT_TRUE(first);
    std::optional<std::string> inner = first->get<std::string>("bar");
    ASSERT_TRUE(inner.has_value());
    EXPECT_EQ("pop", inner.value());
}

TEST(JSON, arrayOfArrays)
{
    // GIVEN
    uint8_t *input = (uint8_t *)"{ \"foo\": [[\"bar\", \"pop\"]] }";
    uint32_t length = 27;
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject(input, length, bytesParsed);

    // THEN
    EXPECT_EQ(length, bytesParsed);
    std::shared_ptr<fwoop::JsonArray> val = json.getArray("foo");
    ASSERT_TRUE(val.get());
    std::shared_ptr<fwoop::JsonArray> first = val->getArray(0);
    ASSERT_TRUE(first);
    std::optional<std::string> innerFirst = first->get<std::string>(0);
    ASSERT_TRUE(innerFirst.has_value());
    EXPECT_EQ("bar", innerFirst.value());
    std::optional<std::string> innerSecond = first->get<std::string>(1);
    ASSERT_TRUE(innerSecond.has_value());
    EXPECT_EQ("pop", innerSecond.value());
}

TEST(JSON, largeInput)
{
    // GIVEN
    const std::string input =
        "{"
        "\"coord\" : {\"lon\" : -73.9497, \"lat\" : 40.6526},"
        "\"weather\" : [ {\"id\" : 800, \"main\" : \"Clear\", \"description\" : \"clear sky\", \"icon\" : \"01d\"} ],"
        "\"base\" : \"stations\","
        "\"main\" : {"
        "    \"temp\" : 45.25,"
        "    \"feels_like\" : 39.24,"
        "    \"temp_min\" : 41.32,"
        "    \"temp_max\" : 48.11,"
        "    \"pressure\" : 1033,"
        "    \"humidity\" : 48"
        "},"
        "\"visibility\" : 10000,"
        "\"wind\" : {\"speed\" : 12.66, \"deg\" : 40},"
        "\"clouds\" : {\"all\" : 0},"
        "\"dt\" : 1681044900,"
        "\"sys\" : {\"type\" : 2, \"id\" : 2080536, \"country\" : \"US\", \"sunrise\" : 1681036000, \"sunset\" : "
        "1681082866},"
        "\"timezone\" : -14400,"
        "\"id\" : 5110302,"
        "\"name\" : \"Brooklyn\","
        "\"cod\" : 200"
        "}";
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject((uint8_t *)input.c_str(), input.size(), bytesParsed);

    // THEN
    EXPECT_EQ(input.size(), bytesParsed);
    auto name = json.get<std::string>("name");
    ASSERT_TRUE(name.has_value());
    EXPECT_EQ("Brooklyn", name.value());
    auto main = json.getObject("main");
    ASSERT_TRUE(main != nullptr);
    auto temp = main->get<double>("temp");
    ASSERT_TRUE(temp.has_value());
    EXPECT_FLOAT_EQ(45.25, temp.value());
    auto humidity = main->get<int>("humidity");
    ASSERT_TRUE(humidity.has_value());
    EXPECT_EQ(48, humidity.value());
}

TEST(JSON, largeInput2)
{
    // GIVEN
    const std::string input =
        "{"
        "\"coord\": {\"lon\":-73.9496,\"lat\":40.6501},"
        "\"weather\": [{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],"
        "\"base\": \"stations\","
        "\"main\": "
        "{\"temp\":65.25,\"feels_like\":64.85,\"temp_min\":62.85,\"temp_max\":68.88,\"pressure\":1016,\"humidity\":72},"
        "\"visibility\":10000,"
        "\"wind\":{\"speed\":17.27,\"deg\":60},"
        "\"clouds\":{\"all\":100},"
        "\"dt\":1685811607,"
        "\"sys\":{\"type\":2,\"id\":2080536,\"country\":\"US\",\"sunrise\":1685784392,\"sunset\":1685838098},"
        "\"timezone\":-14400,"
        "\"id\":5110302,"
        "\"name\":\"Brooklyn\","
        "\"cod\":200"
        "}";
    uint32_t bytesParsed;

    // WHEN
    auto json = fwoop::JsonObject((uint8_t *)input.c_str(), input.size(), bytesParsed);

    // THEN
    EXPECT_EQ(input.size(), bytesParsed);
    auto name = json.get<std::string>("name");
    ASSERT_TRUE(name.has_value());
    EXPECT_EQ("Brooklyn", name.value());
    auto main = json.getObject("main");
    ASSERT_TRUE(main != nullptr);
    auto temp = main->get<double>("temp");
    ASSERT_TRUE(temp.has_value());
    EXPECT_FLOAT_EQ(65.25, temp.value());
    auto humidity = main->get<int>("humidity");
    ASSERT_TRUE(humidity.has_value());
    EXPECT_EQ(72, humidity.value());
}

TEST(JSON, encodeBasicTypes)
{
    // GIVEN
    uint8_t *input = (uint8_t *)"{ \"foo\": \"bar\", \"total\" : 24 , \"chance\": 0.30, \"winner\": true }";
    uint32_t length = 63;
    uint32_t bytesParsed;
    uint32_t encodedLen = 0;

    // WHEN
    auto json = fwoop::JsonObject(input, length, bytesParsed);

    // THEN
    auto encoding = json.encode(encodedLen);
    std::cerr << encoding << '\n';
}
