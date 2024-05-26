#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "dirent.h"
#include <cstring>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

// Template class for file search
template <typename T>
class FileSearch
{
protected:
    DIR* directory;
    struct dirent* entry;
    T searchString;
    string directoryPath;
    bool resultFound;
    int entryCount;
    mutex mtx; // Mutex to protect shared resources

public:
    FileSearch()
    {
        entryCount = 0;
        resultFound = false;
    }

    virtual void getInput() = 0;
    virtual void searchFiles() = 0;
    virtual void printResult() = 0;
};

// Derived class for searching files in a directory
template <typename T>
class DirectorySearch : public FileSearch<T>
{
public:
    void getInput() override
    {
        cout << "Please enter the path to search: " << endl;
        cin >> this->directoryPath;

        cout << "Please enter the string to search: " << endl;
        cin >> this->searchString;
    }

    void searchFiles() override
    {
        auto startTime = high_resolution_clock::now();

        try
        {
            if ((this->directory = opendir(this->directoryPath.c_str())) == NULL)
            {
                throw runtime_error("Error: Cannot open directory!");
            }

            while ((this->entry = readdir(this->directory)) != NULL)
            {
                thread t(&DirectorySearch::processEntry, this, this->entry);
                t.join(); // Wait for the thread to complete
            }

            if (this->entryCount == 0)
            {
                throw runtime_error("Error: No entries found in directory!");
            }

            if (!this->resultFound)
            {
                throw runtime_error("Error: Search string not found in any entries!");
            }

            closedir(this->directory);

            auto stopTime = high_resolution_clock::now();
            auto duration = duration_cast<nanoseconds>(stopTime - startTime);

            cout << "\nSearching time: " << duration.count() << " nanoseconds" << endl;
        }
        catch (const exception& e)
        {
            cerr << e.what() << endl;
            if (this->directory != NULL)
            {
                closedir(this->directory);
            }
        }
    }

    void processEntry(struct dirent* entry)
    {
        lock_guard<mutex> lock(this->mtx);
        this->entryCount++;
        // Convert searchString from std::string to const char* before using strstr
        if (strstr(entry->d_name, this->searchString.c_str()))
        {
            cout << entry->d_name << endl;
            this->resultFound = true;
        }
    }

    void printResult() override
    {
        if (this->resultFound)
        {
            cout << "\nFile(s) is present" << endl;
        }
        else
        {
            cout << "\nFile(s) is not present" << endl;
        }
    }
};