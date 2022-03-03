#include <fwoop_httpheadersframe.h>
#include <fwoop_log.h>
#include <gtest/gtest.h>

TEST(HttpHeadersFrame, headerBlocks)
{
    // GIVEN
    auto packer = std::make_shared<fwoop::HttpHPacker>();
    fwoop::HttpHeadersFrame frame(packer);

    // WHEN
    frame.addHeaderBlock(fwoop::HttpHeader::Status, "200");
    frame.addHeaderBlock("custom-header", "custom-value");

    // THEN
    auto itr = frame.beginHeaderList();
    ASSERT_TRUE(std::holds_alternative<fwoop::HttpHeader>(itr->first));
    EXPECT_EQ(fwoop::HttpHeader::Status, std::get<fwoop::HttpHeader>(itr->first));
    EXPECT_EQ("200", itr->second);
    itr++;
    ASSERT_TRUE(itr != frame.endHeaderList());
    ASSERT_TRUE(std::holds_alternative<std::string>(itr->first));
    EXPECT_EQ("custom-header", std::get<std::string>(itr->first));
    EXPECT_EQ("custom-value", itr->second);
    itr++;
    EXPECT_TRUE(itr == frame.endHeaderList());
}