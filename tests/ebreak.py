#!/usr/bin/python

import os
import testlib
import unittest
import tempfile
import time

class EbreakTest(unittest.TestCase):
    def setUp(self):
        self.binary = testlib.compile("ebreak.s")

    def test_ebreak(self):
        """Make sure that we can run past ebreak without halting when a
        debugger isn't attached."""
        spike = testlib.spike(self.binary, with_gdb=False, timeout=10)
        result = spike.wait()
        self.assertEqual(result, 0)

if __name__ == '__main__':
    unittest.main()
