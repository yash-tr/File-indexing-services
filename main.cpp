#define UNICODE          // Set UNICODE to enable Unicode support on Windows
#include <iostream>      // Provides functions for input and output
#include <string>        // Provides string-related functions
#include <chrono>        // Provides time-related functions
#include <unordered_map> // Provides an unordered associative container
#include <map>           // Provides map container
#include <vector>        // Provides vector container
#include <Windows.h>     // Provides Windows-specific functions and data types
#include "binarysearchtree.h"

// Include the source files for the B-Tree, hashing, and search algorithms
#include "C:\\Users\\msi Katana\\source\\repos\\OS_P\\b-tree.cpp"
#include "C:\\Users\\msi Katana\\source\\repos\\OS_P\\hashing.cpp"
#include "C:\\Users\\msi Katana\\source\\repos\\OS_P\\search.cpp"
//#include "C:\\Users\\msi Katana\\source\\repos\\OS_P\\binarysearchtree.cpp"

// This is the main function of the program.
int main()
{
    // Prompt the user to choose between indexing or searching.
    std::cout << "Press 1 for indexing" << std::endl;
    std::cout << "Press 2 for searching" << std::endl;
    // Read user input for choice
    int choice1;
    std::cin >> choice1;

    // If the user chooses to index files
    if (choice1 == 1)
    {
        // Ask user to input the directory path
        std::wstring directory;
        std::cout << "Enter directory path: ";

        // Ignore any remaining newline characters from previous input
        std::cin.ignore();
        // Read directory path from user
        std::getline(std::wcin, directory);

        // Declare variables for file and subdirectory count
        int fileCount = 0;
        int subdirectoryCount = 0;

        // Ask user to choose indexing method
        int choice;
        std::cout << "Enter your choice:" << std::endl;
        std::cout << "Press 1 for indexing using binary tree" << std::endl;
        std::cout << "Press 2 for indexing using btree-search" << std::endl;
        std::cout << "Press 3 for indexing using hashing" << std::endl;
        // Switch statement to choose indexing method
        std::cin >> choice;

        switch (choice)
        {
            // If user chooses binary search indexing
        case 1:
        {
            // Create a binary search tree object
            BinarySearchTree<std::wstring> bst;

            // Record start time for indexing
            auto start = std::chrono::high_resolution_clock::now();

            // Index files in the directory using binary tree method
            vListFilesInDirectory(directory, fileCount, bst);

            // Record end time for indexing
            auto end = std::chrono::high_resolution_clock::now();

            // Calculate indexing duration in nanoseconds
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            // Print the indexed files using inorder traversal of the binary tree
            bst.traverse();

            // Print file count and indexing duration
            std::cout << "Total files: " << fileCount << std::endl;
            std::cout << "Time taken to index files: " << duration << " nanoseconds" << std::endl;

            break;
        }
        // If user chooses btree-search indexing
        case 2:
        {
            // Create a new instance of the BtreeSearchIndexer class
            BtreeSearchIndexer<std::string, std::wstring> indexer;

            // Declare variables to hold the index statistics
            std::unordered_map<std::string, std::wstring> fileIndex;

            // Start the timer
            auto start = std::chrono::high_resolution_clock::now();

            // Index the directory and its subdirectories using the BtreeSearchIndexer class
            indexer.IndexDirectory(directory, fileCount, subdirectoryCount, fileIndex);

            // Stop the timer
            auto stop = std::chrono::high_resolution_clock::now();

            // Calculate the duration of the indexing process
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Print the index statistics
            std::cout << "Indexed " << fileCount << " files in " << subdirectoryCount << " subdirectories." << std::endl;

            // Print the file index
            for (auto it = fileIndex.begin(); it != fileIndex.end(); it++)
            {
                std::string value = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(it->second);
                std::cout << "Key: " << it->first << ", Value: " << value << std::endl;
            }

            // Print the time taken to index the files
            std::cout << "Time taken to index files: " << duration << " nanoseconds" << std::endl;

            break;
        }
        // If user chooses hashing-search indexing
        case 3:
        {
            // Create a Hasing object
            CHashing<std::wstring> hashing;
            std::map<size_t, std::vector<std::wstring>> index;

            // Record the starting time of the indexing process
            auto start = std::chrono::high_resolution_clock::now();

            // Index files in the directory using hashing method
            hashing.vListFilesInDirectoryH(directory, index, fileCount);

            // Print the index
            hashing.print_index(index, fileCount);

            // Record the ending time of the indexing process
            auto end = std::chrono::high_resolution_clock::now();

            // Calculate the time taken to index the files
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            // Print the time taken to index the files
            std::cout << "Time taken to index files: " << duration << " nanoseconds" << std::endl;

            break;
        }
        // Default case if no valid choice is entered
        default:
        {
            std::cout << "Enter a valid choice" << std::endl;
            break;
        }
        }
    }
    // If the user chooses to search files
    else if (choice1 == 2)
    {
        // Create a DirectorySearch object and call its member functions
        DirectorySearch<std::string> search;
        search.getInput();    // Get user input
        search.searchFiles(); // Search for files
        search.printResult(); // Print the search result
    }
    else
    {
        std::cout << "Enter a valid choice" << std::endl;
    }

    return 0;
}
