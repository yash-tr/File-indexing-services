#pragma once
#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

#include <string>
#include <Windows.h>
#include <iostream>
using namespace std;

template <typename T>
class CAbstract
{
public:
    virtual void traverse() = 0;
    virtual void insert(T data) = 0;
};

template <typename T>
class CNode : public CAbstract<T>
{
public:
    T data;
    CNode<T>* left;
    CNode<T>* right;

    CNode(T data) : data(data), left(nullptr), right(nullptr) {}

    void traverse() override
    {
        if (left != nullptr)
            left->traverse();
        wcout << data << endl;
        if (right != nullptr)
            right->traverse();
    }

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

template <typename T>
class BinarySearchTree : public CAbstract<T>
{
public:
    CNode<T>* root;

    BinarySearchTree() : root(nullptr) {}

    void traverse() override
    {
        if (root != nullptr)
            root->traverse();
    }

    void insert(T newData) override
    {
        if (root == nullptr)
            root = new CNode<T>(newData);
        else
            root->insert(newData);
    }
};

void vListFilesInDirectory(const wstring& directory, int& fileCount, BinarySearchTree<wstring>& bst);

#endif // BINARYSEARCHTREE_H
