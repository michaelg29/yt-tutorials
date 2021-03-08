#include "avl.h"

#include <stdlib.h>
#include <string.h>

/*
    allocation
*/

// create root with key and value
avl* avl_createRoot(void* key, void* val, int (*keycmp)(void* val1, void* val2))
{
    avl* ret = (avl*)malloc(sizeof(avl));

    if (!ret)
    {
        return NULL;
    }

    ret->key = key;
    ret->val = val;
    ret->height = 1; // initially a leaf node

    ret->left = NULL;
    ret->right = NULL;

    ret->keycmp = keycmp;

    return ret;
}

// create root with null key and value
avl* avl_createEmptyRoot(int (*keycmp)(void* val1, void* val2))
{
    return avl_createRoot(NULL, NULL, keycmp);
}

/*
    accessor functions
*/

// get the smallest key in a subtree
avl* avl_smallestKey(avl* root)
{
    return root->left ? avl_smallestKey(root->left) : root;
}

// get the height of a root
int avl_height(avl* root)
{
    return root ? root->height : 0;
}

// get the balance factor of a root
int avl_balanceFactor(avl* root)
{
    return root ? avl_height(root->left) - avl_height(root->right) : 0;
}

/*
    operation functions
*/

// insert into avl
avl* avl_insert(avl* root, void* key, void* val)
{
    // insert as in bst
    avl* newRoot = avl_bst_insert(root, key, val);

    // rebalance if necessary
    newRoot = avl_rebalance(newRoot);

    return newRoot;
}

// insert into bst
avl* avl_bst_insert(avl* root, void* key, void* val)
{
    if (!root->key)
    {
        // null values, simply assign
        root->key = key;
        root->val = val;
        root->height = 1;
        return root;
    }

    if (root->keycmp(key, root->key) < 0)
    {
        // new key less than stored key, insert left
        if (root->left)
        {
            // traverse down
            root->left = avl_insert(root->left, key, val);
        }
        else
        {
            // create new node
            root->left = avl_createRoot(key, val, root->keycmp);
        }
    }
    else
    {
        // new key greater than or equal to stored key, insert right
        if (root->right)
        {
            // traverse down
            root->right = avl_insert(root->right, key, val);
        }
        else
        {
            // create new node
            root->right = avl_createRoot(key, val, root->keycmp);
        }
    }

    avl_recalcHeight(root);

    return root;
}

// remove from avl
avl* avl_remove(avl* root, void* key)
{
    // remove as in BST
    avl* newRoot = avl_bst_remove(root, key);

    // rebalance if necessary
    newRoot = avl_rebalance(newRoot);

    return newRoot;
}

// remove from bst
avl* avl_bst_remove(avl* root, void* key)
{
    int cmp = root->keycmp(key, root->key);
    if (cmp < 0)
    {
        // target less than stored key, look left
        if (root->left)
        {
            // traverse left
            root->left = avl_remove(root->left, key);
            // recalculate heights
            avl_recalcHeight(root->left);
        }
    }
    else if (cmp > 0)
    {
        // target greater than stored key, look right
        if (root->right)
        {
            // traverse right
            root->right = avl_remove(root->right, key);
            avl_recalcHeight(root->right);
        }
    }
    else
    {
        // target key found

        // 3 cases
        /*
            1) leaf node
                - simply delete
            2) has one child
                - substitute with the child
            3) has both children
                - substitute with in-order successor
                - recursively delete the in-order successor from the right subtree
        */

        // Case 1
        if (!root->left && !root->right)
        {
            free(root);
            return NULL;
        }

        // Case 2
        if (root->left && !root->right)
        {
            // only child is left
            avl* ret = root->left;
            free(root);
            return ret;
        }
        else if (!root->left && root->right)
        {
            // only child is right
            avl* ret = root->right;
            free(root);
            return ret;
        }

        // Case 3
        avl* sub = avl_smallestKey(root->right);
        // copy values over
        root->key = sub->key;
        root->val = sub->val;
        // recursively delete
        root = avl_remove(root->right, root->key);
    }

    avl_recalcHeight(root);

    return root;
}

