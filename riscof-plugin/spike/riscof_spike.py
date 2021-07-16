import os
import re
import shutil
import subprocess
import shlex
import logging
import random
import string
from string import Template
import sys

import riscof.utils as utils
import riscof.constants as constants
from riscof.pluginTemplate import pluginTemplate

logger = logging.getLogger()

class spike(pluginTemplate):
    __model__ = "spike"

    #TODO: please update the below to indicate family, version, etc of your DUT.
    __version__ = "XXX"

    def __init__(self, *args, **kwargs):
        sclass = super().__init__(*args, **kwargs)

        config = kwargs.get('config')

        self.dut_exe = os.path.join(config['PATH'] if 'PATH' in config else "","spike")
        self.num_jobs = str(config['jobs'] if 'jobs' in config else 1)
        if config is None:
            logger.error("Please enter input file paths in configuration.")
            raise SystemExit
        try:
            self.isa_spec = os.path.abspath(config['ispec'])
            self.platform_spec = os.path.abspath(config['pspec'])
            self.pluginpath = os.path.abspath(config['pluginpath'])
            self.make = config['make'] if 'make' in config else 'make'
        except KeyError as e:
            logger.error("Please check the spike section in config for missing values.")
            logger.error(e)
            raise SystemExit
        logger.debug("SPIKE Parallel plugin initialised using the following configuration.")
        for entry in config:
            logger.debug(entry+' : '+config[entry])
        return sclass

    def initialise(self, suite, work_dir, archtest_env):
        self.work_dir = work_dir

        #TODO: The following assumes you are using the riscv-gcc toolchain. If
        #      not please change appropriately
        self.compile_cmd = 'riscv{1}-unknown-elf-gcc -march={0} \
         -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles\
         -T '+self.pluginpath+'/env/link.ld\
         -I '+self.pluginpath+'/env/\
         -I ' + archtest_env



        # set all the necessary variables like compile command, elf2hex
        # commands, objdump cmds. etc whichever you feel necessary and required
        # for your plugin.

    def build(self, isa_yaml, platform_yaml):
        ispec = utils.load_yaml(isa_yaml)['hart0']
        self.xlen = ('64' if 64 in ispec['supported_xlen'] else '32')
        self.isa = 'rv' + self.xlen
        #TODO: The following assumes you are using the riscv-gcc toolchain. If
        #      not please change appropriately
        self.compile_cmd = self.compile_cmd+' -mabi='+('lp64 ' if 64 in ispec['supported_xlen'] else 'ilp32 ')
        if "I" in ispec["ISA"]:
            self.isa += 'i'
        if "M" in ispec["ISA"]:
            self.isa += 'm'
        if "C" in ispec["ISA"]:
            self.isa += 'c'

        compiler = "riscv{0}-unknown-elf-gcc".format(self.xlen)
        if shutil.which(compiler) is None:
            logger.error(compiler+": executable not found. Please check environment setup.")
            raise SystemExit
        if shutil.which(self.dut_exe) is None:
            logger.error(self.dut_exe+ ": executable not found. Please check environment setup.")
            raise SystemExit
        if shutil.which(self.make) is None:
            logger.error(self.make+": executable not found. Please check environment setup.")
            raise SystemExit

        # based on the validated isa and platform configure your simulator or
        # build your RTL here

    def runTests(self, testList, cgf_file=None):
        make = utils.makeUtil(makefilePath=os.path.join(self.work_dir, "Makefile." + self.name[:-1]))
        make.makeCommand = self.make + ' -j' + self.num_jobs
        for file in testList:
            testentry = testList[file]
            test = testentry['test_path']
            test_dir = testentry['work_dir']

            if cgf_file is not None:
                elf = 'ref.elf'
            else:
                elf = 'dut.elf'

            execute = "@cd "+testentry['work_dir']+";"

            cmd = self.compile_cmd.format(testentry['isa'].lower(), self.xlen) + ' ' + test + ' -o ' + elf

            #TODO: we are using -D to enable compile time macros. If your
            #      toolchain is not riscv-gcc you may want to change the below code
            compile_cmd = cmd + ' -D' + " -D".join(testentry['macros'])
            execute+=compile_cmd+";"

            sig_file = os.path.join(test_dir, self.name[:-1] + ".signature")

            if cgf_file is not None:
                execute += self.dut_exe + ' --log-commits --log dump --isa={0} +signature={1} +signature-granularity=4 {2};'.format(self.isa, sig_file, elf)
            else:
                execute += self.dut_exe + ' --isa={0} +signature={1} +signature-granularity=4 {2};'.format(self.isa, sig_file, elf)

            cov_str = ' '
            for label in testentry['coverage_labels']:
                cov_str+=' -l '+label

            if cgf_file is not None:
                coverage_cmd = 'riscv_isac --verbose error coverage -d \
                        -t dump --parser-name spike -o coverage.rpt  \
                        --sig-label begin_signature  end_signature \
                        --test-label rvtest_code_begin rvtest_code_end \
                        -e ref.elf -c ' + ' -c '.join(cgf_file) + cov_str + " -x "+ self.xlen +';'
            else:
                coverage_cmd = ''


            execute+=coverage_cmd

            make.add_target(execute)
        make.execute_all(self.work_dir)
