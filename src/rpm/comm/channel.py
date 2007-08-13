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

import struct
import rpm.comm.protocol

class ChannelFactory:
    def getChannels(self):
        """Returns a list of objects of type rpm.comm.Channel that can be used
        to initiate communication with a target."""
        raise NotImplementedError

class Channel:
    lastSequenceNumber = 0
    def connect(self):
        """Establish connection and start communicating with the target.
        A thread may be spawned internally."""
        raise NotImplementedError

    def addMessageHandler(self, handler):
        """Add a handler object which method fireIncomingMessage() is called
        with a message object as reception of one is finished. The handler
        object should be an instance of class MessageReceiver.
        All registered handlers are called in an unspecified order."""
        raise NotImplementedError

    def connect(self):
        """Creates a PyMite-instance as a subprocess and attaches to it."""
        raise NotImplementedError

    def _transmitBytes(self, messageBytes):
        raise NotImplementedError

    def quit(self):
        raise NotImplementedError

    def sendMessage(self, message):
        """Takes a rpm.comm.messages.Message object to deliver the data,
        wraps it into a complete message and sends it down the channel.
        Uses implementation specific _transmitBytes(messageBytes)."""
        self.lastSequenceNumber += 1
        self.lastSequenceNumber %= 2**8
        data = message.toBytes()
        import pdb
        #pdb.set_trace()
        messageBytes = struct.pack("cBBB", rpm.comm.protocol.START_SYMBOL,
                                   self.lastSequenceNumber, message.type,
                                   len(data))
        messageBytes += data

        # Calculate checksum
        checksum = rpm.comm.protocol.crc16_calculate(messageBytes)
        messageBytes += struct.pack("<H", checksum)
        self._transmitBytes(messageBytes)

class MessageReceiver:
    def fireIncomingMessage(self, message):
        raise NotImplementedError