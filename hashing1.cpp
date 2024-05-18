#define UNICODE
#include <iostream> //provides input/output operations in C++.
#include <fstream>//provides file input/output operations.
#include <string>// Provides functions for working with strings
#include <vector>//provides a dynamic array-like data structure.
#include <map>//provides an associative array data structure.
#include <cstring>// provides functions for manipulating C-style strings
#include <chrono>//provides functionality for measuring time durations.
#include <algorithm>//provides a collection of functions for performing operations on containers.
#include <windows.h>//provides access to the Windows API.
#include <locale>  // to std::wstring_convert
#include <codecvt>//provides a codecvt facet that can convert between different character encodings>

using namespace std;

template <typename T>
class CFileIndexer
{
public:
    virtual size_t hash_filename(const T& filename) = 0;
    virtual void vListFilesInDirectory(const T& directory, map<size_t, vector<T>>& index, int& file_count) = 0;
    virtual void print_index(const map<size_t, vector<T>>& index, int file_count) = 0;
};

template <typename T>
class CHashing : public CFileIndexer<T>
{
public:
    // Hash function that returns an index for a given filename
    size_t hash_filename(const T& filename)
    {
        const size_t p = 31; // A prime number used in the hash function
        const size_t m = 1e9 + 9; // A large prime number used in the hash function
        size_t hash_value = 0;
        size_t p_pow = 1;
        for (auto c : filename)
        {
            hash_value = (hash_value + (size_t(c) * p_pow) % m) % m; // The hash function
            p_pow = (p_pow * p) % m;
        }
        return hash_value;
    }

    void vListFilesInDirectory(const T& directory, map<size_t, vector<T>>& index, int& file_count)
    {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind;

        try
        {
            // Search for all files and directories in the given directory
            T searchPath = directory + L"\\*";
            hFind = FindFirstFile(searchPath.c_str(), &findFileData);

            if (hFind == INVALID_HANDLE_VALUE)
            {
                throw runtime_error("Invalid handle value. Directory not found or access denied.");
            }

            do
            {
                // Ignore "." and ".." directories
                if (wcscmp(findFileData.cFileName, L".") == 0 ||
                    wcscmp(findFileData.cFileName, L"..") == 0)
                {
                    continue;
                }

                // Check if the file is a directory
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    // Recursively list files in subdirectory
                    T subdirectory = directory + L"\\" + findFileData.cFileName;
                    vListFilesInDirectory(subdirectory, index, file_count);
                }
                else
                {
                    // Hash filename to get the index key
                    size_t index_key = hash_filename(findFileData.cFileName);
                    T file_path = directory + L"\\" + findFileData.cFileName;
                    index[index_key].push_back(file_path);
                    file_count++;
                }
            } while (FindNextFile(hFind, &findFileData) != 0);

            FindClose(hFind);
        }
        catch (const runtime_error& e)
        {
            cerr << "Error: " << e.what() << endl;
        }
        catch (...)
        {
            cerr << "An unknown error occurred while listing files in the directory." << endl;
        }
    }

    void print_index(const map<size_t, vector<T>>& index, int file_count)
    {
        cout << "Indexed " << file_count << " files." << endl;

        // Print index
        for (auto it = index.begin(); it != index.end(); it++)
        {
            cout << "Index key " << it->first << " : " << endl;
            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                wcout << "  " << *it2 << endl;
            }   
        }
    }
};


