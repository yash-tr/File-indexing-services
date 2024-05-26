#include "binarysearchtree.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;

mutex mtx;

void vListFilesInDirectory(const wstring& directory, int& fileCount, BinarySearchTree<wstring>& bst)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    wstring searchPath = directory + L"\\*";
    hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            try
            {
                if (wcscmp(findFileData.cFileName, L".") == 0 ||
                    wcscmp(findFileData.cFileName, L"..") == 0)
                {
                    continue;
                }

                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    wstring subdirectory = directory + L"\\" + findFileData.cFileName;

                    thread t([&subdirectory, &fileCount, &bst]() {
                        vListFilesInDirectory(subdirectory, fileCount, bst);
                        });
                    t.join();
                }
                else
                {
                    lock_guard<mutex> lock(mtx);
                    fileCount++;
                    bst.insert(directory + L"\\" + findFileData.cFileName);
                }
            }
            catch (const exception& e)
            {
                cerr << "Exception: " << e.what() << endl;
            }
            catch (...)
            {
                cerr << "Unknown exception occurred." << endl;
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }
}