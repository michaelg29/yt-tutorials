#ifndef AVL_H
#define AVL_H

// max function macro
#define MAX(a, b) (a > b ? a : b)

// avl structure
typedef struct avl
{
    void* key;
    void* val;
    int height;

    struct avl* left;
    struct avl* right;

    int (*keycmp)(void* key1, void* key2);
} avl;

/*
    allocation
*/
// create root with key and value
avl* avl_createRoot(void* key, void* val, int (*keycmp)(void* val1, void* val2));
// create root with null key and value
avl* avl_createEmptyRoot(int (*keycmp)(void* val1, void* val2));

/*
    accessor functions
*/
// get the smallest key in a subtree
avl* avl_smallestKey(avl* root);
// get the height of a root
int avl_height(avl* root);
// get the balance factor of a root
int avl_balanceFactor(avl* root);

/*
    operation functions
*/
// insert into avl
avl* avl_insert(avl* root, void* key, void* val);
// insert into bst
avl* avl_bst_insert(avl* root, void* key, void* val);
// remove from avl
avl* avl_remove(avl* root, void* key);
// remove from bst
avl* avl_bst_remove(avl* root, void* key);
// rebalance the avl
avl* avl_rebalance(avl* root);
// rotate right around a root
avl* avl_rotateRight(avl* root);
// rotate left around a root
avl* avl_rotateLeft(avl* root);
// calcualte the height of a root considering the children's heights
void avl_recalcHeight(avl* root);
// get the value stored with a key
void* avl_get(avl* root, void* key);

/*
    traversal methods
*/
// inorder traversal
void avl_inorderTraverse(avl* root, void (*visit)(avl* node));
// preorder traversal
void avl_preorderTraverse(avl* root, void (*visit)(avl* node));
// postorder traversal
void avl_postorderTraverse(avl* root, void (*visit)(avl* node));

/*
    freeing methods
*/
// free node memory
void avl_deleteNode(avl* node);
// free key, value, and node memory
void avl_deleteNodeDeep(avl* node);
// free subtree memory
void avl_free(avl* root);
// free subtree memory, including keys and values
void avl_freeDeep(avl* root);

/*
    default key comparisons
    format:
        int <name>cmp(void *key1, void *key2);
*/
// string keys
int strkeycmp(void* key1, void* key2);
// integer keys
int intkeycmp(void* key1, void* key2);

#endif