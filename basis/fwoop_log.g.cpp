#include <fwoop_log.h>

#include <gtest/gtest.h>

TEST(LogTest, Log)
{
    // THEN
    fwoop::Log::Debug("Hello World");
    fwoop::Log::Info("Hello Solar System");
    fwoop::Log::Warn("Hello Galaxy");
    fwoop::Log::Error("Hello Universe");
}

TEST(LogTest, LogFormat)
{
    // THEN
    fwoop::Log::Debug("Hello %s", "World");
    fwoop::Log::Info("Hello %s", "Solar System");
    fwoop::Log::Warn("Hello %s", "Galaxy");
    fwoop::Log::Error("Hello %s", "Universe");
}

TEST(LogTest, LogFormatInts)
{
    // THEN
    fwoop::Log::Debug("Hello in %d colors", 5);
    fwoop::Log::Info("Hello in %d colors", 6);
    fwoop::Log::Warn("Hello in %d colors", 7);
    fwoop::Log::Error("Hello in %d colors", 8);
}

TEST(LogTest, LogFormatMultiTypes)
{
    // THEN
    fwoop::Log::Debug("Hello %s in %d colors", "world", 5);
    fwoop::Log::Info("Hello %s in %d colors", "solar system", 6);
    fwoop::Log::Warn("Hello %s in %d colors", "galaxy", 7);
    fwoop::Log::Error("Hello %s in %d colors", "universe", 8);
}
