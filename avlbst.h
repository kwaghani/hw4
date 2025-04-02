#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int getHeight () const;
    void setHeight (int height);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int height_;
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), height_(1)
{

}

template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

template<class Key, class Value>
int AVLNode<Key, Value>::getHeight() const
{
    return height_;
}

template<class Key, class Value>
void AVLNode<Key, Value>::setHeight(int height)
{
    height_ = height;
}

template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item);
    virtual void remove(const Key& key);
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void updateHeight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
    int getBalanceFactor(AVLNode<Key, Value>* node);
};

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item)
{
    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    if(this->root_ == NULL)
    {
        this->root_ = node;
        return;
    }

    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* tracker = NULL;

    while(current != NULL)
    {
        tracker = current;
        if(new_item.first < current->getKey())
        {
            current = current->getLeft();   
        }
        else if(new_item.first > current->getKey())
        {
            current = current->getRight();
        }
        else
        {
            current->setValue(new_item.second);
            delete node;
            return;
        }
    }

    node->setParent(tracker);
    if(new_item.first < tracker->getKey())
    {
        tracker->setLeft(node);
    }
    else
    {
        tracker->setRight(node);
    }

    insertFix(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(node == NULL) 
    {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int balance = 0;

    if(node->getLeft() != NULL && node->getRight() != NULL)
    {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred);
        parent = node->getParent();
    }

    AVLNode<Key, Value>* child = node->getLeft() ? node->getLeft() : node->getRight();
    if(child != NULL)
    {
        child->setParent(parent);
    }

    if(parent == NULL)
    {
        this->root_ = child;
    }
    else if(parent->getLeft() == node)
    {
        parent->setLeft(child);
        balance = 1;
    }
    else
    {
        parent->setRight(child);
        balance = -1;
    }

    delete node;
    removeFix(parent, balance);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* node)
{
    while(node != NULL)
    {
        updateHeight(node);
        int balance = getBalanceFactor(node);

        if(balance == 2)
        {
            if(getBalanceFactor(node->getLeft()) == -1)
                rotateLeft(node->getLeft());
            rotateRight(node);
        }
        else if(balance == -2)
        {
            if(getBalanceFactor(node->getRight()) == 1)
                rotateRight(node->getRight());
            rotateLeft(node);
        }

        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int balance)
{
    while(node != NULL)
    {
        updateHeight(node);
        int balance = getBalanceFactor(node);

        if(balance == 2)
        {
            if(getBalanceFactor(node->getLeft()) < 0)
                rotateLeft(node->getLeft());
            rotateRight(node);
        }
        else if(balance == -2)
        {
            if(getBalanceFactor(node->getRight()) > 0)
                rotateRight(node->getRight());
            rotateLeft(node);
        }

        if(std::abs(balance) <= 1)
        {
            node = node->getParent();
        }
        else break;
    }
}

template<class Key, class Value>
int AVLTree<Key, Value>::getBalanceFactor(AVLNode<Key, Value>* node)
{
    if(node == NULL) return 0;

    int leftHeight = 0;
    if(node->getLeft())
        leftHeight = node->getLeft()->getHeight();

    int rightHeight = 0;
    if(node->getRight())
        rightHeight = node->getRight()->getHeight();

    return leftHeight - rightHeight;
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateHeight(AVLNode<Key, Value>* node)
{
    if(node != NULL)
    {
        int leftHeight = 0;
        if(node->getLeft())
            leftHeight = node->getLeft()->getHeight();

        int rightHeight = 0;
        if(node->getRight())
            rightHeight = node->getRight()->getHeight();

        int max = (leftHeight > rightHeight) ? leftHeight : rightHeight;
        node->setHeight(1 + max);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightc = node->getRight();
    if(rightc == NULL) return;

    node->setRight(rightc->getLeft());
    if(rightc->getLeft())
        rightc->getLeft()->setParent(node);

    rightc->setParent(node->getParent());

    if(node->getParent() == NULL)
        this->root_ = rightc;
    else if(node == node->getParent()->getLeft())
        node->getParent()->setLeft(rightc);
    else
        node->getParent()->setRight(rightc);

    rightc->setLeft(node);
    node->setParent(rightc);

    updateHeight(node);
    updateHeight(rightc);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* leftc = node->getLeft();
    if(leftc == NULL) return;

    node->setLeft(leftc->getRight());
    if(leftc->getRight())
        leftc->getRight()->setParent(node);

    leftc->setParent(node->getParent());

    if(node->getParent() == NULL)
        this->root_ = leftc;
    else if(node == node->getParent()->getRight())
        node->getParent()->setRight(leftc);
    else
        node->getParent()->setLeft(leftc);

    leftc->setRight(node);
    node->setParent(leftc);

    updateHeight(node);
    updateHeight(leftc);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int tempH = n1->getHeight();
    n1->setHeight(n2->getHeight());
    n2->setHeight(tempH);
}

#endif