// Set UNICODE to enable Unicode support on Windows
#define UNICODE
#include <iostream>      // Provides functions for input and output
#include <string>        // Provides string-related functions
#include <chrono>        // Provides time-related functions
#include <unordered_map> // Provides an unordered associative container
#include <Windows.h>     // Provides Windows-specific functions and data types

using namespace std;

// Define an abstract class for tree operations
template <typename T>
class CAbstract
{
public:
    virtual void traverse() = 0;
    virtual void insert(T data) = 0;
};

// Define a class for a node in a binary search tree
template <typename T>
class CNode : public CAbstract<T>
{
public:
    T data;          // The data of the node
    CNode<T> *left;  // Pointer to the left child node
    CNode<T> *right; // Pointer to the right child node

    // Constructor to create a new node with the given data
    CNode(T data) : data(data), left(nullptr), right(nullptr) {}

    // Method to traverse the tree in-order and print the data
    void traverse() override
    {
        if (left != nullptr)
            left->traverse();

        wcout << data << endl;

        if (right != nullptr)
            right->traverse();
    }

    // Method to insert a new data into the tree
    void insert(T newData) override
    {
        if (newData < data)
        {
            if (left == nullptr)
                left = new CNode(newData);
            else
                left->insert(newData);
        }
        else
        {
            if (right == nullptr)
                right = new CNode(newData);
            else
                right->insert(newData);
        }
    }
};

// Define a class for a binary search tree
template <typename T>
class BinarySearchTree : public CAbstract<T>
{
public:
    CNode<T> *root; // Pointer to the root node of the tree

    // Constructor to create a new empty tree
    BinarySearchTree() : root(nullptr) {}

    // Method to traverse the tree in-order and print the data
    void traverse() override
    {
        if (root != nullptr)
            root->traverse();
    }

    // Method to insert a new data into the tree
    void insert(T newData) override
    {
        if (root == nullptr)
            root = new CNode<T>(newData);
        else
            root->insert(newData);
    }
};

// Function to list all files in a directory and its subdirectories
void vListFilesInDirectory(const wstring &directory, int &fileCount, BinarySearchTree<wstring> &bst)
{
    WIN32_FIND_DATA findFileData; // Structure to hold information about a file
    HANDLE hFind;                 // Handle to the search operation

    // Search for all files and directories in the given directory
    wstring searchPath = directory + L"\\*";
    hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            try
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
                    wstring subdirectory = directory + L"\\" + findFileData.cFileName;
                    vListFilesInDirectory(subdirectory, fileCount, bst);
                }
                else
                {
                    // Increment file count and add file to binary search tree
                    fileCount++;
                    bst.insert(directory + L"\\" + findFileData.cFileName);
                }
            }
            catch (const exception &e)
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
