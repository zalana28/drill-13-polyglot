#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stddef.h>

/* Fixed-capacity ring buffer of ints. push() overwrites the oldest element
 * when full. Iteration order is oldest -> newest. */

typedef struct {
    int *buf;
    size_t cap;    /* capacity, > 0 */
    size_t len;    /* elements currently held */
    size_t start;  /* index of the oldest element */
} RingBuffer;

/* Returns NULL when cap == 0 or allocation fails. Caller owns the result. */
RingBuffer *rb_create(size_t cap);
void rb_free(RingBuffer *rb);
void rb_push(RingBuffer *rb, int value);
size_t rb_len(const RingBuffer *rb);
void rb_clear(RingBuffer *rb);
/* Element at position i in oldest->newest order (0 <= i < len). */
int rb_get(const RingBuffer *rb, size_t i);
/* Copies elements oldest->newest into out (caller provides len slots). */
void rb_to_array(const RingBuffer *rb, int *out);

#endif
