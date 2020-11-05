#ifndef B_TREE
#define B_TREE
#include "b-tree-node.h"


typedef struct {
    struct node* root;
} btree;

void btree_init(btree* bt, size_t t);
int btree_search(btree* bt, int64_t key, int64_t* value);
void btree_insert(btree* bt, int64_t key, int64_t value);
int btree_erase(btree* bt, int64_t);
void btree_merge(btree* bt, btree* nextbt);
void btree_foreach(btree* bt, void (*)(struct kv_pair* value));
void btree_free(btree* bt);

#endif
