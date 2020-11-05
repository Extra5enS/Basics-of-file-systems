
#include "../include/b-tree-node.h"
#include <stdio.h>

void kv_pair_init(struct kv_pair* kvp, int64_t key, int64_t value) {
    kvp -> key = key;
    kvp -> value = value;
    kvp -> is_delete = 0;
}

int kv_pair_cmp(void* lv, void* rv) {
    struct kv_pair* lpair = (struct kv_pair*) lv;
    struct kv_pair* rpair = (struct kv_pair*) rv;
    if(lpair -> key == rpair -> key) {
        return lpair -> value - rpair -> value;
    }
    return lpair -> key - rpair -> key;
}

void node_init(struct node* n, size_t t) {
    n -> t = t;
    n -> size = 0;
    n -> leaf = 1;
    n -> pairs = (struct kv_pair*)calloc(2 * t + 2, sizeof(struct kv_pair));
    n -> nodes = (struct node**)calloc(2 * t + 2, sizeof(struct node*));
}

struct ans node_search(struct node* n, int64_t key) {
    size_t i = 1;
    
    for(;i <= n -> size && key > n -> pairs[i].key; ++i);
    if(i <= n -> size && key == n -> pairs[i].key) {
        struct ans a = { n, i };
        return a;
    } else if (n -> leaf) {
        struct ans a  = { NULL, 0 };
        return a;
    } else {
        return node_search(n -> nodes[i], key);
    }
}

void node_split_child(struct node* n, size_t i) {
    struct node* newn = (struct node*)malloc(sizeof(struct node));
    node_init(newn, n -> t);
    struct node* child = n -> nodes[i];
    newn -> leaf = child -> leaf;
    newn -> size = n -> t - 1;

    for(size_t j = 1; j <= n -> t - 1; ++j) {
        newn -> pairs[j] = child -> pairs[j + n -> t];
    }

    if(!child -> leaf) {
        for(size_t j = 1; j <= n -> t; ++j) {
            newn -> nodes[j] = child -> nodes[j + n -> t];
        }
    }
    child -> size = n -> t - 1;
    for(size_t j = n -> size + 1; j >= i + 1; --j) {
        n -> nodes[j + 1] = n -> nodes[j];
    }
    n -> nodes[i + 1] = newn;
    
    for(size_t j = n -> size; j >= i; --j) {
        n -> pairs[j + 1] = n -> pairs[j];
    }

    n -> pairs[i] = child -> pairs[n -> t];
    n -> size++;
}

void node_insert_nonfull(struct node* n, int64_t key, int64_t value) {
    int64_t i = n -> size;
    if(n -> leaf) {
        for(;i >= 1 && key < n -> pairs[i].key; --i) {
            n -> pairs[i + 1] = n -> pairs[i];
        }
        struct kv_pair kvp;
        kv_pair_init(&kvp, key, value); 
        n -> pairs[i + 1] = kvp;
        n -> size++;
    } else {
        for(;i >= 1 && key < n -> pairs[i].key; --i);
        ++i;
        if(n -> nodes[i] -> size == 2 * n -> t - 1) {
            node_split_child(n, i);
            if(key > n -> pairs[i].key) {
                ++i;
            }
        }    
        node_insert_nonfull(n -> nodes[i], key, value);
    }
}

void node_foreach(struct node* n, void (*ptr)(struct kv_pair* value)) {
    if(n == NULL) {
        return;
    }
    for(size_t i = 1; i <= n -> size; ++i) {
        node_foreach(n -> nodes[i], ptr);
        if(!n -> pairs[i].is_delete) {
            ptr(&n->pairs[i]);
        }
    } 
    if(n -> nodes[n -> size + 1]) {
        node_foreach(n -> nodes[n -> size + 1], ptr);
    }
}

void node_free(struct node* n) {
    if(n == NULL) {
        return;
    }
    for(size_t i = 1; i <= n -> size; ++i) {
        node_free(n -> nodes[i]);
        free(n -> nodes[i]);
    } 
    node_free(n -> nodes[n -> size + 1]);
    free(n -> nodes[n -> size + 1]);
    
    free(n -> nodes);
    free(n -> pairs);
}
