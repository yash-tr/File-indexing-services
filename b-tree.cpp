#include "DirectoryIndexer.h"

// Mutex to synchronize access to shared data structures
std::mutex mtx1;

// DirectoryIndexingException implementation
DirectoryIndexingException::DirectoryIndexingException(const std::string& message) : msg(message) {}

const char* DirectoryIndexingException::what() const noexcept
{
    return msg.c_str();
}

// BtreeSearchIndexer implementation
template <typename K, typename V>
void BtreeSearchIndexer<K, V>::IndexDirectory(const std::wstring& directory, int& fileCount, int& subdirectoryCount, stx::btree_map<K, V>& fileIndex)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;  // Initialize hFind to INVALID_HANDLE_VALUE

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
            // Vector to store subdirectory paths for multithreading
            std::vector<std::wstring> subdirectories;

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
                    // Add subdirectory to the list for multithreading
                    std::wstring subdirectory = directory + L"\\" + findFileData.cFileName;
                    subdirectories.push_back(subdirectory);
                }
                else
                {
                    // Increment file count
                    fileCount++;

                    // Generate a simple hash code for the file by summing the Unicode code point values of each character in the file name
                    std::wstring fileName = std::wstring(findFileData.cFileName);
                    int hashCode = 0;
                    for (size_t i = 0; i < fileName.length(); i++)
                    {
                        hashCode += static_cast<int>(fileName[i]);
                    }

                    // Add the file to the index using the hash code as the key
                    std::wstring filePath = directory + L"\\" + findFileData.cFileName;
                    std::lock_guard<std::mutex> lock(mtx1); // Lock mutex for thread-safe access
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

            // Create threads for indexing subdirectories
            std::vector<std::thread> threads;
            for (const auto& subdirectory : subdirectories)
            {
                subdirectoryCount++;
                threads.emplace_back(&BtreeSearchIndexer::IndexDirectory, this, subdirectory, std::ref(fileCount), std::ref(subdirectoryCount), std::ref(fileIndex));
            }

            // Wait for all threads to complete
            for (auto& t : threads)
            {
                t.join();
            }
        }
    }
    catch (const DirectoryIndexingException& e)
    {
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FindClose(hFind);
        }
        std::cerr << "Directory indexing error: " << e.what() << std::endl;
        throw;
    }
    catch (const std::exception& e)
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

// Explicit template instantiation
template class BtreeSearchIndexer<std::string, std::wstring>;
