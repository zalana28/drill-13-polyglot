#include <stdio.h>
#include <string.h>

#include "ringbuffer.h"
#include "lru.h"

static int failures = 0;
static int checks = 0;

#define CHECK(cond) do { \
    checks++; \
    if (!(cond)) { failures++; printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

static int arr_eq(const int *a, const int *b, size_t n) {
    for (size_t i = 0; i < n; i++) if (a[i] != b[i]) return 0;
    return 1;
}

static int keys_eq(const char **a, const char **b, size_t n) {
    for (size_t i = 0; i < n; i++) if (strcmp(a[i], b[i]) != 0) return 0;
    return 1;
}

/* 1. RingBuffer N=1: second push overwrites the only element. */
static void test_rb_n1(void) {
    RingBuffer *rb = rb_create(1);
    rb_push(rb, 10);
    rb_push(rb, 20);
    CHECK(rb_len(rb) == 1);
    CHECK(rb_get(rb, 0) == 20);
    rb_free(rb);
}

/* 2. RingBuffer overwrite when full (capacity 3, push 1..4 -> [2,3,4]). */
static void test_rb_overwrite_full(void) {
    RingBuffer *rb = rb_create(3);
    rb_push(rb, 1); rb_push(rb, 2); rb_push(rb, 3); rb_push(rb, 4);
    int out[3]; rb_to_array(rb, out);
    int want[3] = {2, 3, 4};
    CHECK(rb_len(rb) == 3);
    CHECK(arr_eq(out, want, 3));
    rb_free(rb);
}

/* 3. RingBuffer iteration order oldest->newest after wrap (push 1..5 -> [3,4,5]). */
static void test_rb_order_after_wrap(void) {
    RingBuffer *rb = rb_create(3);
    for (int i = 1; i <= 5; i++) rb_push(rb, i);
    int out[3]; rb_to_array(rb, out);
    int want[3] = {3, 4, 5};
    CHECK(arr_eq(out, want, 3));
    CHECK(rb_get(rb, 0) == 3);
    CHECK(rb_get(rb, 2) == 5);
    rb_free(rb);
}

/* 4. RingBuffer clear + len; push works again after clear. */
static void test_rb_clear(void) {
    RingBuffer *rb = rb_create(3);
    rb_push(rb, 1); rb_push(rb, 2);
    CHECK(rb_len(rb) == 2);
    rb_clear(rb);
    CHECK(rb_len(rb) == 0);
    rb_push(rb, 9);
    CHECK(rb_len(rb) == 1);
    CHECK(rb_get(rb, 0) == 9);
    rb_free(rb);
}

/* 5. RingBuffer invalid capacity N=0 -> NULL. */
static void test_rb_invalid_cap(void) {
    CHECK(rb_create(0) == NULL);
}

/* 6. LRU get refreshes recency: get(a) saves a from eviction. */
static void test_lru_get_refresh(void) {
    LRUCache *c = lru_create(2);
    int v;
    lru_put(c, "a", 1); lru_put(c, "b", 2);
    CHECK(lru_get(c, "a", &v) == 1 && v == 1);
    lru_put(c, "c", 3); /* must evict b, not a */
    CHECK(lru_get(c, "b", &v) == 0);
    CHECK(lru_get(c, "a", &v) == 1 && v == 1);
    CHECK(lru_get(c, "c", &v) == 1 && v == 3);
    CHECK(lru_len(c) == 2);
    lru_free(c);
}

/* 7. LRU eviction order without refresh: put a,b then c evicts a. */
static void test_lru_eviction_order(void) {
    LRUCache *c = lru_create(2);
    lru_put(c, "a", 1); lru_put(c, "b", 2); lru_put(c, "c", 3);
    const char *keys[2]; lru_keys(c, keys);
    const char *want[2] = {"b", "c"};
    CHECK(keys_eq(keys, want, 2));
    CHECK(lru_len(c) == 2);
    lru_free(c);
}

/* 8. LRU put existing key updates value + refreshes recency. */
static void test_lru_put_existing(void) {
    LRUCache *c = lru_create(2);
    int v;
    lru_put(c, "a", 1); lru_put(c, "b", 2);
    lru_put(c, "a", 10); /* update + refresh */
    CHECK(lru_len(c) == 2);
    lru_put(c, "c", 3); /* must evict b, not a */
    CHECK(lru_get(c, "b", &v) == 0);
    CHECK(lru_get(c, "a", &v) == 1 && v == 10);
    const char *keys[2]; lru_keys(c, keys);
    const char *want[2] = {"c", "a"};
    CHECK(keys_eq(keys, want, 2));
    lru_free(c);
}

/* 9. LRU get absent -> not found; len tracking; clear works. */
static void test_lru_absent_and_clear(void) {
    LRUCache *c = lru_create(2);
    int v = -1;
    CHECK(lru_get(c, "nope", &v) == 0);
    CHECK(v == -1); /* untouched on miss */
    lru_put(c, "a", 1);
    CHECK(lru_len(c) == 1);
    lru_clear(c);
    CHECK(lru_len(c) == 0);
    CHECK(lru_get(c, "a", &v) == 0);
    lru_put(c, "z", 26);
    CHECK(lru_get(c, "z", &v) == 1 && v == 26);
    lru_free(c);
}

/* 10. LRU N=1: put b evicts a. */
static void test_lru_n1(void) {
    LRUCache *c = lru_create(1);
    int v;
    lru_put(c, "a", 1);
    lru_put(c, "b", 2);
    CHECK(lru_len(c) == 1);
    CHECK(lru_get(c, "a", &v) == 0);
    CHECK(lru_get(c, "b", &v) == 1 && v == 2);
    lru_free(c);
}

int main(void) {
    test_rb_n1();
    test_rb_overwrite_full();
    test_rb_order_after_wrap();
    test_rb_clear();
    test_rb_invalid_cap();
    test_lru_get_refresh();
    test_lru_eviction_order();
    test_lru_put_existing();
    test_lru_absent_and_clear();
    test_lru_n1();
    printf("C: %d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
