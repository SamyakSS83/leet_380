#include <omp.h>
#include <cstdlib>

struct Node {
    int key, value;
    Node* next;
};

struct Bucket {
    Node* head;
    omp_lock_t lock;
};

struct HashMap {
    Bucket* buckets;
    int num_buckets;
};

HashMap* hashmap_create(int num_buckets) {
    // TODO: allocate HashMap, initialize buckets and per-bucket locks
    return nullptr;
}

void hashmap_insert(HashMap* hm, int key, int value) {
    // TODO: lock the appropriate bucket, insert or update key
}

int hashmap_get(HashMap* hm, int key) {
    // TODO: return value for key, or -1 if not found
    return -1;
}

void hashmap_destroy(HashMap* hm) {
    // TODO: free all memory and destroy locks
}
