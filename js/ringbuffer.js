'use strict';

// RingBuffer: fixed-capacity buffer of any values.
// push() overwrites the oldest element when full. Iteration is oldest ->
// newest. Implemented manually with an array + head index (no stdlib
// structure provides exactly this).

class RingBuffer {
  constructor(capacity) {
    if (!Number.isInteger(capacity) || capacity <= 0) {
      throw new RangeError('capacity must be an integer > 0');
    }
    this._capacity = capacity;
    this._buf = new Array(capacity);
    this._len = 0;
    this._start = 0; // index of the oldest element
  }

  push(value) {
    if (this._len < this._capacity) {
      this._buf[(this._start + this._len) % this._capacity] = value;
      this._len += 1;
    } else {
      // Full: overwrite the oldest element, advance the start.
      this._buf[this._start] = value;
      this._start = (this._start + 1) % this._capacity;
    }
  }

  get length() {
    return this._len;
  }

  clear() {
    this._len = 0;
    this._start = 0;
  }

  get(i) {
    return this._buf[(this._start + i) % this._capacity];
  }

  toArray() {
    const out = [];
    for (let i = 0; i < this._len; i++) out.push(this.get(i));
    return out;
  }

  [Symbol.iterator]() {
    return this.toArray()[Symbol.iterator]();
  }
}

module.exports = { RingBuffer };
