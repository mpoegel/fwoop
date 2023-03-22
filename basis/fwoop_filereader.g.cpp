#include <fwoop_filereader.h>

#include <string>

#include <gtest/gtest.h>

TEST(FileReaderTest, Constructor)
{
    // GIVEN
    const std::string filename("../../basis/testdata/test.txt");
    fwoop::FileReader reader(filename);

    // WHEN
    EXPECT_EQ(0, reader.open());

    // THEN
    for (fwoop::FileReader::Iterator itr = reader.begin(); itr != reader.end(); ++itr) {
        std::cout << *itr << '\n';
    }
}

TEST(FileReaderTest, loadFile)
{
    // GIVEN
    const std::string filename("../../basis/testdata/test.txt");
    fwoop::FileReader reader(filename);
    uint32_t length = 0;
    std::string expected = "hello\nworld";

    // WHEN
    EXPECT_EQ(0, reader.open());
    uint8_t *contents = reader.loadFile(length);

    // THEN
    EXPECT_EQ(11, length);
    EXPECT_STREQ(expected.data(), (char *)contents);
}

TEST(FileReaderText, getExtension)
{
    // GIVEN
    const std::string filename("../../basis/testdata/test.html");

    // WHEN
    auto ext = fwoop::FileReader::getExtension(filename);

    // THEN
    EXPECT_EQ("html", ext);
}
