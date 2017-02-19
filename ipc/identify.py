#!/usr/bin/env python

import sys
from ipc import *
import struct
sys.path.append(os.path.realpath(os.path.dirname(sys.argv[0]))+"/../../pywii/Common")
import pywii as wii

wii.loadkeys()

if len(sys.argv) < 2:
	print "Usage: identify.py <path to wfuse mount point>"
	sys.exit(2)

certificates = open(sys.argv[1] + "/iso/part/main/cert.bin", "rb").read()
sticket = open(sys.argv[1] + "/iso/part/main/ticket.bin", "rb").read()
stmd = open(sys.argv[1] + "/iso/part/main/tmd.bin", "rb").read()

tmd = wii.WiiTmd(stmd)
tmd.showinfo()

print "Waiting for IPC to start up..."
ipc = SkyeyeIPC()
ipc.init()
print "IPC ready"

fd = ipc.IOSOpen("/dev/es")
print "ES fd: %d" % fd
if fd < 0:
	print "Error opening ES"
	sys.exit(1)

keyid = ipc.makebuf(4)
hashes = ipc.makebuf(tmd.num_contents * 20)
res = ipc.IOSIoctlv(fd, 0x1c, "dddd:dd", certificates, None, sticket, stmd, keyid, hashes)
if res < 0:
	print "Error %d" % res
	keyid.free()
	hashes.free()
	ipc.IOSClose(fd)
	sys.exit(1)

keyid.free()
hashes.free()

ipc.IOSClose(fd)

