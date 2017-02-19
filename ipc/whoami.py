#!/usr/bin/env python

import sys
from ipc import *
import struct

ipc = SkyeyeIPC()
ipc.init()

fd = ipc.IOSOpen("/dev/es")
if fd < 0:
	print "Error opening ES"
	sys.exit(1)

buf = ipc.makebuf(8)
res = ipc.IOSIoctlv(fd, 0x20, ":d", buf)
if res < 0:
	print "Error %d" % res
	buf.free()
	ipc.IOSClose(fd)
	sys.exit(1)

title_id = struct.unpack(">Q", buf.read())[0]
print "%016x" % title_id
buf.free()

ipc.IOSClose(fd)

