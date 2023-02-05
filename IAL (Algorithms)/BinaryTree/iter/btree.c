/**
 * @file btree.c
 * @author Michal Ľaš
 * @brief Binary search tree iterative variant
 * 
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>


void bst_init(bst_node_t **tree) {
  if (tree != NULL){
    (*tree) = NULL;
  }
}


bool bst_search(bst_node_t *tree, char key, int *value) {
  while (tree != NULL){
    if (tree->key == key){
      *value = tree->value;
      return true;
    }
    else{
      if (key < tree->key){
        tree = tree->left;
      }
      else{
        tree = tree->right;
      }
    }
  }
  return false;
}


void bst_insert(bst_node_t **tree, char key, int value) {

  bst_node_t *parent = NULL;
  bst_node_t *child = (*tree);

  /* Finding node with same key */
  while (child != NULL){     // In tree is not node with given key

    parent = child;

    if (child->key == key){
      child->value = value;
      return;
    }
    else{
      if (key < child->key){
        child = child->left;
      }
      else{
        child = child->right;
      }
    }
  }
  /* Node with given key was not found */
  /* Create new node */
  bst_node_t *new = (bst_node_t*) malloc(sizeof(struct bst_node));
  if (new == NULL){
    return;
  }
  new->key = key;
  new->value = value;
  new->left = NULL;
  new->right = NULL;

  if (parent == NULL){ // adding to empty tree
    (*tree) = new;
  }
  else{
    if (key < parent->key){
      parent->left = new;
    }
    else{
      parent->right = new;
    }
  }
}


void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {

  if ((*tree) != NULL){
    while ((*tree)->right != NULL){
      tree = &(*tree)->right;
    }
    
    target->key = (*tree)->key;
    target->value = (*tree)->value;

    bst_node_t *tmp = (*tree)->left;
    free((*tree));
    *tree = tmp;
  }
}


void bst_delete(bst_node_t **tree, char key) {


  if (tree != NULL){

    bool found = false;

    /* Finding node with given key */
    while (!found && (*tree) != NULL){

      if (key == (*tree)->key){
        found = true;
      }
      else{
        if (key < (*tree)->key){
          tree = &(*tree)->left;
        }
        else{
          tree = &(*tree)->right;
        }
      }
    }

    /* Node with given key does not exist */
    if ((*tree) == NULL){
      return;
    }

    /* There is node with given key */
    if (key == (*tree)->key){
      if ((*tree)->left == NULL && (*tree)->right == NULL){ // Node is leaf
        free((*tree));
        *tree = NULL;
        return;
      }
      else{
        if ((*tree)->left != NULL && (*tree)->right != NULL){ // Node has right and left child
          bst_replace_by_rightmost((*tree), &((*tree)->left));
        }
        else{ // Node has just one child
          bst_node_t *tmp = (*tree);
          if (tmp->left != NULL){
            tmp = (*tree)->left;
          }
          else{
            tmp = tmp->right;
          }
          free((*tree));
          *tree = tmp;
        }
      }
    }
  }
}


void bst_dispose(bst_node_t **tree) {

  if ((*tree != NULL)){

    stack_bst_t stack;
    stack_bst_init(&stack);
    bst_node_t *tmp;

    while ((*tree) != NULL || !stack_bst_empty(&stack)){
      if ((*tree == NULL)){
        (*tree) = stack_bst_pop(&stack);
      }
      if ((*tree)->right != NULL){
        stack_bst_push(&stack, (*tree)->right);
      }
      tmp = (*tree);
      (*tree) = (*tree)->left;
      free(tmp);
    }
  }
}


void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {

  while (tree != NULL){

    bst_print_node(tree);
    stack_bst_push(to_visit, tree);
    tree = tree->left;
  }
}


void bst_preorder(bst_node_t *tree) {

  if (tree != NULL){
    stack_bst_t stack;
    stack_bst_init(&stack);
    bst_leftmost_preorder(tree, &stack);

    bst_node_t *tmp;
    while (!stack_bst_empty(&stack)){
       tmp = stack_bst_pop(&stack);
       if (tmp->right != NULL){
        bst_leftmost_preorder(tmp->right, &stack);
       }
    }
  }
}


void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {

  while (tree != NULL){

   stack_bst_push(to_visit, tree);
   tree = tree->left; 
  }
}


void bst_inorder(bst_node_t *tree) {

  if (tree != NULL){

    stack_bst_t stack;
    stack_bst_init(&stack);
    bst_leftmost_inorder(tree, &stack);

    bst_node_t *tmp;
    while (!stack_bst_empty(&stack)){
      tmp = stack_bst_pop(&stack);
      if (tmp->right != NULL){
        bst_leftmost_inorder(tmp->right, &stack);
      }
      bst_print_node(tmp);
    }
  }
}


void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {

  while (tree != NULL){

    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, true);
    tree = tree->left; 
  }
}


void bst_postorder(bst_node_t *tree) {

  if (tree != NULL){

    stack_bst_t stack;
    stack_bst_init(&stack);
    stack_bool_t BoolStack;
    stack_bool_init(&BoolStack);
    bst_leftmost_postorder(tree, &stack, &BoolStack);

    bst_node_t *tmp;
    bool visited;
    while (!stack_bst_empty(&stack)){
      tmp = stack_bst_pop(&stack);
      stack_bst_push(&stack, tmp);
      visited = stack_bool_pop(&BoolStack);

      if (visited){
        stack_bool_push(&BoolStack, false);
        bst_leftmost_postorder(tmp->right, &stack, &BoolStack);
      }
      else {
        stack_bst_pop(&stack);
        bst_print_node(tmp);
      }
    }
  }
}
