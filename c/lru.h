#ifndef LRU_H
#define LRU_H

#include <stddef.h>

/* LRU cache: string keys -> int values, capacity N > 0.
 * get() refreshes recency; put() evicts the least-recently-used entry when
 * full; putting an existing key updates its value and refreshes recency.
 * Lookup is a linear scan of a doubly-linked list (O(n)); a hash map would
 * make it O(1) — see README. */

typedef struct LruNode {
    char *key;
    int value;
    struct LruNode *prev;
    struct LruNode *next;
} LruNode;

typedef struct {
    LruNode *head; /* least-recently-used */
    LruNode *tail; /* most-recently-used */
    size_t cap;
    size_t len;
} LRUCache;

/* Returns NULL when cap == 0 or allocation fails. Caller owns the result. */
LRUCache *lru_create(size_t cap);
void lru_free(LRUCache *c);
/* Returns 1 and sets *value when found (and refreshes recency); returns 0
 * and leaves *value untouched when absent. */
int lru_get(LRUCache *c, const char *key, int *value);
void lru_put(LRUCache *c, const char *key, int value);
size_t lru_len(const LRUCache *c);
void lru_clear(LRUCache *c);
/* Copies keys least-recently-used -> most-recently-used into out
 * (caller provides len slots of const char*). Keys stay owned by the cache. */
void lru_keys(const LRUCache *c, const char **out);

#endif
