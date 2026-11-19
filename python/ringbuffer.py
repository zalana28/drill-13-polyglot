"""RingBuffer: fixed-capacity buffer of any values.

push() overwrites the oldest element when full. Iteration is oldest ->
newest. Backed by collections.deque(maxlen=...) — the stdlib already
implements exactly these semantics.
"""

from collections import deque


class RingBuffer:
    def __init__(self, capacity):
        if capacity <= 0:
            raise ValueError("capacity must be > 0")
        self._capacity = capacity
        self._data = deque(maxlen=capacity)

    def push(self, value):
        self._data.append(value)

    def __len__(self):
        return len(self._data)

    def __iter__(self):
        return iter(self._data)

    def to_list(self):
        return list(self._data)

    def clear(self):
        self._data.clear()
