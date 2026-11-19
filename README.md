# Drill #13 — Polyglot Core: RingBuffer + LRUCache in three languages

Identical spec in C, Python and JavaScript (see SPEC.md). Each language has
its own implementation and test suite; all three pass.

## Results (fresh runs, 2026-11-19)

- C (gcc 13.3.0, -std=c11 -Wall -Wextra -O2): `make check` → 10 tests,
  32 checks, 0 failures. One real bug found and fixed during the run:
  `strdup` is POSIX, not C11 — under `-std=c11` it was implicitly declared
  (returning int, truncating the pointer) and the test binary segfaulted.
  Fix: `#define _POSIX_C_SOURCE 200809L` in lru.c. After the fix: clean
  build, zero warnings, all green.
- Python (3.12.3): `python3 -m unittest test_polyglot` → 11 tests, OK.
- JavaScript (Node v24.22.1): `node --test test.js` → 11 tests, 0 fail.

Per-language test count is 10–11 (requirement: ≥6), covering N=1, full
overwrite, eviction order, get-refresh, put-existing, clear/len, absent
get, and invalid capacity.

## Comparison — understanding, not syntax

| | C (gcc 13.3.0) | Python (3.12.3) | JavaScript (Node 24) |
|---|---|---|---|
| Memory management | Manual: malloc/free everywhere; the caller owns every allocation; a missed free leaks, a double free corrupts. RingBuffer owns its int array; LRU owns strdup'd keys + nodes. | GC (reference counting + cycle collector). deque/OrderedDict own their storage; nothing to free. | GC (V8, mark-and-sweep + generational). Arrays/Maps own their storage; nothing to free. |
| Error handling | No exceptions. Errors are return values: create returns NULL on cap==0 or alloc failure; get returns found/not-found via an out-param. The caller MUST check — an unchecked NULL dereference segfaults (as the strdup bug showed). | Exceptions: ValueError on cap<=0. get returns None for absent (documented: None-as-value is out of scope). | Exceptions: RangeError on cap<=0 (including non-integers). get returns null for absent (same None/null caveat). |
| Concurrency | None built in. The structures are not thread-safe; a mutex would be the caller's job. No runtime help. | GIL: threads share the structures "safely" only in the sense that bytecode is atomic-ish; real safety still needs a lock. multiprocessing sidesteps the GIL. | Single-threaded event loop: no data races inside one process by construction; worker_threads / child processes for parallelism. |
| Data structures used | Hand-rolled: circular array + head index; doubly-linked list + linear scan for LRU (O(n) get/put). A hash map would make LRU O(1) — deliberately not done, to keep the C code short and auditable. | stdlib does the work: deque(maxlen=N) IS a ring buffer; OrderedDict + move_to_end/popitem IS an LRU. The implementation is ~15 lines per class. | Mixed: RingBuffer is hand-rolled (array + head index — no stdlib ring buffer); LRU uses Map, whose insertion order makes refresh (delete+set) and eviction (first key) one-liners. |
| When to pick it | Hot paths, embedded, systems code, FFI, or when you must control every byte and every allocation. Cost: you own every bug (see the strdup segfault). | Scripts, data work, teaching, fast iteration. Cost: runtime overhead, GIL limits true parallelism. | Servers, CLIs, and anywhere the code already lives in Node. Cost: single-threaded by default; number semantics (all numbers are doubles) need care for integer-heavy work. |

## What the same spec reveals

- The spec is the same, but the failure modes are not: C fails with a
  segfault on a missing feature-test macro; Python/JS fail with a clean
  exception on a bad capacity. The language chooses where the bug lands.
- The LRU is the honest discriminator: Python and JS get it almost free
  from the stdlib (OrderedDict / Map keep order for you); C pays for it in
  pointer code and a linear scan. That is the real price of "no runtime".
- RingBuffer is the opposite discriminator: C and JS both hand-roll the
  circular index arithmetic; Python's deque already is one. The idiomatic
  solution is whatever the stdlib already solved.

## NOT TESTED (honest)

- Go, Rust, Java, PHP: no toolchain in this sandbox (checked: go/rustc/
  java/php not found). An `apt-get update` was attempted to obtain Go; it
  ran 6+ minutes without producing output and was stopped — Go was not
  installed. JavaScript (Node v24.22.1, present in the sandbox) is the
  third language instead. The compiled-language comparison point (Go/Rust)
  is therefore NOT TESTED, not inferred.

## Layout

- `SPEC.md` — the shared spec.
- `c/` — ringbuffer.h/.c, lru.h/.c, test.c, Makefile (`make check`).
- `python/` — ringbuffer.py, lru.py, test_polyglot.py (`python3 -m unittest test_polyglot`).
- `js/` — ringbuffer.js, lru.js, test.js (`node --test test.js`).
