# Drill #13 — Polyglot Core: identical spec, three languages

## Shared spec (identical in C, Python, JavaScript)

### RingBuffer — capacity N > 0
- `push(x)`: appends x; when full, overwrites the OLDEST element.
- Iteration / to-list: oldest → newest.
- `len`: number of elements currently held.
- `clear()`: empties the buffer; push works again afterwards.
- Invalid capacity (N <= 0): error (C: create returns NULL; Python: ValueError; JS: RangeError).

### LRUCache — capacity N > 0
- `get(key)`: returns the value and refreshes recency; returns null/None (absent) when the key is not present. (Storing null as a value is out of scope.)
- `put(key, value)`: inserts; when full, evicts the LEAST-recently-used entry; putting an existing key updates its value AND refreshes its recency.
- `len`: number of entries.
- `clear()`: empties the cache.
- `items()`: (key, value) pairs ordered least-recently-used → most-recently-used, so eviction order is directly observable.
- Invalid capacity (N <= 0): error, same as RingBuffer.

## Languages
- C (gcc 13.3.0) — manual memory management, linear-search LRU (O(n); a hash map would be O(1), noted in README).
- Python (3.12.3) — deque(maxlen) + OrderedDict.
- JavaScript (Node v24.22.1) — array + head index, Map for LRU.
- NOT TESTED (not available in this sandbox): Go, Rust, Java, PHP — no toolchain installed; apt update was attempted to obtain Go, ran 6+ minutes without output, and was stopped. The compiled-language comparison point is therefore honest NOT TESTED, not inferred.

## Layout
- `c/` — ringbuffer.h/.c, lru.h/.c, test.c, Makefile
- `python/` — ringbuffer.py, lru.py, test_polyglot.py
- `js/` — ringbuffer.js, lru.js, test.js
- `README.md` — comparison table (memory, errors, concurrency, when to pick which)
