#include"../include/b-tree.h"
#include <stdio.h>

void print(struct kv_pair* kv) {
    printf("%ld -> %ld \n", kv -> key, kv -> value);
}

int main() {
    btree btr;
    btree_init(&btr, 3);
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
    printf("delete elements with keys from 16 to 20\n");
    for(int64_t i = 16; i <= 20; ++i) {
        btree_erase(&btr, i);
    }
    printf("----\n");  
    btree_foreach(&btr, print);
    printf("----\n");  
    


    btree_free(&btr);
    return 0;
}
