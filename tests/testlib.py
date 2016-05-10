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
    return None

def compile(*args):
    """Compile a single .c file into a binary."""
    dst = os.path.splitext(args[0])[0]
    cc = os.path.expandvars("$RISCV/bin/riscv64-unknown-elf-gcc")
    cmd = [cc, "-g", "-O", "-o", dst]
    for arg in args:
        found = find_file(arg)
        if found:
            cmd.append(found)
        else:
            cmd.append(arg)
    cmd = " ".join(cmd)
    result = os.system(cmd)
    assert result == 0, "%r failed" % cmd
    return dst

def unused_port():
    # http://stackoverflow.com/questions/2838244/get-open-tcp-port-in-python/2838309#2838309
    import socket
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("",0))
    port = s.getsockname()[1]
    s.close()
    return port

class Spike(object):
    def __init__(self, binary, halted=False, with_gdb=True, timeout=None):
        """Launch spike. Return tuple of its process and the port it's running on."""
        cmd = []
        if timeout:
            cmd += ["timeout", str(timeout)]

        cmd += [find_file("spike")]
        if halted:
            cmd.append('-H')
        if with_gdb:
            self.port = unused_port()
            cmd += ['--gdb-port', str(self.port)]
        cmd.append('pk')
        if binary:
            cmd.append(binary)
        logfile = open("spike.log", "w")
        self.process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=logfile,
                stderr=logfile)

    def __del__(self):
        try:
            self.process.kill()
            self.process.wait()
        except OSError:
            pass

    def wait(self, *args, **kwargs):
        return self.process.wait(*args, **kwargs)

class Gdb(object):
    def __init__(self):
        path = os.path.expandvars("$RISCV/bin/riscv64-unknown-elf-gdb")
        self.child = pexpect.spawn(path)
        self.child.logfile = file("gdb.log", "w")
        self.wait()
        self.command("set width 0")
        self.command("set height 0")
        # Force consistency.
        self.command("set print entry-values no")

    def wait(self):
        """Wait for prompt."""
        self.child.expect("\(gdb\)")

    def command(self, command, timeout=-1):
        self.child.sendline(command)
        self.child.expect("\n", timeout=timeout)
        self.child.expect("\(gdb\)", timeout=timeout)
        return self.child.before.strip()

    def c(self, wait=True):
        if wait:
            return self.command("c")
        else:
            self.child.sendline("c")
            self.child.expect("Continuing")

    def interrupt(self):
        self.child.send("\003");
        self.child.expect("\(gdb\)")

    def x(self, address, size='w'):
        output = self.command("x/%s %s" % (size, address))
        value = int(output.split(':')[1].strip(), 0)
        return value

    def p(self, obj):
        output = self.command("p %s" % obj)
        value = int(output.split('=')[-1].strip())
        return value

    def stepi(self):
        return self.command("stepi")
