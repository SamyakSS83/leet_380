// tests.cpp for problem 014 — OMP Locks Fine-Grained Hash Map
#include "harness.h"

// Node, Bucket, HashMap defined in solve.cpp (included before this file)
HashMap* hashmap_create(int num_buckets);
void hashmap_insert(HashMap* hm, int key, int value);
int  hashmap_get(HashMap* hm, int key);
void hashmap_destroy(HashMap* hm);

void test_basic_insert_get() {
    auto hm = hashmap_create(16);
    hashmap_insert(hm, 5, 100);
    ASSERT_EQ(hashmap_get(hm, 5), 100);
    hashmap_destroy(hm);
}

void test_not_found() {
    auto hm = hashmap_create(16);
    ASSERT_EQ(hashmap_get(hm, 99), -1);
    hashmap_destroy(hm);
}

void test_update() {
    auto hm = hashmap_create(16);
    hashmap_insert(hm, 3, 10);
    hashmap_insert(hm, 3, 20);
    ASSERT_EQ(hashmap_get(hm, 3), 20);
    hashmap_destroy(hm);
}

void test_collision() {
    auto hm = hashmap_create(4);
    hashmap_insert(hm, 0, 1);
    hashmap_insert(hm, 4, 2);  // same bucket as 0
    hashmap_insert(hm, 8, 3);  // same bucket as 0
    ASSERT_EQ(hashmap_get(hm, 0), 1);
    ASSERT_EQ(hashmap_get(hm, 4), 2);
    ASSERT_EQ(hashmap_get(hm, 8), 3);
    hashmap_destroy(hm);
}

void test_concurrent_insert() {
    auto hm = hashmap_create(64);
    #pragma omp parallel for schedule(static) num_threads(4)
    for (int i = 0; i < 1000; i++) {
        hashmap_insert(hm, i, i * 2);
    }
    // verify all
    int errors = 0;
    for (int i = 0; i < 1000; i++) {
        if (hashmap_get(hm, i) != i * 2) errors++;
    }
    ASSERT_EQ(errors, 0);
    hashmap_destroy(hm);
}

void test_many_keys() {
    auto hm = hashmap_create(128);
    for (int i = 0; i < 500; i++) hashmap_insert(hm, i, i + 1000);
    int errors = 0;
    for (int i = 0; i < 500; i++)
        if (hashmap_get(hm, i) != i + 1000) errors++;
    ASSERT_EQ(errors, 0);
    hashmap_destroy(hm);
}

int main() {
    RUN_TEST("basic insert/get", test_basic_insert_get);
    RUN_TEST("not found returns -1", test_not_found);
    RUN_TEST("update existing key", test_update);
    RUN_TEST("collision handling", test_collision);
    RUN_TEST("concurrent insert 1000 keys", test_concurrent_insert);
    RUN_TEST("many keys sequential", test_many_keys);
    print_results();
    return 0;
}
