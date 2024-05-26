#pragma once
#ifndef HASHING_H
#define HASHING_H

#define UNICODE
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

// Base class template for file indexing
template <typename T>
class CFileIndexer
{
public:
    // Pure virtual function for hashing filenames
    virtual size_t hash_filename(const T& filename) = 0;

    // Pure virtual function for listing files in a directory and hashing them
    virtual void vListFilesInDirectoryH(const T& directory, map<size_t, vector<T>>& index, int& file_count) = 0;

    // Pure virtual function for printing the indexed files
    virtual void print_index(const map<size_t, vector<T>>& index, int file_count) = 0;
};

// Derived class template that implements the file indexing functionality
template <typename T>
class CHashing : public CFileIndexer<T>
{
public:
    // Hash function that returns an index for a given filename
    size_t hash_filename(const T& filename) override;

    // Function to list files in a directory, hash their names, and store them in the index
    void vListFilesInDirectoryH(const T& directory, map<size_t, vector<T>>& index, int& file_count) override;

    // Function to print the indexed files
    void print_index(const map<size_t, vector<T>>& index, int file_count) override;
};

#endif // HASHING_H

