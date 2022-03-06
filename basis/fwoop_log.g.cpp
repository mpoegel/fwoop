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
    fwoop::Log::Debug("Hello ", "World");
    fwoop::Log::Info("Hello ", "Solar System");
    fwoop::Log::Warn("Hello ", "Galaxy");
    fwoop::Log::Error("Hello ", "Universe");
}

TEST(LogTest, LogFormatInts)
{
    // THEN
    fwoop::Log::Debug("Hello in ", 5, " colors");
    fwoop::Log::Info("Hello in ", 6, " colors");
    fwoop::Log::Warn("Hello in ", 7, " colors");
    fwoop::Log::Error("Hello in ", 8, " colors");
}

TEST(LogTest, LogFormatMultiTypes)
{
    // THEN
    fwoop::Log::Debug("Hello ", "world", " in ", 5, " colors");
    fwoop::Log::Info("Hello ", "solar system", " in ", 6, " colors");
    fwoop::Log::Warn("Hello ", "galaxy", " in ", 7, " colors");
    fwoop::Log::Error("Hello ", "universe", " in ", 8, " colors");
}

TEST(LogTest, LogComplexTypes)
{
    // GIVEN
    std::string str("world");

    // THEN
    fwoop::Log::Debug("Hello ", str);
}
