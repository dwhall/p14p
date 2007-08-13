#!/usr/bin/env python
#
# RPM/RPP: A PyMite component
# Copyright 2007 Philipp Adelt
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

"""
=============
Remote PyMite
=============

Log
---

==========      ================================================================
Date            Action
==========      ================================================================
2007/07/13      Initial creation
"""

import serial
import rpm.comm.channel
import threading
import sys

class SerialFactory(rpm.comm.channel.ChannelFactory):
    """Pyserial does not allow ports to be enumerated, therefore the channel
    has to be opened directly."""
    def getChannels(self):
        return []

class SerialChannel(rpm.comm.channel.Channel):
    readerThread = None
    messageHandlers = []
    child = None
    BAUD=19200

    def __init__(self, portName=0):
        self.portName = portName

    def __str__(self):
        if self.child != None:
            connection = "open"
        else:
            connection = "closed"
        return "<PipeChannel, Status: %s>" % connection

    def addMessageHandler(self, handler):
        """Add a function that is called with a message object as completion
        of one is finished. All registered handlers are called in an
        unspecified order."""
        self.messageHandlers.append(handler)
        if self.child != None:
            self.child.registerMessageHandler(handler)

    def connect(self):
        """Creates a PyMite-instance as a subprocess and attaches to it."""
        print "Port: %s" % self.portName
        self.ser = serial.Serial(self.portName,
                                 self.BAUD, rtscts=0)
        self.readerThread = ReaderThread(self.ser)
        for handler in self.messageHandlers:
            self.readerThread.registerMessageHandler(handler)
        self.readerThread.start()

    def quit(self):
        self.readerThread.running = False
        self.ser.close()

    def _transmitBytes(self, data):
        self.ser.write(data)
        sys.stderr.write("Sending to serial port: ")
        sys.stderr.write(rpm.util.hexdump(data))
        sys.stderr.write("\n")

    def sendMessage(self, message):
        rpm.comm.channel.Channel.sendMessage(self, message)

class ReaderThread(threading.Thread):
    running = True
    messageHandlers = []
    def __init__(self, ser):
        threading.Thread.__init__(self)
        self.ser = ser
        self.decoder = rpm.comm.protocol.Decoder()
        self.handlerLock = threading.Lock()

    def registerMessageHandler(self, handler):
        self.handlerLock.acquire()
        self.messageHandlers.append(handler)
        self.handlerLock.release()

    def run(self):
        while self.running:
            datain = self.ser.read(1)
            if len(datain) == 0:
                if not self.running:
                    # User instructed exit.
                    return
                raise rpm.exceptions.ChannelBroken("Pipe channel connection "
                                                   "unexpectedly closed.")
            # TODO Fixed number is to be replaced. At the moment, this needs
            # to equal the target's send buffer size to allow detection of
            # overlong messages.
            message = self.decoder.decode(datain, 50)
            if message != None:
                self.handlerLock.acquire()
                myHandlers = self.messageHandlers[:]
                self.handlerLock.release()
                for handler in myHandlers:
                    handler.fireIncomingMessage(message)
