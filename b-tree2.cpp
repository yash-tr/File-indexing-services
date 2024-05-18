#include "DirectoryIndexer.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <chrono>
#include <exception>

// Derived class implementing btree-search method for directory indexing
template <typename K, typename V>
class BtreeSearchIndexer : public DirectoryIndexer<K, V>
{
public:
    void IndexDirectory(const std::wstring &directory, int &fileCount, int &subdirectoryCount, std::unordered_map<K, V> &fileIndex) override
    {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind;

        try
        {
            // Create a search path for all files and directories in the given directory
            std::wstring searchPath = directory + L"\\*";

            // Start searching for files and directories
            hFind = FindFirstFile(searchPath.c_str(), &findFileData);

            // Check if the search was successful
            if (INVALID_HANDLE_VALUE == hFind)
            {
                throw DirectoryIndexingException("Error in Finding File");
            }

            // If the search was successful, continue searching until no more files or directories are found
            if (hFind != INVALID_HANDLE_VALUE)
            {
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
                        // Recursively list all files and directories in the subdirectory
                        std::wstring subdirectory = directory + L"\\" + findFileData.cFileName;
                        subdirectoryCount++;
                        IndexDirectory(subdirectory, fileCount, subdirectoryCount, fileIndex);
                    }
                    else
                    {
                        // Increment file count
                        fileCount++;

                        // Generate a simple hash code for the file by summing the Unicode code point values of each character in the file name
                        std::wstring fileName = std::wstring(findFileData.cFileName);
                        int hashCode = 0;
                        for (int i = 0; i < fileName.length(); i++)
                        {
                            hashCode += (int)fileName[i];
                        }

                        // Add the file to the index using the hash code as the key
                        std::wstring filePath = directory + L"\\" + findFileData.cFileName;
                        fileIndex[std::to_string(hashCode)] = filePath;
                    }

                } while (FindNextFile(hFind, &findFileData) != 0); // Keep searching until no more files or directories are found

                // Check for errors in the search process
                if (GetLastError() != ERROR_NO_MORE_FILES)
                {
                    throw DirectoryIndexingException("Error in Finding Next File");
                }

                // Close the handle to the file search
                FindClose(hFind);
            }
        }
        catch (const DirectoryIndexingException &e)
        {
            if (hFind != INVALID_HANDLE_VALUE)
            {
                FindClose(hFind);
            }
            std::cerr << "Directory indexing error: " << e.what() << std::endl;
            throw;
        }
        catch (const std::exception &e)
        {
            if (hFind != INVALID_HANDLE_VALUE)
            {
                FindClose(hFind);
            }
            std::cerr << "General error: " << e.what() << std::endl;
            throw;
        }
        catch (...)
        {
            if (hFind != INVALID_HANDLE_VALUE)
            {
                FindClose(hFind);
            }
            std::cerr << "Unknown error occurred during directory indexing" << std::endl;
            throw;
        }
    }
};

// Explicit instantiation of the template classes
template class BtreeSearchIndexer<std::string, std::wstring>;

