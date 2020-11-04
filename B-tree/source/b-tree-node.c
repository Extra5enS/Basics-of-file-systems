
#include "../include/b-tree-node.h"

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
    n -> pairs = calloc(2 * t, sizeof(int64_t));
    n -> nodes = calloc(2 * t, sizeof(struct node*));
}

struct ans node_search(struct node* n, int64_t key) {
    size_t i = 0;
    
    for(;i < n -> size && key > n -> pairs[i].key; ++i);
    if(i < n -> size && key == n -> pairs[i].key) {
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
    struct node* newn = malloc(sizeof(struct node));
    node_init(newn, n -> t);

    struct node* child = n -> nodes[i];
    newn -> leaf = n -> leaf;
    newn -> size = n -> t - 1;
    for(size_t j = 0; j < n -> t - 1; ++j) {
        newn -> pairs[j] = child -> pairs[j];
    }


    if(!child -> leaf) {
        for(size_t j = 0; j < n -> t; ++j) {
            newn -> nodes[j] = child -> nodes[j + n -> t];
        }
    }
    child -> size = n -> t - 1;
    for(size_t j = n -> size; j > i - 1; --j) {
        n -> nodes[j + 1] = n -> nodes[j];
    }
    n -> nodes[i] = newn;

    for(size_t j = n -> size - 1; j > i; --j) {
        n -> nodes[j + 1] = n -> nodes[j];
    }

    n -> pairs[i] = child -> pairs[n -> t];
    n -> size++;
}

// TODO test this!!!!
void node_insert_nonfull(struct node* n, int64_t key, int64_t value) {
    size_t i = n -> size;
    size_t k = i + 1;
    if(n -> leaf) {
        for(;k >= 1 && key < n -> pairs[k - 1].key; --k) {
            n -> pairs[k] = n -> pairs[k - 1];
        }
        struct kv_pair kvp;
        kv_pair_init(&kvp, key, value); 
        n -> pairs[k - 1] = kvp;
        n -> size = n -> size + 1;
    } else {
        for(;k >= 1 && key < n -> pairs[k - 1].key; --k);
        if(n -> nodes[k - 1] -> size == 2 * n -> t - 1) {
            node_split_child(n, i);
            if(key > n -> pairs[k - 1].key) {
                k += 1;
            }
        }    
        node_insert_nonfull(n -> nodes[k - 1], key, value);
    }
}

void node_foreach(struct node* n, void (*ptr)(struct kv_pair* value)) {
    for(size_t i = 0; i < n -> size; ++i) {
        node_foreach(n -> nodes[i], ptr);
        ptr(&n->pairs[i]);
    } 
    node_foreach(n -> nodes[n -> size], ptr);
}

void node_free(struct node* n) {
    for(size_t i = 0; i < n -> size; ++i) {
        node_free(n -> nodes[i]);
        free(n -> nodes[i]);
    } 
    node_free(n -> nodes[n -> size]);
    free(n -> nodes[n -> size]);
}
