#include"../include/b-tree.h"
#include <stdio.h>
#include <sys/time.h>
#include <limits.h>

#include <map>

#define TEST_ARRAY_SIZE (1l << 15)//(1l << 22)

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
    int64_t mmt1[5] = {1, 3, 5, 7, 9};/*,
                         11, 13, 15, 17, 19, 
                         21, 23, 25, 27, 29, 
                         31, 33, 35, 37, 39};*/
    int64_t mmt2[5] = {0, 2, 4, 6, 8};/*, 
                         10, 12, 14, 16, 18, 
                         20, 22, 24, 26, 28, 
                         30, 32, 34, 36, 38};*/
    btree mt1, mt2;
    btree_init(&mt1, 2);
    btree_init(&mt2, 2);
    for(int i = 0; i < 5; ++i) {
       btree_insert(&mt1, mmt1[i], i); 
       btree_insert(&mt2, mmt2[i], i); 
    }
    btree merged = btree_merge(&mt1, &mt2);
    printf("----\n"); 
    printf("\n1 three\n");
    btree_foreach(&mt1, print);
    printf("\n2 three\n");
    btree_foreach(&mt2, print);
    printf("\nmerged\n");
    btree_foreach(&merged, print);
    printf("----\n");  
    
    printf("time test\n");
    int64_t sec, usec;
    std::map<int64_t, int64_t> test_map;
    btree test_tree;
    btree_init(&test_tree, 3);

    int64_t* big_array = (int64_t*)calloc(TEST_ARRAY_SIZE, sizeof(int64_t));
    srand(time(NULL));    
    for(int64_t i = 0; i < TEST_ARRAY_SIZE; ++i) {
        big_array[i] = rand();
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for(int64_t i = 0; i < TEST_ARRAY_SIZE; ++i) {
         test_map[big_array[i]] = i;
    }
    gettimeofday(&end, NULL);
    usec = end.tv_usec - start.tv_usec;
    sec = end.tv_sec - start.tv_sec;
    printf("%s:%lds, %ldus\n", "std::map insert", 
            (usec < 0) ? (sec - 1) : (sec), (usec < 0) ? (~usec) : (usec));

    gettimeofday(&start, NULL);
    for(int64_t i = 0; i < TEST_ARRAY_SIZE; ++i) {
        btree_insert(&test_tree, big_array[i], i);
    }
    gettimeofday(&end, NULL);
    usec = end.tv_usec - start.tv_usec;
    sec = end.tv_sec - start.tv_sec;
    printf("%s:%lds, %ldus\n", "btree insert", 
            (usec < 0) ? (sec - 1) : (sec), (usec < 0) ? (~usec) : (usec));

    free(big_array);
    btree_free(&test_tree);
    btree_free(&btr);
    btree_free(&mt1);
    btree_free(&mt2);
    //btree_free(&merged);
    return 0;
}
