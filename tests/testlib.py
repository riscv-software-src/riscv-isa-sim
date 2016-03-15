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

def compile(src):
    """Compile a single .c file into a binary."""
    src = find_file(src)
    dst = os.path.splitext(src)[0]
    cc = os.path.expandvars("$RISCV/bin/riscv64-unknown-elf-gcc")
    cmd = "%s -g -o %s %s" % (cc, dst, src)
    result = os.system(cmd)
    assert result == 0, "%r failed" % cmd
    return dst

def spike(binary, halted=False):
    cmd = [find_file("spike")]
    if halted:
        cmd.append('-H')
    cmd += ['pk', binary]
    logfile = open("spike.log", "w")
    return subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=logfile, stderr=logfile)

class Gdb(object):
    def __init__(self):
        path = os.path.expandvars("$RISCV/bin/riscv64-unknown-elf-gdb")
        self.child = pexpect.spawn(path)
        self.child.logfile = file("gdb.log", "w")
        self.wait()
        self.command("set width 0")
        self.command("set height 0")

    def wait(self):
        """Wait for prompt."""
        self.child.expect("\(gdb\)")

    def command(self, command):
        self.child.sendline(command)
        self.child.expect("\n")
        self.child.expect("\(gdb\)")
        return self.child.before.strip()

    def x(self, address, size='w'):
        output = self.command("x/%s %s" % (size, address))
        value = int(output.split(':')[1].strip())
        return value

    def p(self, obj):
        output = self.command("p %s" % obj)
        value = int(output.split('=')[-1].strip())
        return value

    def stepi(self):
        return self.command("stepi")
