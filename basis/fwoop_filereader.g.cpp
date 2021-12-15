#include <fwoop_filereader.h>

#include <string>

#include <gtest/gtest.h>

TEST(FileReaderTest, Constructor)
{
    // GIVEN
    const std::string filename("test.txt");
    fwoop::FileReader reader(filename);

    // WHEN
    EXPECT_EQ(0, reader.open());

    // THEN
    for (fwoop::FileReader::Iterator itr = reader.begin(); itr != reader.end(); ++itr) {
        std::cout << *itr << '\n';
    }
}
