"""Tests for the Python RingBuffer + LRUCache (same spec as C and JS)."""

import unittest

from lru import LRUCache
from ringbuffer import RingBuffer


class TestRingBuffer(unittest.TestCase):
    def test_n1_second_push_overwrites(self):
        rb = RingBuffer(1)
        rb.push(10)
        rb.push(20)
        self.assertEqual(len(rb), 1)
        self.assertEqual(rb.to_list(), [20])

    def test_overwrite_when_full(self):
        rb = RingBuffer(3)
        for v in (1, 2, 3, 4):
            rb.push(v)
        self.assertEqual(len(rb), 3)
        self.assertEqual(rb.to_list(), [2, 3, 4])

    def test_order_oldest_to_newest_after_wrap(self):
        rb = RingBuffer(3)
        for v in (1, 2, 3, 4, 5):
            rb.push(v)
        self.assertEqual(rb.to_list(), [3, 4, 5])
        self.assertEqual(list(rb), [3, 4, 5])

    def test_clear_and_len_then_push_again(self):
        rb = RingBuffer(3)
        rb.push(1)
        rb.push(2)
        self.assertEqual(len(rb), 2)
        rb.clear()
        self.assertEqual(len(rb), 0)
        self.assertEqual(rb.to_list(), [])
        rb.push(9)
        self.assertEqual(rb.to_list(), [9])

    def test_invalid_capacity(self):
        with self.assertRaises(ValueError):
            RingBuffer(0)
        with self.assertRaises(ValueError):
            RingBuffer(-3)


class TestLRUCache(unittest.TestCase):
    def test_get_refreshes_recency(self):
        c = LRUCache(2)
        c.put("a", 1)
        c.put("b", 2)
        self.assertEqual(c.get("a"), 1)
        c.put("c", 3)  # must evict b, not a
        self.assertIsNone(c.get("b"))
        self.assertEqual(c.get("a"), 1)
        self.assertEqual(c.get("c"), 3)
        self.assertEqual(len(c), 2)

    def test_eviction_order_without_refresh(self):
        c = LRUCache(2)
        c.put("a", 1)
        c.put("b", 2)
        c.put("c", 3)
        self.assertEqual(c.items(), [("b", 2), ("c", 3)])
        self.assertEqual(len(c), 2)

    def test_put_existing_updates_value_and_recency(self):
        c = LRUCache(2)
        c.put("a", 1)
        c.put("b", 2)
        c.put("a", 10)  # update + refresh
        self.assertEqual(len(c), 2)
        c.put("c", 3)  # must evict b, not a
        self.assertIsNone(c.get("b"))
        self.assertEqual(c.get("a"), 10)
        self.assertEqual(c.items(), [("c", 3), ("a", 10)])

    def test_get_absent_is_none_and_clear(self):
        c = LRUCache(2)
        self.assertIsNone(c.get("nope"))
        c.put("a", 1)
        self.assertEqual(len(c), 1)
        c.clear()
        self.assertEqual(len(c), 0)
        self.assertIsNone(c.get("a"))
        c.put("z", 26)
        self.assertEqual(c.get("z"), 26)

    def test_n1_put_evicts(self):
        c = LRUCache(1)
        c.put("a", 1)
        c.put("b", 2)
        self.assertEqual(len(c), 1)
        self.assertIsNone(c.get("a"))
        self.assertEqual(c.get("b"), 2)

    def test_invalid_capacity(self):
        with self.assertRaises(ValueError):
            LRUCache(0)


if __name__ == "__main__":
    unittest.main(verbosity=2)
