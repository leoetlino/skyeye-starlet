#!/usr/bin/env python

import sys
from ipc import *
import struct

if len(sys.argv) < 2:
	print "Usage: deletetitle.py <title ID>"
	sys.exit(2)

print "Waiting for IPC to start up..."
ipc = SkyeyeIPC()
ipc.init()
print "IPC ready"

fd = ipc.IOSOpen("/dev/es")
print "ES fd: %d" % fd
if fd < 0:
	print "Error opening ES"
	sys.exit(1)

title_id = int(sys.argv[1], 16)
res = ipc.IOSIoctlv(fd, 0x17, "q", title_id)
if res < 0:
	print "Error %d" % res
	ipc.IOSClose(fd)
	sys.exit(1)

ipc.IOSClose(fd)
