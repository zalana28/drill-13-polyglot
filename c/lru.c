#define _POSIX_C_SOURCE 200809L /* strdup */

#include "lru.h"

#include <stdlib.h>
#include <string.h>

static LruNode *find_node(LRUCache *c, const char *key) {
    for (LruNode *n = c->head; n; n = n->next) {
        if (strcmp(n->key, key) == 0) return n;
    }
    return NULL;
}

static void move_to_tail(LRUCache *c, LruNode *n) {
    if (n == c->tail) return;
    if (n->prev) n->prev->next = n->next;
    else c->head = n->next;
    if (n->next) n->next->prev = n->prev;
    n->prev = c->tail;
    n->next = NULL;
    if (c->tail) c->tail->next = n;
    c->tail = n;
    if (!c->head) c->head = n;
}

static void unlink_node(LRUCache *c, LruNode *n) {
    if (n->prev) n->prev->next = n->next;
    else c->head = n->next;
    if (n->next) n->next->prev = n->prev;
    else c->tail = n->prev;
    free(n->key);
    free(n);
    c->len--;
}

LRUCache *lru_create(size_t cap) {
    if (cap == 0) return NULL;
    LRUCache *c = calloc(1, sizeof(*c));
    if (!c) return NULL;
    c->cap = cap;
    return c;
}

void lru_free(LRUCache *c) {
    if (!c) return;
    lru_clear(c);
    free(c);
}

int lru_get(LRUCache *c, const char *key, int *value) {
    LruNode *n = find_node(c, key);
    if (!n) return 0;
    move_to_tail(c, n);
    *value = n->value;
    return 1;
}

void lru_put(LRUCache *c, const char *key, int value) {
    LruNode *n = find_node(c, key);
    if (n) {
        n->value = value;
        move_to_tail(c, n);
        return;
    }
    if (c->len == c->cap) unlink_node(c, c->head); /* evict LRU */
    n = malloc(sizeof(*n));
    if (!n) return; /* out of memory: fail silently, cache unchanged */
    n->key = strdup(key);
    if (!n->key) {
        free(n);
        return;
    }
    n->value = value;
    n->prev = c->tail;
    n->next = NULL;
    if (c->tail) c->tail->next = n;
    c->tail = n;
    if (!c->head) c->head = n;
    c->len++;
}

size_t lru_len(const LRUCache *c) {
    return c->len;
}

void lru_clear(LRUCache *c) {
    LruNode *n = c->head;
    while (n) {
        LruNode *next = n->next;
        free(n->key);
        free(n);
        n = next;
    }
    c->head = c->tail = NULL;
    c->len = 0;
}

void lru_keys(const LRUCache *c, const char **out) {
    size_t i = 0;
    for (LruNode *n = c->head; n; n = n->next) out[i++] = n->key;
}
