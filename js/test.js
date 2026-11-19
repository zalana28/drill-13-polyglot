'use strict';

// Tests for the JavaScript RingBuffer + LRUCache (same spec as C and Python).

const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const { RingBuffer } = require('./ringbuffer');
const { LRUCache } = require('./lru');

describe('RingBuffer', () => {
  it('N=1: second push overwrites the only element', () => {
    const rb = new RingBuffer(1);
    rb.push(10);
    rb.push(20);
    assert.equal(rb.length, 1);
    assert.deepEqual(rb.toArray(), [20]);
  });

  it('overwrite when full: capacity 3, push 1..4 -> [2,3,4]', () => {
    const rb = new RingBuffer(3);
    for (const v of [1, 2, 3, 4]) rb.push(v);
    assert.equal(rb.length, 3);
    assert.deepEqual(rb.toArray(), [2, 3, 4]);
  });

  it('iteration order oldest->newest after wrap: push 1..5 -> [3,4,5]', () => {
    const rb = new RingBuffer(3);
    for (const v of [1, 2, 3, 4, 5]) rb.push(v);
    assert.deepEqual(rb.toArray(), [3, 4, 5]);
    assert.deepEqual([...rb], [3, 4, 5]);
    assert.equal(rb.get(0), 3);
    assert.equal(rb.get(2), 5);
  });

  it('clear + length; push works again after clear', () => {
    const rb = new RingBuffer(3);
    rb.push(1);
    rb.push(2);
    assert.equal(rb.length, 2);
    rb.clear();
    assert.equal(rb.length, 0);
    assert.deepEqual(rb.toArray(), []);
    rb.push(9);
    assert.deepEqual(rb.toArray(), [9]);
  });

  it('invalid capacity throws RangeError', () => {
    assert.throws(() => new RingBuffer(0), RangeError);
    assert.throws(() => new RingBuffer(-3), RangeError);
    assert.throws(() => new RingBuffer(1.5), RangeError);
  });
});

describe('LRUCache', () => {
  it('get refreshes recency: get(a) saves a from eviction', () => {
    const c = new LRUCache(2);
    c.put('a', 1);
    c.put('b', 2);
    assert.equal(c.get('a'), 1);
    c.put('c', 3); // must evict b, not a
    assert.equal(c.get('b'), null);
    assert.equal(c.get('a'), 1);
    assert.equal(c.get('c'), 3);
    assert.equal(c.size, 2);
  });

  it('eviction order without refresh: put a,b then c evicts a', () => {
    const c = new LRUCache(2);
    c.put('a', 1);
    c.put('b', 2);
    c.put('c', 3);
    assert.deepEqual(c.items(), [['b', 2], ['c', 3]]);
    assert.equal(c.size, 2);
  });

  it('put existing key updates value and refreshes recency', () => {
    const c = new LRUCache(2);
    c.put('a', 1);
    c.put('b', 2);
    c.put('a', 10); // update + refresh
    assert.equal(c.size, 2);
    c.put('c', 3); // must evict b, not a
    assert.equal(c.get('b'), null);
    assert.equal(c.get('a'), 10);
    assert.deepEqual(c.items(), [['c', 3], ['a', 10]]);
  });

  it('get absent returns null; length tracking; clear works', () => {
    const c = new LRUCache(2);
    assert.equal(c.get('nope'), null);
    c.put('a', 1);
    assert.equal(c.size, 1);
    c.clear();
    assert.equal(c.size, 0);
    assert.equal(c.get('a'), null);
    c.put('z', 26);
    assert.equal(c.get('z'), 26);
  });

  it('N=1: put b evicts a', () => {
    const c = new LRUCache(1);
    c.put('a', 1);
    c.put('b', 2);
    assert.equal(c.size, 1);
    assert.equal(c.get('a'), null);
    assert.equal(c.get('b'), 2);
  });

  it('invalid capacity throws RangeError', () => {
    assert.throws(() => new LRUCache(0), RangeError);
  });
});
