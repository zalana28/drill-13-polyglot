'use strict';

// LRUCache: fixed-capacity cache, string keys -> any value.
// get() refreshes recency and returns null when absent. put() evicts the
// least-recently-used entry when full; putting an existing key updates its
// value and refreshes its recency. Backed by Map, which keeps insertion
// order: refresh = delete + set; eviction = delete the first key.
//
// Note: storing null as a value is out of scope — null means "absent".

class LRUCache {
  constructor(capacity) {
    if (!Number.isInteger(capacity) || capacity <= 0) {
      throw new RangeError('capacity must be an integer > 0');
    }
    this._capacity = capacity;
    this._data = new Map();
  }

  get(key) {
    if (!this._data.has(key)) return null;
    const value = this._data.get(key);
    this._data.delete(key);
    this._data.set(key, value); // refresh recency
    return value;
  }

  put(key, value) {
    if (this._data.has(key)) this._data.delete(key);
    this._data.set(key, value);
    if (this._data.size > this._capacity) {
      // Evict the least-recently-used entry: the first key in the Map.
      this._data.delete(this._data.keys().next().value);
    }
  }

  get size() {
    return this._data.size;
  }

  clear() {
    this._data.clear();
  }

  // [key, value] pairs, least-recently-used -> most-recently-used.
  items() {
    return [...this._data.entries()];
  }
}

module.exports = { LRUCache };
