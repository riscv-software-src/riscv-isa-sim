#!/usr/bin/python

import os
import testlib
import unittest
import tempfile
import time

class EbreakTest(unittest.TestCase):
    def setUp(self):
        self.binary = testlib.compile("ebreak.s")

    def test_noport(self):
        """Make sure that we can run past ebreak when --gdb-port isn't used."""
        spike = testlib.Spike(self.binary, with_gdb=False, timeout=10)
        result = spike.wait()
        self.assertEqual(result, 0)

    def test_nogdb(self):
        """Make sure that we can run past ebreak when gdb isn't attached."""
        spike = testlib.Spike(self.binary, timeout=10)
        result = spike.wait()
        self.assertEqual(result, 0)

if __name__ == '__main__':
    unittest.main()
