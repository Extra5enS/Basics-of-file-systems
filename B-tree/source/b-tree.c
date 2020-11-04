#include"../include/b-tree.h"

void btree_init(btree* bt, size_t t) {
    node_init(bt -> root, t);
}

int btree_search(btree* bt, int64_t key, int64_t* value) {
    struct ans a = node_search(bt -> root, key);
    if(a.n == NULL) {
        return 0;
    } else {
        *value = a.n -> pairs[a.i].value;
        return 1;
    }
}

void btree_insert(btree* bt, int64_t key, int64_t value) {
    struct node* root = bt -> root;
    
    if(root -> size == root -> t * 2 - 1) {
        struct node* new = malloc(sizeof(struct node));
        node_init(new, root -> t);

        bt -> root = new;
        new -> leaf = 0;
        new -> size = 0;
        new -> nodes[0] = root;

        node_split_child(new, 1);
        node_insert_nonfull(new, key, value);
    } else {
        node_insert_nonfull(root, key, value);
    }
}

void btree_foreach(btree* bt, void (*ptr)(struct kv_pair* value)) {
    node_foreach(bt -> root, ptr);
}

void btree_free(btree* bt) {
    node_free(bt -> root);
}
