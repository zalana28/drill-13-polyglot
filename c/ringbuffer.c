#include "ringbuffer.h"

#include <stdlib.h>

RingBuffer *rb_create(size_t cap) {
    if (cap == 0) return NULL;
    RingBuffer *rb = malloc(sizeof(*rb));
    if (!rb) return NULL;
    rb->buf = malloc(cap * sizeof(int));
    if (!rb->buf) {
        free(rb);
        return NULL;
    }
    rb->cap = cap;
    rb->len = 0;
    rb->start = 0;
    return rb;
}

void rb_free(RingBuffer *rb) {
    if (!rb) return;
    free(rb->buf);
    free(rb);
}

void rb_push(RingBuffer *rb, int value) {
    if (rb->len < rb->cap) {
        rb->buf[(rb->start + rb->len) % rb->cap] = value;
        rb->len++;
    } else {
        /* Full: overwrite the oldest element, advance the start. */
        rb->buf[rb->start] = value;
        rb->start = (rb->start + 1) % rb->cap;
    }
}

size_t rb_len(const RingBuffer *rb) {
    return rb->len;
}

void rb_clear(RingBuffer *rb) {
    rb->len = 0;
    rb->start = 0;
}

int rb_get(const RingBuffer *rb, size_t i) {
    return rb->buf[(rb->start + i) % rb->cap];
}

void rb_to_array(const RingBuffer *rb, int *out) {
    for (size_t i = 0; i < rb->len; i++) out[i] = rb_get(rb, i);
}
