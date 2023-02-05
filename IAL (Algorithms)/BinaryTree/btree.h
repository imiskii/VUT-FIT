/**
 * @file btree.h
 * @author VUT FIT - IAL
 * @brief header file for btree.c
 * 
 */

#ifndef IAL_BTREE_H
#define IAL_BTREE_H

#include <stdbool.h>

/* Tree node */
typedef struct bst_node {
  char key;               // key
  int value;              // value
  struct bst_node *left;  // left child
  struct bst_node *right; // right child
} bst_node_t;


/**
 * @brief Function initialize new binary tree
 * 
 * @param tree pointer to tree structure
 */
void bst_init(bst_node_t **tree);


/**
 * @brief Function for adding new node
 * If node with same key already exist then replace value
 * 
 * @param tree pointer to initialized binary tree
 * @param key key of node that will be inserted
 * @param value value of new created node
 */
void bst_insert(bst_node_t **tree, char key, int value);


/**
 * @brief Function find value in tree
 * 
 * @param tree pointer to initialized binary tree
 * @param key key of searching node
 * @param value this parameter will be set with value of node with given key if was found
 * @return true if node with given key was found
 * @return false if node with given key was not found
 */
bool bst_search(bst_node_t *tree, char key, int *value);


/**
 * @brief Function delete node with given key
 * If node with given key does not exist function do nothing
 * 
 * @param tree pointer to initialized binary tree
 * @param key key of node that will be deleted
 */
void bst_delete(bst_node_t **tree, char key);


/**
 * @brief Function delete all nodes in tree
 * 
 * @param tree pointer to initialized binary tree
 */
void bst_dispose(bst_node_t **tree);


/**
 * @brief Preoreder traversal
 * 
 * @param tree pointer to initialized binary tree
 */
void bst_preorder(bst_node_t *tree);


/**
 * @brief Inorder traversal
 * 
 * @param tree pointer to initialized binary tree
 */
void bst_inorder(bst_node_t *tree);


/**
 * @brief Postorder traversal
 * 
 * @param tree pointer to initialized binary tree
 */
void bst_postorder(bst_node_t *tree);



/**
 * @brief Function replace right most node with given node
 * - this function is used in bst_delete() function
 * 
 * @param target node that will be replaced
 * @param tree pointer to initialized binary tree
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree);



void bst_print_node(bst_node_t *node);


#endif // IAL_BTREE_H

/* END OF FILE */