#ifndef _HASHMAP_H
#define _HASHMAP_H

#include "utils/iterator.h"
#include "utils/allocator.h"

typedef struct hashmap_pair_t hashmap_pair;
typedef struct hashmap_bucket_t hashmap_bucket;
typedef struct hashmap_node_t hashmap_node;
typedef struct hashmap_t hashmap;

struct hashmap_pair_t {
    unsigned int keylen, vallen;
    void *key, *val;
};

struct hashmap_node_t {
    hashmap_pair pair; 
    hashmap_node *next;
};
 
struct hashmap_bucket_t {
    hashmap_node *first;
};

struct hashmap_t {
    hashmap_bucket *buckets;
    unsigned int buckets_x;
    unsigned int reserved;
    allocator alloc;
};

void hashmap_create(hashmap *hashmap, int n_size); // actual size will be 2^n_size
void hashmap_create_with_allocator(hashmap *hashmap, int n_size, allocator alloc);
void hashmap_free(hashmap *hashmap);
unsigned int hashmap_size(hashmap *hashmap);
unsigned int hashmap_reserved(hashmap *hashmap);
void hashmap_sput(hashmap *hashmap, const char *key, void *value, unsigned int value_len);
void hashmap_iput(hashmap *hashmap, unsigned int key, void *value, unsigned int value_len);
int hashmap_sget(hashmap *hashmap, const char *key, void **value, unsigned int *value_len);
int hashmap_iget(hashmap *hashmap, unsigned int key, void **value, unsigned int *value_len);
void hashmap_sdel(hashmap *hashmap, const char *key);
void hashmap_idel(hashmap *hashmap, unsigned int key);
void hashmap_iter_begin(hashmap *hashmap, iterator *iter);
void hashmap_delete(hashmap *hashmap, iterator *iter);

#endif // _HASHMAP_H