/**
 * @file btree.c
 * @author Michal Ľaš
 * @brief Binary search tree recurzive variant
 * 
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>


void bst_init(bst_node_t **tree) {
  *tree = NULL;
}


bool bst_search(bst_node_t *tree, char key, int *value) {
      
  if (tree != NULL){
    if (tree->key == key){
      *value = tree->value;
      return true;
    }
    else{
      if (key < tree->key){
        return bst_search(tree->left, key, value);
      }
      else{
        return bst_search(tree->right, key, value);
      }
    }
  }
  else{
    return false;
  }
}


void bst_insert(bst_node_t **tree, char key, int value) {
  if (*tree == NULL){               // Create new node
    (*tree) = malloc(sizeof(struct bst_node));
    (*tree)->key = key;
    (*tree)->value = value;
    (*tree)->left = NULL;
    (*tree)->right = NULL;
  }
  else{
    if ((*tree)->key == key){       // if there is given key rewrite value
      (*tree)->value = value;
    }
    else{
      if (key < (*tree)->key){     // if key does not exist continue
        bst_insert(&(*tree)->left, key, value);
        return; 
      }
      else{
        bst_insert(&(*tree)->right, key, value);
        return;
      }
    }
  }
}


void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {

  if ((*tree) != NULL){
    if ((*tree)->right != NULL){
      bst_replace_by_rightmost(target, &(*tree)->right);
    }
    else{
      target->key = (*tree)->key;
      target->value = (*tree)->value;

      bst_node_t *tmp = (*tree)->left;
      free(*tree);
      (*tree) = tmp;
    }
  }
}


void bst_delete(bst_node_t **tree, char key) {
  if ((*tree) != NULL){
    // Search node for deletion
    if (key < (*tree)->key){
      bst_delete(&(*tree)->left, key);
    }
    else if (key > (*tree)->key){
      bst_delete(&(*tree)->right, key);
    }
    // Find node
    else{
      if (((*tree)->left == NULL) && ((*tree)->right == NULL)){ // node is leaf
        free((*tree));
        (*tree) = NULL;
        return;
      }
      else{
        if (((*tree)->left != NULL) && ((*tree)->right != NULL)){ // node has right and left child
          bst_replace_by_rightmost((*tree), &(*tree)->left);
        }
        else{   // node has just one child
          bst_node_t *tmp;
          if ((*tree)->left != NULL){
            tmp = (*tree)->left;
          }
          else{
            tmp = (*tree)->right;
          }
          free(*tree);
          (*tree) = tmp;
        }
      }
    }
  }
}


void bst_dispose(bst_node_t **tree) {
  if ((*tree) != NULL){
    bst_dispose(&(*tree)->left);
    bst_dispose(&(*tree)->right);
    free((*tree));
  }
  (*tree) = NULL;
}


void bst_preorder(bst_node_t *tree) {
  if (tree != NULL){
    bst_print_node(tree);
    bst_preorder(tree->left);
    bst_preorder(tree->right);
  }
}


void bst_inorder(bst_node_t *tree) {
  if (tree != NULL){
    bst_inorder(tree->left);
    bst_print_node(tree);
    bst_inorder(tree->right);
  }
}


void bst_postorder(bst_node_t *tree) {
  if (tree != NULL){
    bst_postorder(tree->left);
    bst_postorder(tree->right);
    bst_print_node(tree);
  }
}
