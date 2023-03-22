#include <fwoop_filereader.h>

#include <cstring>
#include <limits>

namespace fwoop {

std::string FileReader::getExtension(const std::string &filename)
{
    auto index = filename.rfind(".");
    if (index == std::string::npos) {
        return "";
    }
    return filename.substr(index + 1, filename.length() - index - 1);
}

FileReader::FileReader(const std::string &filename) : d_filename(filename) {}

int FileReader::open()
{
    d_file.open(d_filename);
    if (!d_file.is_open()) {
        return 1;
    }
    return 0;
}

void FileReader::close() { d_file.close(); }

uint8_t *FileReader::loadFile(uint32_t &length)
{
    d_file.ignore(std::numeric_limits<std::streamsize>::max());
    length = d_file.gcount();
    d_file.clear();
    d_file.seekg(0, std::ios_base::beg);

    uint8_t *contents = new uint8_t[length];
    memset(contents, 0, length);
    // d_file.get((char*)contents, length);
    d_file.read((char *)contents, length);
    return contents;
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

FileReader::Iterator::Iterator(FileReader &reader, bool isEnd) : d_reader(reader), d_isEnd(isEnd) {}

void FileReader::Iterator::operator++()
{
    if (d_reader.d_file.good()) {
        std::getline(d_reader.d_file, d_currLine);
    } else {
        d_currLine.clear();
        d_isEnd = true;
    }
}

std::string FileReader::Iterator::operator*() const { return d_currLine; }

bool FileReader::Iterator::operator==(const Iterator &lhs) const
{
    return d_currLine == lhs.d_currLine && d_isEnd == lhs.d_isEnd;
}

} // namespace fwoop
