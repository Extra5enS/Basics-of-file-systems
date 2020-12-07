#include "../include/b-tree-node.h"
#include <stdio.h>
#include <limits.h>

#define max(lv, rv) ((lv) < (rv))?(rv):(lv)

void* xmalloc(size_t count, size_t size) {
    void* pointer = calloc(count, size);
    if(pointer < 0) {
        exit(-1);
    }
    return pointer;
}

size_t bin_search(struct kv_pair* array, size_t size, int64_t key) {
    size_t low = 0, high = size;
    while (low < high) {
        int mid = (low + high) / 2;
        if (key <= array[mid].key) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    return low;
}

int64_t my_pow(int64_t base, int64_t f) {
    int64_t res = 1;
    for(int64_t i = 0; i < f; ++i) {
        res *= base;
    }
    return res;
}

void kv_pair_init(struct kv_pair* kvp, int64_t key, int64_t value) {
    kvp -> key = key;
    kvp -> value = value;
    kvp -> is_delete = 0;
}

int kv_pair_cmp(void* lv, void* rv) {
    struct kv_pair* lkv_pair = lv;
    struct kv_pair* rkv_pair = rv;
    if(lkv_pair -> key == rkv_pair -> key) {
        return lkv_pair -> value - rkv_pair -> value;
    }
    return lkv_pair -> key - rkv_pair -> key;
}

void node_init(struct node* n, size_t t) {
    n -> pairs = xmalloc(2 * t + 2, sizeof(struct kv_pair));
    n -> nodes = xmalloc(2 * t + 2, sizeof(struct node*));
    
    n -> t = t;
    n -> size = 0;
    n -> leaf = 1;
}

struct ans node_search(struct node* n, int64_t key) {
    //printf("call %s with %p\n", "search", n);
    size_t i = 0;
    
    //for(;i < n -> size && key > n -> pairs[i].key; ++i);
    i = bin_search(n -> pairs, n -> size, key);
    if(i < n -> size && key == n -> pairs[i].key) {
        if(n -> leaf) {
            struct ans a = { n, i };
            return a;
        } else {
            return node_search(n -> nodes[i], key);
        }
    } else if (n -> leaf) {
        struct ans a  = { NULL, 0 };
        return a;
    } else {
        return node_search(n -> nodes[i], key);
    }
}

void node_split_child(struct node* n, size_t i) {
    //printf("call %s with %p\n", "split_child", n);
    struct node* newn = xmalloc(1, sizeof(struct node));
    node_init(newn, n -> t);
    struct node* child = n -> nodes[i];
    newn -> leaf = child -> leaf;
    newn -> size = n -> t - 1;

    for(size_t j = 0; j < n -> t - 1; ++j) {
        newn -> pairs[j] = child -> pairs[j + n -> t];
    }

    if(!child -> leaf) {
        for(size_t j = 0; j < n -> t; ++j) {
            newn -> nodes[j] = child -> nodes[j + n -> t];
        }
    }
    // this chosen is based on redument keys in no leaf nodes
    child -> size = (child -> leaf)? n -> t : n -> t - 1;
    for(int64_t j = (int64_t)(n -> size); j >= (int64_t)(i) + 1; --j) {
        n -> nodes[j + 1] = n -> nodes[j];
    }
    n -> nodes[i + 1] = newn;
    
    for(int64_t j = (int64_t)(n -> size) - 1; j >= (int64_t)(i); --j) {
        n -> pairs[j + 1] = n -> pairs[j];
    }

    n -> pairs[i] = child -> pairs[n -> t - 1];
    n -> size++;
}

void node_insert_nonfull(struct node* n, int64_t key, int64_t value) {
    //printf("call %s with %p\n", "insert_nonfull", n);
    int64_t i = n -> size - 1;
    if(n -> leaf) {
        for(;i >= 0 && key < n -> pairs[i].key; --i) {
            n -> pairs[i + 1] = n -> pairs[i];
        }
        struct kv_pair kvp;
        kv_pair_init(&kvp, key, value); 
        n -> pairs[i + 1] = kvp;
        n -> size++;
    } else {
        for(;i >= 0 && key < n -> pairs[i].key; --i);
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
    for(size_t i = 0; i < n -> size; ++i) {
        node_foreach(n -> nodes[i], ptr);
        if(!n -> pairs[i].is_delete && n -> leaf) {
            ptr(&n->pairs[i]);
        }
    } 
    node_foreach(n -> nodes[n -> size], ptr);
}

void node_free(struct node* n) {
    if(n == NULL) {
        return;
    }
    for(size_t i = 0; i < n -> size; ++i) {
        node_free(n -> nodes[i]);
        free(n -> nodes[i]);
    }
    node_free(n -> nodes[n -> size]);
    free(n -> nodes[n -> size]);
    free(n -> nodes);
    free(n -> pairs);
}

void node_depth(struct node* n, size_t next_depth, size_t* max_lvl) {
    if(n == NULL) {
        if(*max_lvl < next_depth) {
            *max_lvl = next_depth;
        }
        return;
    }
    for(size_t i = 0; i <= n -> size; ++i) {
        node_depth(n -> nodes[i], next_depth + 1, max_lvl);
    } 
}

void btree2table(struct node* n, struct kv_pair* line, size_t* tsize) {
    if(n == NULL) {
        return;
    }
    for(size_t i = 0; i < n -> size; ++i) {
        btree2table(n -> nodes[i], line, tsize);
        if(n -> leaf) {
            line[(*tsize)++] = n -> pairs[i];
        }
    }
    btree2table(n -> nodes[n -> size], line, tsize);
}

void print_table(struct node** table, size_t size) {
    printf("%lu\n", size);
    for(int i = 0; i < size; ++i) {
        for(size_t j = 0; j < table[i] -> size; ++j) {
            printf("%ld ", table[i] -> pairs[j].key);
        }
        printf("| ");
    }
    putchar('\n');
}

int64_t node_find_max(struct node* n) {
    if(n -> leaf) {
        return n -> pairs[n -> size - 1].key;
    } else {
        return node_find_max(n -> nodes[n -> size]);
    }
}

struct node* node_merge(struct node* ltree, struct node* rtree) {
    size_t lsize = 0, rsize = 0;
    node_depth(ltree, 0, &lsize);
    node_depth(rtree, 0, &rsize);
    int max_lvl = max(lsize, rsize);
    
    struct kv_pair* rline = xmalloc(my_pow(ltree -> t * 2 + 1, max_lvl + 1), sizeof(struct kv_pair));
    struct kv_pair* lline = xmalloc(my_pow(ltree -> t * 2 + 1, max_lvl + 1), sizeof(struct kv_pair));

    lsize = 0;
    rsize = 0;

    btree2table(ltree, lline, &lsize);
    btree2table(rtree, rline, &rsize);
    
    struct node** low_line = xmalloc((lsize + rsize) / ltree -> t + 1, sizeof(struct node*));
    size_t line_size = (lsize + rsize) / ltree -> t + 1;
    for(int i = 0; i < line_size; ++i) {
        low_line[i] = xmalloc(1, sizeof(struct node));
        node_init(low_line[i], ltree -> t);  
    }
    
    // merge
    size_t liter = 0, riter = 0;
    size_t glob_iter = 0, loc_iter = 0;

    while(liter != lsize || riter != rsize) {
        if((lline[liter].key <= rline[riter].key && 
                    liter != lsize)|| riter == rsize) {
            if(!lline[liter].is_delete) {
                low_line[glob_iter] -> pairs[loc_iter++] = lline[liter++];
                low_line[glob_iter] -> size++;
            } else {
                liter++;
            }
            if(lline[liter - 1].key == rline[riter].key) {
                riter++;
            }
        } else {
            if(!rline[riter].is_delete) {
                low_line[glob_iter] -> pairs[loc_iter++] = rline[riter++];
                low_line[glob_iter] -> size++;
            } else {
                riter++;
            }
        }
        if(loc_iter == ltree -> t) {
            glob_iter++;
            loc_iter = 0;
        }
    }

    for(size_t i = glob_iter; i < line_size; ++i) {
        node_free(low_line[i]);
        free(low_line[i]);
    }

    size_t tsize = glob_iter;
    size_t next_tsize = 0;
    struct node* node_copy = xmalloc(1, sizeof(struct node));
    node_init(node_copy, ltree -> t);
    node_copy -> leaf = 0;
    while(tsize != 1) {
        for(size_t i = 0; i < tsize; ++i) {
            if(i % (2 * ltree -> t - 1) != 2*ltree -> t - 2 && i != tsize - 1) {
                node_copy -> pairs[node_copy -> size].key = node_find_max(low_line[i]);
                node_copy -> nodes[node_copy -> size++] = low_line[i];
            } else {
                node_copy -> nodes[node_copy -> size] = low_line[i];
                low_line[next_tsize++] = node_copy;             
                node_copy = xmalloc(1, sizeof(struct node));
                node_init(node_copy, ltree -> t);
                node_copy -> leaf = 0;
            }
        }
        tsize = next_tsize;
        next_tsize = 0;    
    }
    
    struct node* root = low_line[0];
    
    node_free(node_copy);
    free(node_copy);
    free(rline);
    free(lline);
    free(low_line);
    
    return root;
}
