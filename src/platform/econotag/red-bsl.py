#!/usr/bin/env python

# This file was obtained from: http://bmi.berkeley.edu/~pullin/red-bsl.py


import serial
from struct import *
import os
import sys
import time
from optparse import OptionParser


parser = OptionParser()
parser.add_option("-f", "--file", dest="filename",
                  help="binary file to program", metavar="FILE")
parser.add_option("-t", "--target", dest="target",
                  help="serial device to send to, default /dev/ttyUSB0", default="/dev/ttyUSB0",
		  metavar="TARGET")
parser.add_option("-s", "--flash", dest="flashprog", action="store_true",
                  help="write program to flash, default writes to RAM", default=False,
		  metavar="FLASHPROG")
parser.add_option("-b", "--baudrate", dest="baudrate",
                  help="baudrate for serial device, default 115200", default=115200,
		  metavar="BAUDRATE")

(options, args) = parser.parse_args()

ZEROCHAR = chr(0)

if options.filename == None:
	print "No binary file specified."
	parser.print_help()
	sys.exit(0)

ser = serial.Serial(port=options.target,baudrate=options.baudrate,timeout=0.25,rtscts=1)
if ser.isOpen():
	ser.setRTS()

connected = 0
print "Press RESET now..."

while True:
	ser.write(ZEROCHAR)
	response = ser.read(100)
	if response == "\x00CONNECT" or response=="CONNECT":
		print "Connected!"
		break

if options.flashprog:
	infile = open("../tools/flasher.bin")
	filesize = os.path.getsize("../tools/flasher.bin")
	print "Sending flasher... (%dB)" % filesize
else:
	infile = open(options.filename)
	filesize = os.path.getsize(options.filename)
	print "Sending binary file... (%dB)" % filesize


#ser.write(chr(filesize & 255))
#ser.write(chr((filesize >> 8)&255))
#ser.write(chr((filesize >> 16)&255))
#ser.write(chr((filesize >> 24)&255))
fileSizeBytes = pack('I',filesize)
ser.write(fileSizeBytes)


#bytes = [i for i in infile.read()]
bytes = infile.read()

starttime = time.time()
#ser.write(bytes)
for byte in bytes:
	ser.write(byte)
endtime = time.time()

print "Speed: %.2f KBps" % (filesize/(endtime-starttime)/1000)

if options.flashprog:
	print "Waiting for flasher to start..."
	for tries in range(100):
		time.sleep(0.010)
		if tries==99:
			print "No response from flasher utility. Erase flash with jumpers and try again."
			sys.exit(0)
		if ser.readline().strip() == "ready":
			break

	flashfile = open(options.filename)
	filesize = os.path.getsize(options.filename)
	print "Writing to flash... (%dB)" % filesize
	flashbytes = [i for i in flashfile.read()]
	ser.write(chr(filesize & 255))
	ser.write(chr(filesize >> 8))
	ser.write(chr(filesize >> 16))
	ser.write(chr(filesize >> 24))
	starttime = time.time()
	for byte in flashbytes:
		ser.write(byte)
		time.sleep(0.0005)
	endtime = time.time()
	print "Speed: %.2f KBps" % (filesize/(endtime-starttime)/1000)
	
print "Done."

ser.close()