// rebalance the avl
avl* avl_rebalance(avl* root)
{
    // get balance factor
    int bf = avl_balanceFactor(root);
    if (bf < -1 || bf > 1)
    {
        // 4 cases
        /*
            1) Left-left: left heavy, left child is balanced or left heavy
                - right rotation around root
            2) Left-right: left heavy, left child is right heavy
                - left rotation around left child
                - now left-left, so rotate right around the root
            3) Right-right: right heavy, right child is balanced or right heavy
                - left rotation around the root
            4) Right-left: right heavy, right child is left heavy
                - right rotation around the right child
                - now right-right, so rotate left around the root
        */

        // Case 1
        if (bf > 1 && avl_balanceFactor(root->left) >= 0)
        {
            root = avl_rotateRight(root);
        }
        // Case 2
        else if (bf > 1)
        {
            root->left = avl_rotateLeft(root->left);
            root = avl_rotateRight(root);
        }
        // Case 3
        else if (bf < -1 && avl_balanceFactor(root->right) <= 0)
        {
            root = avl_rotateLeft(root);
        }
        // Case 4
        else
        {
            root->right = avl_rotateRight(root->right);
            root = avl_rotateLeft(root);
        }
    }

    return root;
}

// rotate right around a root
avl* avl_rotateRight(avl* root)
{
    if (!root->left)
    {
        return root;
    }

    avl* newRoot = root->left;
    root->left = newRoot->right;
    newRoot->right = root;

    // recalculate heights
    avl_recalcHeight(root);
    avl_recalcHeight(newRoot);

    return newRoot;
}

// rotate left around a root
avl* avl_rotateLeft(avl* root)
{
    if (!root->right)
    {
        return root;
    }

    avl* newRoot = root->right;
    root->right = newRoot->left;
    newRoot->left = root;

    // recalculate height
    avl_recalcHeight(root);
    avl_recalcHeight(newRoot);

    return newRoot;
}

// calcualte the height of a root considering the children's heights
void avl_recalcHeight(avl* root)
{
    if (root)
    {
        root->height = 1 + MAX(avl_height(root->left), avl_height(root->right));
    }
}

// get the value stored with a key
void* avl_get(avl* root, void* key)
{
    if (!root)
    {
        return NULL;
    }

    char cmp = root->keycmp(key, root->key);
    if (!cmp) // cmp == 0
    {
        // found key
        return root->val;
    }
    else if (cmp > 0)
    {
        // traverse right
        return root->right ? avl_get(root->right, key) : NULL;
    }
    else // cmp < 0
    {
        // traverse left
        return root->left ? avl_get(root->left, key) : NULL;
    }
}

/*
    traversal methods
*/

// inorder traversal
void avl_inorderTraverse(avl* root, void (*visit)(avl* node))
{
    if (root->left)
    {
        avl_inorderTraverse(root->left, visit);
    }

    visit(root);

    if (root->right)
    {
        avl_inorderTraverse(root->right, visit);
    }
}

// preorder traversal
void avl_preorderTraverse(avl* root, void (*visit)(avl* node))
{
    visit(root);

    if (root->left)
    {
        avl_preorderTraverse(root->left, visit);
    }
    if (root->right)
    {
        avl_preorderTraverse(root->right, visit);
    }
}

// postorder traversal
void avl_postorderTraverse(avl* root, void (*visit)(avl* node))
{
    if (root->left)
    {
        avl_postorderTraverse(root->left, visit);
    }
    if (root->right)
    {
        avl_postorderTraverse(root->right, visit);
    }

    visit(root);
}

/*
    freeing methods
*/

// free node memory
void avl_deleteNode(avl* node)
{
    free(node);
}

// free key, value, and node memory
void avl_deleteNodeDeep(avl* node)
{
    free(node->key);
    free(node->val);

    free(node);
}

// free subtree memory
void avl_free(avl* root)
{
    avl_postorderTraverse(root, avl_deleteNode);
}

// free subtree memory, including keys and values
void avl_freeDeep(avl* root)
{
    avl_postorderTraverse(root, avl_deleteNodeDeep);
}

/*
    default key comparisons
    format:
        int <name>cmp(void *key1, void *key2);
*/

// string keys
int strkeycmp(void* key1, void* key2)
{
    return strcmp((char*)key1, (char*)key2);
}

// integer keys
int intkeycmp(void* key1, void* key2)
{
    int i1 = *((int*)key1);
    int i2 = *((int*)key2);

    if (i1 > i2)
    {
        return 1;
    }
    else if (i1 == i2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}