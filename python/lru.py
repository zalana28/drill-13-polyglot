"""LRUCache: fixed-capacity cache, string keys -> any value.

get() refreshes recency and returns None when absent. put() evicts the
least-recently-used entry when full; putting an existing key updates its
value and refreshes its recency. Backed by collections.OrderedDict
(move_to_end + popitem(last=False)).

Note: storing None as a value is out of scope — None means "absent".
"""

from collections import OrderedDict


class LRUCache:
    def __init__(self, capacity):
        if capacity <= 0:
            raise ValueError("capacity must be > 0")
        self._capacity = capacity
        self._data = OrderedDict()

    def get(self, key):
        if key not in self._data:
            return None
        self._data.move_to_end(key)
        return self._data[key]

    def put(self, key, value):
        if key in self._data:
            self._data.move_to_end(key)
        self._data[key] = value
        if len(self._data) > self._capacity:
            self._data.popitem(last=False)  # evict least-recently-used

    def __len__(self):
        return len(self._data)

    def clear(self):
        self._data.clear()

    def items(self):
        """(key, value) pairs, least-recently-used -> most-recently-used."""
        return list(self._data.items())
