#include "../include/b-tree-node.h"
#include <stdio.h>
#include <limits.h>

#define max(lv, rv) ((lv) < (rv))?(rv):(lv)

int64_t pow(int64_t base, int64_t f) {
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
    struct kv_pair* lkv_pair = (struct kv_pair*) lv;
    struct kv_pair* rkv_pair = (struct kv_pair*) rv;
    if(lkv_pair -> key == rkv_pair -> key) {
        return lkv_pair -> value - rkv_pair -> value;
    }
    return lkv_pair -> key - rkv_pair -> key;
}

void node_init(struct node* n, size_t t) {
    n -> pairs = (struct kv_pair*)calloc(2 * t + 2, sizeof(struct kv_pair));
    n -> nodes = (struct node**)calloc(2 * t + 2, sizeof(struct node*));
    
    n -> t = t;
    n -> size = 0;
    n -> leaf = 1;
}

struct ans node_search(struct node* n, int64_t key) {
    //printf("call %s with %p\n", "search", n);
    size_t i = 0;
    
    for(;i < n -> size && key > n -> pairs[i].key; ++i);
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
    struct node* newn = (struct node*)calloc(1, sizeof(struct node));
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
    for(int64_t j = (int64_t)(n -> size); j >= int64_t(i) + 1; --j) {
        n -> nodes[j + 1] = n -> nodes[j];
    }
    n -> nodes[i + 1] = newn;
    
    for(int64_t j = (int64_t)(n -> size) - 1; j >= int64_t(i); --j) {
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
        if(n -> leaf && !line[*tsize].is_delete) {
            line[(*tsize)++] = n -> pairs[i];
        }
    }
    btree2table(n -> nodes[n -> size], line, tsize);
}

void print_table(kv_pair** table, size_t* tsize, int max_lvl) {
    for(int i = 0; i < max_lvl + 1; ++i) {
        for(size_t j = 0; j < tsize[i]; ++j) {
            printf("%ld ", table[i][j].key);
        }
        putchar('\n');
    }
}

void table2btree(struct node* root, struct kv_pair** table, size_t* tsize, 
        int lvl, int max_lvl, int64_t low_r, int64_t up_r) {
    
    size_t i = 0;
    struct kv_pair* line = table[lvl];
    size_t size = tsize[lvl];
    size_t iter = 0;
    for(;i < size && line[i].key <= low_r;++i);
    for(;i < size && line[i].key <= up_r;++i) {
        root -> pairs[iter++] = line[i];
    }
    root -> size = iter;
    if(lvl == max_lvl) {
        root -> leaf = 1;
        return;
    } 
    root -> leaf = 0;
    int64_t new_low_r = low_r;
    int64_t new_up_r = root -> pairs[0].key;
    for(size_t i = 0; i <= root -> size; ++i) {
        root -> nodes[i] = (struct node*)calloc(1, sizeof(struct node));
        node_init(root -> nodes[i], root -> t);
        table2btree(root -> nodes[i], table, tsize, 
                lvl + 1, max_lvl, new_low_r, new_up_r);
        
        new_low_r = new_up_r;
        new_up_r = (i == root -> size - 1) ? up_r : root -> pairs[i + 1].key;
    }
}

struct node* node_merge(struct node* ltree, struct node* rtree) {
    size_t lsize = 0, rsize = 0;
    node_depth(ltree, 0, &lsize);
    node_depth(rtree, 0, &rsize);
    int max_lvl = max(lsize, rsize);
    
    struct kv_pair** value_lvl_table = 
        (struct kv_pair**)calloc(max_lvl + 1, sizeof(struct kv_pair*));
    
    for(int i = 0; i < max_lvl + 1; ++i) { 
        value_lvl_table[i] = 
            (struct kv_pair*)calloc(pow(ltree -> t * 2, i + 1), sizeof(struct kv_pair));
    }
    
    struct kv_pair* lline = (struct kv_pair*)calloc(pow(ltree -> t * 2, max_lvl + 1), sizeof(struct kv_pair));
    struct kv_pair* rline = (struct kv_pair*)calloc(pow(ltree -> t * 2, max_lvl + 1), sizeof(struct kv_pair));

    size_t* tsize = (size_t*)calloc(max_lvl + 1, sizeof(size_t));
    lsize = 0;
    rsize = 0;

    btree2table(ltree, lline, &lsize);
    btree2table(rtree, rline, &rsize);
    
    // merge
    size_t liter = 0, riter = 0, resiter = 0;
    struct kv_pair* resline = value_lvl_table[max_lvl];
    while(liter != lsize || riter != rsize) {
        if((lline[liter].key <= rline[riter].key && 
                    liter != lsize)|| riter == rsize) {
            resline[resiter++] = lline[liter++];
            if(lline[liter - 1].key == rline[riter].key) {
                riter++;
            }
        } else {
            resline[resiter++] = rline[riter++];
        }
    }
    tsize[max_lvl] = resiter;

    // creat info for all noleaf nodes
    for(int titer = max_lvl; titer > 0; --titer) {
        size_t counter_for_t = 0;
        size_t up_iter = 0;
        kv_pair* lower_line = value_lvl_table[titer];
        kv_pair* prevl_line = value_lvl_table[titer - 1];
        for(size_t i = 0; i < tsize[titer]; ++i) {
            if(counter_for_t == ltree -> t * 2 - 1) { 
                for(size_t j = tsize[titer - 1]; j > up_iter; --j) {
                    prevl_line[j] = prevl_line[j - 1];
                }
                prevl_line[up_iter++] = lower_line[i - ltree -> t + 1];
                tsize[titer - 1]++;
                if(titer != max_lvl) {
                    for(size_t j = i - ltree -> t + 1; j < tsize[titer] - 1; ++j) {
                        lower_line[j] = lower_line[j + 1];
                    }
                    tsize[titer]--;
                }
                counter_for_t = ltree -> t;
            }
            counter_for_t++;
        }
    }

    
    // creat new btree
    struct node* root = (struct node*) calloc(1, sizeof(struct node));
    node_init(root, ltree -> t);
    table2btree(root, value_lvl_table, tsize,
            0, max_lvl, LLONG_MIN, LLONG_MAX); 

    for(int i = 0; i < max_lvl + 1; ++i) {
        free(value_lvl_table[i]);
    }
    free(value_lvl_table);
    free(tsize);
    free(lline);
    free(rline);

    return root;
}
