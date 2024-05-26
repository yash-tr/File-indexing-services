#ifndef DIRECTORYINDEXER_H
#define DIRECTORYINDEXER_H

#define UNICODE
#include <Windows.h>
#include <iostream>
#include <string>
#include <stx/btree_map>
#include <locale>
#include <codecvt>
#include <chrono>
#include <exception>
#include <thread>
#include <mutex>
#include <vector>

// Exception class for directory indexing errors
class DirectoryIndexingException : public std::exception
{
public:
    DirectoryIndexingException(const std::string& message);

    const char* what() const noexcept override;

private:
    std::string msg;
};

// Abstract base class for directory indexing using templates
template <typename K, typename V>
class DirectoryIndexer
{
public:
    virtual void IndexDirectory(const std::wstring& directory, int& fileCount, int& subdirectoryCount, stx::btree_map<K, V>& fileIndex) = 0;
};

// Derived class implementing B-tree method for directory indexing
template <typename K, typename V>
class BtreeSearchIndexer : public DirectoryIndexer<K, V>
{
public:
    void IndexDirectory(const std::wstring& directory, int& fileCount, int& subdirectoryCount, stx::btree_map<K, V>& fileIndex) override;
};

#endif // DIRECTORYINDEXER_H

private:
    string msg;
};

// Abstract base class for directory indexing using templates
template <typename K, typename V>
class DirectoryIndexer
{
public:
    virtual void IndexDirectory(const wstring& directory, int& fileCount, int& subdirectoryCount, stx::btree_map<K, V>& fileIndex) = 0;
};

// Mutex to synchronize access to shared data structures
extern std::mutex mtx1;

// Derived class implementing btree-search method for directory indexing
template <typename K, typename V>
class BtreeSearchIndexer : public DirectoryIndexer<K, V>
{
public:
    void IndexDirectory(const wstring& directory, int& fileCount, int& subdirectoryCount, stx::btree_map<K, V>& fileIndex) override;
};


#endif // DIRECTORYINDEXER_H
