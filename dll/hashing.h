#pragma once

#ifdef HASHING_EXPORTS
#define HASHING_API __declspec(dllexport)
#else
#define HASHING_API __declspec(dllimport)
#endif
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <chrono>
#include <algorithm>
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
class HASHING_API CFileIndexer
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
    size_t hash_filename(const T& filename) override;
    void vListFilesInDirectoryH(const T& directory, map<size_t, vector<T>>& index, int& file_count) override;
    void print_index(const map<size_t, vector<T>>& index, int file_count) override;
};

// Explicit instantiation declaration (no HASHING_API here)
extern template class CHashing<wstring>;
