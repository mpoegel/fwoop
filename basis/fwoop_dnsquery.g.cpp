#include <gtest/gtest.h>

#include <fwoop_dnsquery.h>

TEST(DnsQuery, GetHostByName)
{
    // GIVEN

    // WHEN
    const std::string host = fwoop::DNS::Query::GetHostByName("www.google.com");

    // THEN
    EXPECT_LT(0, host.length());
    std::cerr << "host: " << host << '\n';
}

TEST(DnsQuery, GetRecord)
{
    // GIVEN
    std::string hostname = "www.google.com";
    auto question = fwoop::DNS::Question(hostname, fwoop::DNS::A, fwoop::DNS::IN);

    // WHEN
    auto record = fwoop::DNS::Query::GetRecord(question);

    // THEN
    ASSERT_TRUE(record);
    std::cerr << "record: " << *record.get() << '\n';
}
