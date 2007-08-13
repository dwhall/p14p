#!/usr/bin/env python
#
# RPM/RPP: A PyMite component
# Copyright 2007 Philipp Adelt
# Parts based on sample/ipm-desktop/ipm.py: Copyright 2002 Dean Hall
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

import subprocess
import threading
import rpm.comm.channel
import rpm.comm.protocol
import rpm.exceptions
import os, signal, sys
import pdb

PMVM_EXE = "sample/rpm-desktop/main.out"

class PipeFactory(rpm.comm.channel.ChannelFactory):
    """Initially reports one channel. When opened, a subprocess
    is generated with a PyMite instance. Additional instances can
    be created via createChannel()."""
    def getChannels(self):
        return [PipeChannel()]

class PipeChannel(rpm.comm.channel.Channel):
    readerThread = None
    messageHandlers = []
    child = None

    def __init__(self, target=PMVM_EXE):
        self.target = target

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
        self.child = subprocess.Popen(self.target,
                                      bufsize=-1,
                                      stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=None,
                                      )
        self.readerThread = ReaderThread(self.child.stdout)
        for handler in self.messageHandlers:
            self.readerThread.registerMessageHandler(handler)
        self.readerThread.start()

    def quit(self):
        self.readerThread.running = False
        try:
            os.kill(self.child.pid, signal.SIGKILL)
        except AttributeError:
            raise NotImplementedError("""You have now found out that:
                a) Python does not support killing a process under Win32.
                b) RPM does not implement a workaround and learning from this
                message:
                c) RPM does not support Win32-native environments.
                Using Cygwin's Python solves this. Workaround:
                Please kill the process with PID %i yourself."""
                % self.child.pid)

    def _transmitBytes(self, data):
        self.child.stdin.write(data)
        self.child.stdin.flush()
        sys.stderr.write("Sending to child: ")
        sys.stderr.write(rpm.util.hexdump(data))
        sys.stderr.write("\n")

    def sendMessage(self, message):
        rpm.comm.channel.Channel.sendMessage(self, message)

class ReaderThread(threading.Thread):
    running = True
    messageHandlers = []
    def __init__(self, stdout):
        threading.Thread.__init__(self)
        self.stdout = stdout
        self.decoder = rpm.comm.protocol.Decoder()
        self.handlerLock = threading.Lock()

    def registerMessageHandler(self, handler):
        self.handlerLock.acquire()
        self.messageHandlers.append(handler)
        self.handlerLock.release()

    def run(self):
        while self.running:
            datain = self.stdout.read(1)
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
