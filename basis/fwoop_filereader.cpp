#include <fwoop_filereader.h>

namespace fwoop {

FileReader::FileReader(const std::string& filename)
: d_filename(filename)
{
}

int FileReader::open()
{
    d_file.open(d_filename);
    if (!d_file.is_open()) {
        return 1;
    }
    return 0;
}

void FileReader::close()
{
    d_file.close();
}

FileReader::Iterator FileReader::begin()
{
    Iterator itr(*this, false);
    return itr;
}

FileReader::Iterator FileReader::end()
{
    Iterator itr(*this, true);
    return itr;
}

FileReader::Iterator::Iterator(FileReader &reader, bool isEnd)
: d_reader(reader)
, d_isEnd(isEnd)
{
}

void FileReader::Iterator::operator++()
{
    if (d_reader.d_file.good()) {
        std::getline(d_reader.d_file, d_currLine);
    }
    else {
        d_isEnd = true;
    }
}

std::string FileReader::Iterator::operator*() const
{
    return d_currLine;
}

bool FileReader::Iterator::operator==(const Iterator &lhs) const
{
    return d_currLine == lhs.d_currLine && d_isEnd == lhs.d_isEnd;
}

}
