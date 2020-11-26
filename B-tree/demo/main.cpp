#include"../include/b-tree.h"
#include <stdio.h>

void print(struct kv_pair* kv) {
    printf("%ld -> %ld \n", kv -> key, kv -> value);
}

int main() {
    btree btr;
    btree_init(&btr, 2);
    printf("insert key's from 1 to 20 with value = 10 * key\n"); 
    for(int64_t i = 0; i <= 20; ++i) {
        btree_insert(&btr, i, 10*i); 
    }
    printf("----\n");  
    btree_foreach(&btr, print);
    printf("----\n");  
    printf("change value's with key from 5 to 10 with value = key\n");
    for(int64_t i = 5; i <= 10; ++i) {
        btree_insert(&btr, i, i);
    }
    printf("----\n");  
    btree_foreach(&btr, print);
    printf("----\n");  
    printf("delete elements with keys from 0 to 10\n");
    for(int64_t i = 0; i <= 10; ++i) {
        btree_erase(&btr, i);
    }
    printf("----\n");  
    btree_foreach(&btr, print);
    printf("----\n");  
    printf("merge test\n");
    uint64_t mmt1[5] = {1, 3, 5, 7, 9};
    uint64_t mmt2[5] = {0, 2, 4, 6, 8};
    btree mt1, mt2;
    btree_init(&mt1, 2);
    btree_init(&mt2, 2);
    for(int i = 0; i < 5; ++i) {
       btree_insert(&mt1, mmt1[i], i); 
       btree_insert(&mt2, mmt2[i], i); 
    }
    node_merge(mt1.root, mt2.root);
    btree_free(&btr);
    return 0;
}
