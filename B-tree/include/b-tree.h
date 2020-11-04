#ifndef B_TREE
#define B_TREE
#include "b-tree-node.h"


typedef struct {
    struct node* root;
} btree;

void btree_init(btree* bt, size_t t);
int64_t btree_search(btree* bt, int64_t key);
void btree_insert(btree* bt, int64_t key, int64_t value);
void btree_erase(btree* bt, int64_t);
void btree_merge(btree* bt, btree* nextbt);
void btree_foreach(btree* bt, void (*)(struct kv_pair value));
void btree_free(btree* bt);

#endif
