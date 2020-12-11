#include <iostream>
#include <set>

#include "../include/b-tree.h"

std::set<int64_t> test_set;
btree test_btree;

int comper_stores() {
	kv_pair kv;
	btree_iterator biter;
	btree_iterator_init(&biter, &test_btree);

	int res = 0;

	for(auto& set_key :	test_set) {
		int loc_res = btree_iterator_next(&biter, &kv);
		if(loc_res != 0 || kv.key != set_key) {
			fprintf(stderr, "ERROR: tree_size < set_size\n");
			res = -1;
		}
		if(kv.key != set_key) {
			fprintf(stderr, "ERROR: set_key = %ld, tree_key = %ld\n", set_key, kv.key);
			res = -1;
		}
	}
	if(btree_iterator_next(&biter, &kv) != -1) {
		fprintf(stderr, "ERROR: tree_size > set_size\n");
		res = -1;
	}
	return res;
}

int main() {
	srand(time(NULL));
    btree_init(&test_btree, 4);
	/*
	int64_t test_array[5] = {1, 2, 3, 4, 5};
	for(int i = 0; i < 5; ++i) {
		btree_insert(&test_btree, test_array[i], i);
	}
	for(int i = 0; i < 4; ++i) {
		test_set.insert(test_array[i]);
	}
	btree_erase(&test_btree, 5);	
	
	if(comper_stores() == 0) {
		printf("OK\n");
	} else {
		printf("FAIL\n");
	}*/
	
	

	btree_free(&test_btree);
	return 0;
}
