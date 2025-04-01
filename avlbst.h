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
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
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
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* node, int diff);
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (!this->root_) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);

    while (current) {
        parent = current;
        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        } else if (new_item.first > current->getKey()) {
            current = current->getRight();
        } else {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }

    insertFix(parent, newNode);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child)
{
    if (!parent) return;
    
    if (child == parent->getLeft()) {
        parent->updateBalance(-1);
    } else {
        parent->updateBalance(1);
    }
    
    int balance = parent->getBalance();
    if (balance == 0) return;
    
    if (balance == -1 || balance == 1) {
        insertFix(parent->getParent(), parent);
    }
    else if (balance == -2) {
        if (parent->getLeft()->getBalance() == 1) {
            rotateLeft(parent->getLeft());
        }
        rotateRight(parent);
    }
    else if (balance == 2) {
        if (parent->getRight()->getBalance() == -1) {
            rotateRight(parent->getRight());
        }
        rotateLeft(parent);
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (!node) return;
    AVLNode<Key, Value>* parent = node->getParent();
    int diff = 0;
    if (node->getLeft() && node->getRight()) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(node));
        nodeSwap(node, pred);
        parent = node->getParent(); // update parent pointer after swap
    }
    AVLNode<Key, Value>* child = node->getLeft() ? node->getLeft() : node->getRight();
    if (child)
        child->setParent(parent);
    if (!parent)
        this->root_ = child;
    else {
        if (parent->getLeft() == node) {
            parent->setLeft(child);
            diff = 1;
        } else {
            parent->setRight(child);
            diff = -1;
        }
    }
    delete node;
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    if (!node) return;
    
    AVLNode<Key, Value>* parent = node->getParent();
    
    node->updateBalance(diff);
    int balance = node->getBalance();
    
    if (balance == -2) {
        AVLNode<Key, Value>* left = node->getLeft();
        int leftBalance = left->getBalance();
        if (leftBalance <= 0) {
            rotateRight(node);
            if (leftBalance == 0)
                return;
        } else {
            rotateLeft(left);
            rotateRight(node);
        }
    }
    else if (balance == 2) {
        AVLNode<Key, Value>* right = node->getRight();
        int rightBalance = right->getBalance();
        if (rightBalance >= 0) {
            rotateLeft(node);
            if (rightBalance == 0)
                return;
        } else {
            rotateRight(right);
            rotateLeft(node);
        }
    }
    else if (balance != 0) {
        return;
    }
    if (parent)
        removeFix(parent, (parent->getLeft() == node) ? 1 : -1);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* parent = x->getParent();
    x->setRight(y->getLeft());
    if (y->getLeft())
        y->getLeft()->setParent(x);
    y->setLeft(x);
    x->setParent(y);
    y->setParent(parent);
    if (!parent)
        this->root_ = y;
    else if (parent->getLeft() == x)
        parent->setLeft(y);
    else
        parent->setRight(y);
    int8_t oldX = x->getBalance();
    int8_t oldY = y->getBalance();
    x->setBalance(oldX - 1 - std::max(oldY, static_cast<int8_t>(0)));
    y->setBalance(oldY - 1 + std::min(x->getBalance(), static_cast<int8_t>(0)));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getLeft();
    AVLNode<Key, Value>* parent = x->getParent();
    x->setLeft(y->getRight());
    if (y->getRight())
        y->getRight()->setParent(x);
    y->setRight(x);
    x->setParent(y);
    y->setParent(parent);
    if (!parent)
        this->root_ = y;
    else if (parent->getLeft() == x)
        parent->setLeft(y);
    else
        parent->setRight(y);
    int8_t oldX = x->getBalance();
    int8_t oldY = y->getBalance();
    x->setBalance(oldX + 1 - std::min(oldY, static_cast<int8_t>(0)));
    y->setBalance(oldY + 1 + std::max(x->getBalance(), static_cast<int8_t>(0)));
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
