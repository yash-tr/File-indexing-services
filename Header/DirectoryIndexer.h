#ifndef DIRECTORY_INDEXER_H // Header guard to prevent multiple inclusion of the same header
#define DIRECTORY_INDEXER_H

#include <Windows.h> // Windows header for WinAPI functions and data types
#include <string> // Standard string manipulation library
#include <unordered_map> // Standard unordered map container library

// Exception class for directory indexing errors
class DirectoryIndexingException : public std::exception // Define an exception class derived from std::exception
{
public:
    DirectoryIndexingException(const std::string &message) : msg(message) {} // Constructor to initialize the exception message

    const char *what() const noexcept override // Override the what() method to return the exception message
    {
        return msg.c_str(); // Return the exception message as a C-style string
    }

private:
    std::string msg; // Member variable to store the exception message
};

// Abstract base class for directory indexing using templates
template <typename K, typename V>
class DirectoryIndexer
{
public:
    // Pure virtual function to index a directory; to be implemented by derived classes
    virtual void IndexDirectory(const std::wstring &directory, int &fileCount, int &subdirectoryCount, std::unordered_map<K, V> &fileIndex) = 0;
};

#endif // End of header guard
