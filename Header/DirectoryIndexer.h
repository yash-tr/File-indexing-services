#ifndef DIRECTORY_INDEXER_H
#define DIRECTORY_INDEXER_H

#include <Windows.h>
#include <string>
#include <unordered_map>

// Exception class for directory indexing errors
class DirectoryIndexingException : public std::exception
{
public:
    DirectoryIndexingException(const std::string &message) : msg(message) {}

    const char *what() const noexcept override
    {
        return msg.c_str();
    }

private:
    std::string msg;
};

// Abstract base class for directory indexing using templates
template <typename K, typename V>
class DirectoryIndexer
{
public:
    virtual void IndexDirectory(const std::wstring &directory, int &fileCount, int &subdirectoryCount, std::unordered_map<K, V> &fileIndex) = 0;
};

