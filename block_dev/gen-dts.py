#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import os

base = int(sys.argv[1],16)
filePath = sys.argv[2]
fsize = os.path.getsize(filePath)
len = int(fsize / 512)
fp = open(sys.argv[4],"w")
fp.write("/include/ \"" + sys.argv[3] + "\"\n\n/ {\n  vblock@")
fp.write(sys.argv[1])
fp.write(" {\n    compatible = \"plct,vblock\";\n    reg = <0x0 0x")
fp.write(sys.argv[1])
fp.write(" 0x0 ")
fp.write(hex(fsize))
fp.write(">;\n    len = <")
fp.write(hex(len))
fp.write(">;\n  };\n};\n")
