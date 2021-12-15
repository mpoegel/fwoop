#pragma once

#include <fstream>
#include <string>
#include <system_error>

namespace fwoop {

class FileReader {
  private:
    std::string   d_filename;
    std::ifstream d_file;

  public:
    class Iterator;

    explicit FileReader(const std::string& filename);
    ~FileReader();

    int open();
    void close();

    Iterator begin();
    Iterator end();
};

class FileReader::Iterator {
  private:
    FileReader &d_reader;
    std::string d_currLine;
    bool        d_isEnd;

  public:
    Iterator(FileReader &reader, bool isEnd);
    ~Iterator();

    void operator++();
    std::string operator*() const;

    bool operator==(const Iterator &lhs) const;
    bool operator!=(const Iterator &lhs) const;
};

inline
FileReader::~FileReader()
{
    close();
}

inline
FileReader::Iterator::~Iterator()
{
}

inline
bool FileReader::Iterator::operator!=(const Iterator &lhs) const
{
    return !(*this == lhs);
}

}
