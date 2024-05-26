#include "hashing.h"

// Hash function that returns an index for a given filename
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

// Function to list files in a directory, hash their names, and store them in the index
template <typename T>
void CHashing<T>::vListFilesInDirectoryH(const T& directory, map<size_t, vector<T>>& index, int& file_count)
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

        // Queue to manage directories for multi-threading
        queue<T> directories;
        directories.push(directory);

        // Vector to hold threads
        vector<thread> threads;
        // Mutex for thread-safe access to shared resources
        mutex index_mutex;
        // Condition variable for thread synchronization
        condition_variable cv;
        // Flag to indicate if all directories are processed
        bool done = false;

        // Lambda function to be run by each thread
        auto worker = [&]()
            {
                while (true)
                {
                    T current_directory;

                    // Lock mutex and wait for work
                    {
                        unique_lock<mutex> lock(index_mutex);
                        cv.wait(lock, [&]() { return !directories.empty() || done; });

                        // If done and no more directories, exit the thread
                        if (done && directories.empty()) return;

                        // Get the next directory to process
                        current_directory = directories.front();
                        directories.pop();
                    }

                    T searchPath = current_directory + L"\\*";
                    hFind = FindFirstFile(searchPath.c_str(), &findFileData);

                    if (hFind == INVALID_HANDLE_VALUE) continue;

                    do
                    {
                        // Ignore "." and ".." directories
                        if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) continue;

                        // Check if the file is a directory
                        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                        {
                            // Add subdirectory to the queue
                            T subdirectory = current_directory + L"\\" + findFileData.cFileName;
                            {
                                lock_guard<mutex> lock(index_mutex);
                                directories.push(subdirectory);
                            }
                            cv.notify_one(); // Notify one waiting thread
                        }
                        else
                        {
                            // Hash filename to get the index key
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

        // Determine the number of threads to use
        int num_threads = thread::hardware_concurrency();
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(worker);
        }

        // Join all threads to wait for their completion
        for (auto& t : threads)
        {
            if (t.joinable()) t.join();
        }

        // Indicate that all work is done and notify all threads
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

// Function to print the indexed files
template <typename T>
void CHashing<T>::print_index(const map<size_t, vector<T>>& index, int file_count)
{
    cout << "Indexed " << file_count << " files." << endl;

    // Print the index
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
//template class CHashing<wstring>;
//
//int main()
//{
//    // Create an instance of CHashing for wstring
//    CHashing<wstring> hasher;
//    map<size_t, vector<wstring>> index;
//    int file_count = 0;
//    wstring directory = L"C:\\path\\to\\your\\directory"; // Change this to the directory you want to index
//
//    // Measure the time taken to index files
//    auto start = chrono::high_resolution_clock::now();
//    hasher.vListFilesInDirectoryH(directory, index, file_count);
//    auto end = chrono::high_resolution_clock::now();
//
//    // Calculate and print the elapsed time
//    chrono::duration<double> elapsed = end - start;
//    cout << "Time taken: " << elapsed.count() << " seconds." << endl;
//
//    // Print the index
//    hasher.print_index(index, file_count);
//
//    return 0;
//}
