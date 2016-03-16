#!/usr/bin/python

import os
import testlib
import unittest
import tempfile
import time

class DebugTest(unittest.TestCase):
    def setUp(self):
        self.binary = testlib.compile("debug.c")
        self.spike = testlib.spike(self.binary, halted=False)
        self.gdb = testlib.Gdb()
        self.gdb.command("file %s" % self.binary)
        self.gdb.command("target extended-remote localhost:9824")
        self.gdb.command("p i=0");

    def tearDown(self):
        self.spike.kill()
        self.spike.wait()

    def test_turbostep(self):
        """Single step a bunch of times."""
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
        # Try both forms to test gdb.
        for cmd in ("info all-registers", "info registers all"):
            output = self.gdb.command(cmd)
            self.assertNotIn("Could not", output)
            for reg in ('zero', 'ra', 'sp', 'gp', 'tp'):
                self.assertIn(reg, output)
        # mcpuid is one of the few registers that should have the high bit set
        # (for rv64).
        self.assertRegexpMatches(output, ".*mcpuid *0x80")

        # The time register should always be changing.
        last_time = None
        for _ in range(5):
            time = self.gdb.p("$time")
            self.assertNotEqual(time, last_time)
            last_time = time
            self.gdb.command("stepi")

class RegsTest(unittest.TestCase):
    def setUp(self):
        self.binary = testlib.compile("regs.s")
        self.spike = testlib.spike(self.binary, halted=False)
        self.gdb = testlib.Gdb()
        self.gdb.command("file %s" % self.binary)
        self.gdb.command("target extended-remote localhost:9824")

    def tearDown(self):
        self.spike.kill()
        self.spike.wait()

    def test_write_gprs(self):
        # Note a0 is missing from this list since it's used to hold the
        # address.
        regs = ("ra", "sp", "gp", "tp", "t0", "t1", "t2", "fp", "s1",
                "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4",
                "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5",
                "t6")

        self.gdb.command("p $pc=write_regs")
        for i, r in enumerate(regs):
            self.gdb.command("p $%s=%d" % (r, i*0xdeadbeef+17))
        self.gdb.command("p $a0=data")
        self.gdb.command("b all_done")
        output = self.gdb.command("c")
        self.assertIn("Breakpoint 1", output)

        for n in range(len(regs)):
            self.assertEqual(self.gdb.x("data+%d" % (8*n), 'g'),
                    n*0xdeadbeef+17)

    def test_write_csrs(self):
        # As much a test of gdb as of the simulator.
        self.gdb.p("$mscratch=0")
        self.gdb.stepi()
        self.assertEqual(self.gdb.p("$mscratch"), 0)
        self.gdb.p("$mscratch=123")
        self.gdb.stepi()
        self.assertEqual(self.gdb.p("$mscratch"), 123)

if __name__ == '__main__':
    unittest.main()
