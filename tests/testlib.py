import os.path
import pexpect
import subprocess
import tempfile
import testlib
import unittest

# Note that gdb comes with its own testsuite. I was unable to figure out how to
# run that testsuite against the spike simulator.

def find_file(path):
    for directory in (os.getcwd(), os.path.dirname(testlib.__file__)):
        fullpath = os.path.join(directory, path)
        if os.path.exists(fullpath):
            return fullpath
    raise ValueError("Couldn't find %r." % path)

def compile(src, dst):
    """Compile a single .c file into a binary."""
    cc = os.path.expandvars("$RISCV/bin/riscv64-unknown-elf-gcc")
    return os.system("%s -g -o %s %s" % (cc, dst, find_file(src)))

def spike(binary, halted=False):
    cmd = [find_file("spike")]
    if halted:
        cmd.append('-H')
    cmd += ['pk', binary]
    logfile = open("spike.log", "w")
    return subprocess.Popen(cmd, stdout=logfile, stderr=logfile)

class Gdb(object):
    def __init__(self):
        path = os.path.expandvars("$RISCV/bin/riscv64-unknown-elf-gdb")
        self.child = pexpect.spawn(path)
        self.child.logfile = file("gdb.log", "w")
        self.wait()

    def wait(self):
        """Wait for prompt."""
        self.child.expect("\(gdb\)")

    def command(self, command):
        self.child.sendline(command)
        self.child.expect("\n")
        self.child.expect("\(gdb\)")
        return self.child.before.strip()
