#include <iostream>
#include <set>

#include "../include/b-tree.h"
#define MAX_TASK_ARRAY_SIZE (1 << 10) // for first time

#define MAX_KEY_ARRAY_SIZE (1 << 12)
#define	IN_TASK(n) ((n) % 3)
#define IS_INSERT 0
#define IS_ERASE 1
#define IS_SEARCH 2

int comper_stores(std::set<int64_t> test_set, btree test_btree) {
	kv_pair kv;
	btree_iterator biter;
	btree_iterator_init(&biter, &test_btree);

	int res = 0;

	for(auto& set_key :	test_set) {
		int loc_res = btree_iterator_next(&biter, &kv);
		if(loc_res != 0) {
			fprintf(stderr, "ERROR: tree_size < set_size\n");
			res = -1;
		} else if(kv.key != set_key) {
			fprintf(stderr, "ERROR: set_key = %ld, tree_key = %ld\n", set_key, kv.key);
			res = -1;
		}
	}
	while(btree_iterator_next(&biter, &kv) != -1) {
//		fprintf(stderr, "ERROR: tree_size > set_size : kv = (%ld, %ld)\n", kv.key, kv.value);
		res = -1;
	}
	return res;
}



int main() {
	srand(time(NULL));
	uint8_t* task_array = (uint8_t*)calloc(MAX_TASK_ARRAY_SIZE, sizeof(uint8_t)); 
	int64_t* key_array = (int64_t*)calloc(MAX_KEY_ARRAY_SIZE, sizeof(int64_t));
	
	for(;;) {
		size_t task_array_step_size = rand() % (MAX_TASK_ARRAY_SIZE - 10) + 10;
		for(size_t i = 0; i < MAX_KEY_ARRAY_SIZE; ++i) {
			key_array[i] = rand();
		}
		for(size_t i = 0; i < task_array_step_size; ++i) {
			task_array[i] = IN_TASK(rand());
		}
		
		std::set<int64_t> test_set;
		btree test_btree;
		btree_init(&test_btree, 4);
		
		int64_t counter_of_bad_search = 0;
		for(size_t i = 0; i < task_array_step_size; ++i) {
			int64_t key = key_array[rand() % MAX_KEY_ARRAY_SIZE];
			switch(task_array[i]) {
				case IS_INSERT:
					test_set.insert(key);
					btree_insert(&test_btree, key, i);
					break;
				case IS_ERASE:
					{
						auto p = test_set.find(key);
						if(p != test_set.end()) {
							test_set.erase(p);
						}
						btree_erase(&test_btree, key);
					}
					break;
				case IS_SEARCH:
					{
						auto p = test_set.find(key);
						int64_t value = 0;
						int ans = btree_search(&test_btree, key, &value);
						if(!((p == test_set.end() && ans == -1) ||
									(p != test_set.end() && ans == 0))) {
							counter_of_bad_search++;
						}
					}
					break;
			}
		}
		if(comper_stores(test_set, test_btree) == 0) {
		//	printf("OK\n");
		} else {
		//	printf("FAIL\n");
			break;
		}	
		btree_free(&test_btree);
	}

	free(key_array);
	free(task_array);
	return 0;
}
