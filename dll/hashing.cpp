#define HASHING_EXPORTS
#include "hashing.h"
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

// Implementations of template functions

template <typename T>
size_t CHashing<T>::hash_filename(const T& filename)
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

template <typename T>
void CHashing<T>::vListFilesInDirectoryH(const T& directory, map<size_t, vector<T>>& index, int& file_count)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    try
    {
        T searchPath = directory + L"\\*";
        hFind = FindFirstFile(searchPath.c_str(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE)
        {
            throw runtime_error("Invalid handle value. Directory not found or access denied.");
        }

        queue<T> directories;
        directories.push(directory);

        vector<thread> threads;
        mutex index_mutex;
        condition_variable cv;
        bool done = false;

        auto worker = [&]()
            {
                while (true)
                {
                    T current_directory;

                    {
                        unique_lock<mutex> lock(index_mutex);
                        cv.wait(lock, [&]() { return !directories.empty() || done; });

                        if (done && directories.empty()) return;

                        current_directory = directories.front();
                        directories.pop();
                    }

                    T searchPath = current_directory + L"\\*";
                    hFind = FindFirstFile(searchPath.c_str(), &findFileData);

                    if (hFind == INVALID_HANDLE_VALUE) continue;

                    do
                    {
                        if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) continue;

                        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                        {
                            T subdirectory = current_directory + L"\\" + findFileData.cFileName;
                            {
                                lock_guard<mutex> lock(index_mutex);
                                directories.push(subdirectory);
                            }
                            cv.notify_one();
                        }
                        else
                        {
                            size_t index_key = hash_filename(findFileData.cFileName);
                            T file_path = current_directory + L"\\" + findFileData.cFileName;
                            {
                                lock_guard<mutex> lock(index_mutex);
                                index[index_key].push_back(file_path);
                                file_count++;
                            }
                        }
                    } while (FindNextFile(hFind, &findFileData) != 0);

                    FindClose(hFind);
                }
            };

        int num_threads = thread::hardware_concurrency();
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(worker);
        }

        for (auto& t : threads)
        {
            if (t.joinable()) t.join();
        }

        done = true;
        cv.notify_all();
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

template <typename T>
void CHashing<T>::print_index(const map<size_t, vector<T>>& index, int file_count)
{
    cout << "Indexed " << file_count << " files." << endl;

    for (auto it = index.begin(); it != index.end(); it++)
    {
        cout << "Index key " << it->first << " : " << endl;
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            wcout << "  " << *it2 << endl;
        }
    }
}

// Explicit template instantiation
template class CHashing<wstring>;
