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
