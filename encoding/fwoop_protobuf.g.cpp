#include <cstdint>
#include <fwoop_protobuf.h>
#include <optional>

#include <gtest/gtest.h>
#include <system_error>
#include <variant>
#include <vector>

class Test1 : public fwoop::protobuf::Message {
  public:
    ~Test1(){};
    std::optional<int32_t> a;

    std::error_code setField(uint32_t id, uint64_t value) override
    {
        if (id == 1) {
            a = value;
            return std::error_code();
        }
        return std::error_code();
    };
};

class Test2 : public fwoop::protobuf::Message {
  public:
    ~Test2(){};
    std::optional<std::string> b;

    std::error_code setField(uint32_t id, const std::string &value) override
    {
        if (id == 2) {
            b = value;
            return std::error_code();
        }
        return std::error_code();
    };
};

class Test3 : public fwoop::protobuf::Message {
  public:
    ~Test3(){};
    std::optional<Test1> c;

    std::error_code setField(uint32_t id, const std::string &value) override
    {
        if (id == 3) {
            return std::error_code(static_cast<int>(DecodeResult::WantSubMessage), fwoop::protobuf::Error);
        }
        return std::error_code();
    };
    std::error_code setField(uint32_t id, uint8_t *buf, uint32_t bufLen) override
    {
        if (id == 3) {
            Test1 msg;
            c.emplace(msg);
            uint32_t parsed = 0;
            c.value().decode(buf, bufLen, parsed);
        }
        return std::error_code();
    }
};

class Test4 : public fwoop::protobuf::Message {
  public:
    ~Test4(){};
    std::optional<std::string> d;
    std::vector<int32_t> e;

    std::error_code setField(uint32_t id, uint64_t value) override
    {
        if (id == 5) {
            e.push_back(value);
            return std::error_code();
        }
        return std::error_code();
    };
    std::error_code setField(uint32_t id, const std::string &value) override
    {
        if (id == 4) {
            d = value;
        }
        return std::error_code();
    };
};

TEST(Protobuf, decodeTest1)
{
    // GIVEN
    uint8_t input[] = {0x08, 0x96, 0x01};
    uint32_t bytesParsed = 0;
    Test1 msg;

    // WHEN
    msg.decode(input, sizeof(input), bytesParsed);

    // THEN
    EXPECT_EQ(bytesParsed, sizeof(input));
    ASSERT_TRUE(msg.a.has_value());
    EXPECT_EQ(msg.a, 150);
}

TEST(Protobuf, decodeTest2)
{
    // GIVEN
    uint8_t input[] = {0x12, 0x07, 0x74, 0x65, 0x73, 0x74, 0x69, 0x6e, 0x67};
    uint32_t bytesParsed = 0;
    Test2 msg;

    // WHEN
    msg.decode(input, sizeof(input), bytesParsed);

    // THEN
    EXPECT_EQ(bytesParsed, sizeof(input));
    ASSERT_TRUE(msg.b.has_value());
    EXPECT_EQ(msg.b, "testing");
}

TEST(Protobuf, decodeTest3)
{
    // GIVEN
    uint8_t input[] = {0x1a, 0x03, 0x08, 0x96, 0x01};
    uint32_t bytesParsed = 0;
    Test3 msg;

    // WHEN
    msg.decode(input, sizeof(input), bytesParsed);

    // THEN
    EXPECT_EQ(bytesParsed, sizeof(input));
    ASSERT_TRUE(msg.c.has_value());
    ASSERT_TRUE(msg.c.value().a.has_value());
    EXPECT_EQ(msg.c.value().a.value(), 150);
}

TEST(Protobuf, decodeTest4)
{
    // GIVEN
    uint8_t input[] = {0x22, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x28, 0x01, 0x28, 0x02, 0x28, 0x03};
    uint32_t bytesParsed = 0;
    Test4 msg;

    // WHEN
    msg.decode(input, sizeof(input), bytesParsed);

    // THEN
    EXPECT_EQ(bytesParsed, sizeof(input));
    ASSERT_TRUE(msg.d.has_value());
    EXPECT_EQ(msg.d.value(), "hello");
    ASSERT_EQ(msg.e.size(), 3);
    EXPECT_EQ(msg.e[0], 1);
    EXPECT_EQ(msg.e[1], 2);
    EXPECT_EQ(msg.e[2], 3);
}
