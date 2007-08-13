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

import rpm.comm.messages
from rpm.comm.messages import Message, DefectiveMessage
from rpm.exceptions import *
import struct
import rpm.util

START_SYMBOL = '\xFF'

RPP_OFFSETS = {
               "SEQUENCENUMBER": 1,
               "TYPE": 2,
               "DATALENGTH": 3,
               "DATASTART": 4,
               }

class Decoder:
    """Utility class that will handle incoming bytes that should contain RPP
    data from a target. It is incrementally called with new data that is
    buffered internally. If the new data yields a complete message, that is
    returned. If more than one message is available, the next call will return
    the next one."""
    buffer = ""
    lastSequenceNumber = None
    # Is set to the type number after the first segment of an overlong message
    # is received. Set back to None after the last segment is received and
    # the complete message is delivered.
    olMessageType = None
    olMessageData = ""

    def removeFirstMessage(self):
        """Removes as much data from self.buffer as belongs to the first
        complete message in it."""
        self.buffer = \
            self.buffer[6+ord(self.buffer[RPP_OFFSETS["DATALENGTH"]]):]

    def decode(self, data, bufferSize):
        """Can be called with or without data. If the buffered plus the new data
        contain a complete message, an instance of a subclass of type
        rpm.comm.protocol.MessageType is returned. If the message contains
        a corrupt checksum, the type is DefectiveMessage. Returns None
        otherwise.
        bufferSize Is our receiving buffer size that is advertised in the sync
                   message. At the moment the target ignores this and uses its
                   own receive buffer size as send buffer size, so we
                   artificially use the target's receive buffer size from the
                   sync message as bufferSize."""

        # TODO Ensure that new data received after a long time is handled as independent data.
        self.buffer += data
        while len(self.buffer) > 0:
            # ensure buffer starts with START_SYMBOL or is empty
            try:
                self.buffer = self.buffer[self.buffer.index(START_SYMBOL):]
            except ValueError:
                # No start symbol in buffer data
                self.buffer = ""
                return None
            if len(self.buffer) > 1 and self.lastSequenceNumber:
                # Check sequence number
                if self.buffer[RPP_OFFSETS["SEQUENCENUMBER"]] != self.lastSequenceNumber+1:
                    # TODO Handle incorrect sequence number
                    pass
            if len(self.buffer) > 2:
                # Check if type is valid
                type = ord(self.buffer[RPP_OFFSETS["TYPE"]])
                try:
                    rpm.comm.messages.messageByType[type]
                except KeyError:
                    # The type is known
                    self.buffer = ""
                    return None
            if len(self.buffer) >= 6:
                dataLength = ord(self.buffer[RPP_OFFSETS["DATALENGTH"]])
                # Message could be complete
                if len(self.buffer) < 6+dataLength:
                    # Not yet
                    return None
                # Calculate and compare checksum
                checksum = crc16_calculate(self.buffer[0:4+dataLength])
                crc = self.buffer[4+dataLength:][0:2]
                if checksum != struct.unpack("<H", crc)[0]:
                    # Remove first message but don't trust message contents
                    self.buffer = self.buffer[self.buffer.index(START_SYMBOL):]
                    return DefectiveMessage()
                # Construct message object
                if self.olMessageType != None:
                    # This should be another segment of an overlong message
                    if self.olMessageType != ord(self.buffer[RPP_OFFSETS["TYPE"]]):
                        print "Overlong message type %i was not completed with"\
                            " this data waiting: %s" % (self.olMessageType,
                                                        self.olMessageData)
                        self.olMessageType = None
                        self.olMessageData = ""
                        # Fall through to normal message processing
                    else:
                        # Another segment. Store data.
                        self.olMessageData += self.buffer[RPP_OFFSETS["DATASTART"]:RPP_OFFSETS["DATASTART"]+dataLength]
                        if dataLength != bufferSize:
                            # Last segment.
                            message = Message().overlongMessage(
                                        self.olMessageType,
                                        self.olMessageData)
                            self.olMessageType = None
                            self.olMessageData = ""
                            self.removeFirstMessage()
                            return message
                        else:
                            # Neither first nor last segment.
                            self.removeFirstMessage()
                            return None
                else:
                    if dataLength == bufferSize:
                    # First overlong message segment
                        self.olMessageType = ord(self.buffer[RPP_OFFSETS["TYPE"]])
                        self.olMessageData += self.buffer[RPP_OFFSETS["DATASTART"]:RPP_OFFSETS["DATASTART"]+dataLength]
                        self.removeFirstMessage()
                        return None

                # Plain old normal message
                message = Message().fromBytes(self.buffer)

                self.removeFirstMessage()
                return message
            return None

def crc16_update(crc, data):
    crc ^= ord(data)
    for i in range(0,8):
        if crc & 1:
            crc = (crc >> 1) ^ 0xA001
        else:
            crc = (crc >> 1)
    return crc

def crc16_calculate(data):
    """Calculate a CRC16 with polynomial 0xA001 and an initializer of 0xFFFF."""
    checksum = 0xFFFF
    for c in data:
        checksum = crc16_update(checksum, c)
    return checksum