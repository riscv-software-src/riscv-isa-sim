#!/usr/bin/python

import os
import testlib
import unittest
import tempfile
import time

class InstantHaltTest(unittest.TestCase):
    def setUp(self):
        self.binary = testlib.compile("debug.c")
        self.spike, self.port = testlib.spike(self.binary, halted=True)
        self.gdb = testlib.Gdb()
        self.gdb.command("file %s" % self.binary)
        self.gdb.command("target extended-remote localhost:%d" % self.port)

    def tearDown(self):
        self.spike.kill()
        self.spike.wait()

    def test_instant_halt(self):
        self.assertEqual(0x1000, self.gdb.p("$pc"))
        # For some reason instret resets to 0.
        self.assertLess(self.gdb.p("$instret"), 8)
        self.gdb.command("stepi")
        self.assertNotEqual(0x1000, self.gdb.p("$pc"))

class DebugTest(unittest.TestCase):
    def setUp(self):
        self.binary = testlib.compile("debug.c")
        self.spike, self.port = testlib.spike(self.binary, halted=False)
        self.gdb = testlib.Gdb()
        self.gdb.command("file %s" % self.binary)
        self.gdb.command("target extended-remote localhost:%d" % self.port)

    def tearDown(self):
        self.spike.kill()
        self.spike.wait()

    def test_turbostep(self):
        """Single step a bunch of times."""
        self.gdb.command("p i=0");
        last_pc = None
        for _ in range(100):
            self.gdb.command("stepi")
            pc = self.gdb.command("p $pc")
            self.assertNotEqual(last_pc, pc)
            last_pc = pc

    def test_exit(self):
        self.gdb.command("p i=0");
        output = self.gdb.command("c")
        self.assertIn("Continuing", output)
        self.assertIn("Remote connection closed", output)

    def test_breakpoint(self):
        self.gdb.command("p i=0");
        self.gdb.command("b print_row")
        # The breakpoint should be hit exactly 10 times.
        for i in range(10):
            output = self.gdb.command("c")
            self.assertIn("Continuing", output)
            self.assertIn("length=%d" % i, output)
            self.assertIn("Breakpoint 1", output)
        output = self.gdb.command("c")
        self.assertIn("Continuing", output)
        self.assertIn("Remote connection closed", output)

    def test_registers(self):
        self.gdb.command("p i=0");
        # Try both forms to test gdb.
        for cmd in ("info all-registers", "info registers all"):
            output = self.gdb.command(cmd)
            self.assertNotIn("Could not", output)
            for reg in ('zero', 'ra', 'sp', 'gp', 'tp'):
                self.assertIn(reg, output)
        # mcpuid is one of the few registers that should have the high bit set
        # (for rv64).
        # Leave this commented out until gdb and spike agree on the encoding of
        # mcpuid (which is going to be renamed to misa in any case).
        #self.assertRegexpMatches(output, ".*mcpuid *0x80")

        # The instret register should always be changing.
        last_instret = None
        for _ in range(5):
            instret = self.gdb.p("$instret")
            self.assertNotEqual(instret, last_instret)
            last_instret = instret
            self.gdb.command("stepi")

    def test_interrupt(self):
        """Sending gdb ^C while the program is running should cause it to halt."""
        self.gdb.c(wait=False)
        time.sleep(0.1)
        self.gdb.interrupt()
        self.gdb.command("p i=123");
        self.gdb.c(wait=False)
        time.sleep(0.1)
        self.gdb.interrupt()
        self.gdb.command("p i=0");
        output = self.gdb.c()
        self.assertIn("Continuing", output)
        self.assertIn("Remote connection closed", output)

class RegsTest(unittest.TestCase):
    def setUp(self):
        self.binary = testlib.compile("regs.s")
        self.spike, self.port = testlib.spike(self.binary, halted=False)
        self.gdb = testlib.Gdb()
        self.gdb.command("file %s" % self.binary)
        self.gdb.command("target extended-remote localhost:%d" % self.port)

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
            self.gdb.command("p $%s=%d" % (r, (0xdeadbeef<<i)+17))
        self.gdb.command("p $a0=data")
        self.gdb.command("b all_done")
        output = self.gdb.command("c")
        self.assertIn("Breakpoint 1", output)

        # Just to get this data in the log.
        self.gdb.command("x/30gx data")
        self.gdb.command("info registers")
        for n in range(len(regs)):
            self.assertEqual(self.gdb.x("data+%d" % (8*n), 'g'),
                    (0xdeadbeef<<n)+17)

    def test_write_csrs(self):
        # As much a test of gdb as of the simulator.
        self.gdb.p("$mscratch=0")
        self.gdb.stepi()
        self.assertEqual(self.gdb.p("$mscratch"), 0)
        self.gdb.p("$mscratch=123")
        self.gdb.stepi()
        self.assertEqual(self.gdb.p("$mscratch"), 123)

        self.gdb.command("p $fflags=9")
        self.gdb.command("p $pc=write_regs")
        self.gdb.command("p $a0=data")
        self.gdb.command("b all_done")
        self.gdb.command("c")

        self.assertEqual(9, self.gdb.p("$fflags"))
        self.assertEqual(9, self.gdb.p("$x1"))
        self.assertEqual(9, self.gdb.p("$csr1"))

#class MprvTest(unittest.TestCase):
#    def setUp(self):
#        self.binary = testlib.compile("mprv.S")
#        self.spike, self.port = testlib.spike(self.binary, halted=False)
#        self.gdb = testlib.Gdb()
#        self.gdb.command("file %s" % self.binary)
#        self.gdb.command("target extended-remote localhost:%d" % self.port)
#
#    def tearDown(self):
#        self.spike.kill()
#        self.spike.wait()
#
#    def test_mprv(self):
#        """Test that the debugger can access memory when MPRV is set."""
#        output = self.gdb.command("p/x data");
#        self.assertIn("0xbead", output)

if __name__ == '__main__':
    unittest.main()
