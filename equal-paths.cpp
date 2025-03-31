#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
// bool checkLeafDepth(Node* root, int depth, int& leafDepth);

bool checkLeafDepth(Node* root, int depth, int& leafDepth) {
    if (!root) return true;

    if (!root->left && !root->right) {
        if (leafDepth == -1) {
            leafDepth = depth;  
        }
        return depth == leafDepth;  
    }

    // Recursively check both subtrees
    bool leftCheck = true, rightCheck = true;
    if (root->left) leftCheck = checkLeafDepth(root->left, depth + 1, leafDepth);
    if (root->right) rightCheck = checkLeafDepth(root->right, depth + 1, leafDepth);

    return leftCheck && rightCheck;
}

bool equalPaths(Node * root)
{
    // Add your code below
    if (!root) return true;
    
    int leafDepth = -1;  // Variable to track the first leaf depth
    return checkLeafDepth(root, 0, leafDepth);
}

