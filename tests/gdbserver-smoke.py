#!/usr/bin/python

import os
import testlib
import unittest
import tempfile
import time

class SmokeTest(unittest.TestCase):
    def setUp(self):
        self.tmpf = tempfile.NamedTemporaryFile()
        testlib.compile("debug.c", self.tmpf.name)
        self.spike = testlib.spike(self.tmpf.name, halted=False)
        self.gdb = testlib.Gdb()
        self.gdb.command("file %s" % self.tmpf.name)
        self.gdb.command("target extended-remote localhost:9824")
        self.gdb.command("p i=0");

    def tearDown(self):
        self.spike.kill()
        self.spike.wait()

    def test_turbostep(self):
        """Single step until the program exits. TODO"""
        last_pc = None
        for _ in range(100):
            self.gdb.command("stepi")
            pc = self.gdb.command("p $pc")
            self.assertNotEqual(last_pc, pc)
            last_pc = pc

    def test_exit(self):
        output = self.gdb.command("c")
        self.assertIn("Continuing", output)
        self.assertIn("Remote connection closed", output)

    def test_breakpoint(self):
        self.gdb.command("b print_row")
        # The breakpoint should be hit exactly 10 times.
        for _ in range(10):
            output = self.gdb.command("c")
            self.assertIn("Continuing", output)
            self.assertIn("Breakpoint 1", output)
        output = self.gdb.command("c")
        self.assertIn("Continuing", output)
        self.assertIn("Remote connection closed", output)

    def test_registers(self):
        output = self.gdb.command("info all-registers")
        self.assertNotIn("Could not", output)
        for reg in ('zero', 'ra', 'sp', 'gp', 'tp'):
            self.assertIn(reg, output)
        # mcpuid is one of the few registers that should have the high bit set
        # (for rv64).
        self.assertRegexpMatches(output, ".*mcpuid *0x80")

        # The time register should always be changing.
        last_time = None
        for _ in range(5):
            time = self.gdb.command("p $time").split('=')[-1]
            self.assertNotEqual(time, last_time)
            last_time = time
            self.gdb.command("stepi")

if __name__ == '__main__':
    unittest.main()
