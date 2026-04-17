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
    HashMap* hm = new HashMap();
    hm->num_buckets = num_buckets;
    hm->buckets = new Bucket[num_buckets];
    for (int i = 0; i < num_buckets; i++) {
        hm->buckets[i].head = nullptr;
        omp_init_lock(&hm->buckets[i].lock);
    }
    return hm;
}

void hashmap_insert(HashMap* hm, int key, int value) {
    int b = key % hm->num_buckets;
    omp_set_lock(&hm->buckets[b].lock);
    Node* cur = hm->buckets[b].head;
    while (cur) {
        if (cur->key == key) { cur->value = value; omp_unset_lock(&hm->buckets[b].lock); return; }
        cur = cur->next;
    }
    Node* n = new Node{key, value, hm->buckets[b].head};
    hm->buckets[b].head = n;
    omp_unset_lock(&hm->buckets[b].lock);
}

int hashmap_get(HashMap* hm, int key) {
    int b = key % hm->num_buckets;
    omp_set_lock(&hm->buckets[b].lock);
    Node* cur = hm->buckets[b].head;
    while (cur) {
        if (cur->key == key) { int v = cur->value; omp_unset_lock(&hm->buckets[b].lock); return v; }
        cur = cur->next;
    }
    omp_unset_lock(&hm->buckets[b].lock);
    return -1;
}

void hashmap_destroy(HashMap* hm) {
    for (int i = 0; i < hm->num_buckets; i++) {
        Node* cur = hm->buckets[i].head;
        while (cur) { Node* next = cur->next; delete cur; cur = next; }
        omp_destroy_lock(&hm->buckets[i].lock);
    }
    delete[] hm->buckets;
    delete hm;
}
