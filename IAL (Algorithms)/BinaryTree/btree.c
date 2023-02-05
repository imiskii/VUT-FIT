#include "btree.h"
#include <stdio.h>

/*
 * Function for printing node of tree
 */
void bst_print_node(bst_node_t *node) {
  printf("[%c,%d]", node->key, node->value);
}
