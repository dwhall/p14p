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
These message classes should correspond to the types in enum PmRppType_e
(src/vm/rpp.h).

Log
---

==========      ================================================================
Date            Action
==========      ================================================================
2007/07/13      Initial creation
"""
import rpm.comm.protocol
import rpm.exceptions
import struct
import platform

class Message:
    type = 0
    parameters = {}
    def __setitem__(self, key, value):
        self.parameters[key] = value

    def __getitem__(self, key):
        return self.parameters[key]

    def getNumericalType(self):
        return self.type

    def fromBytes(self, messageData):
        """When passed a complete message string including start symbol and
        checksum, constructs an appropriate message object of the correct
        type. Won't check message data integrity."""
        # Dispatch to corresponding message class
        RPP_OFFSETS = rpm.comm.protocol.RPP_OFFSETS
        try:
            message = messageByType[ord(messageData[RPP_OFFSETS["TYPE"]])]()
        except KeyError:
            raise rpm.exceptions.UnknownMessageType("Message type is %i" %
                                     ord(messageData[RPP_OFFSETS["TYPE"]]))
        # Send the data section
        message.fromBytes(messageData[RPP_OFFSETS["DATASTART"]:-2])
        return message

    def overlongMessage(self, messageType, data):
        """Constructs an overlong message from messageType with a long data
        section."""
        try:
            message = messageByType[messageType]()
        except KeyError:
            raise rpm.exceptions.UnknownMessageType("Message type is %i" %
                                     ord(messageData[RPP_OFFSETS["TYPE"]]))
        # Send the data section
        message.fromBytes(data)
        return message

    def toBytes(self):
        """Construct the data section of a message from the (optional)
        parameters that a message carries."""
        raise NotImplementedError

    def __str__(self):
        return "<%s, Type=%i>" % (self.__class__, self.type)

class InvalidMessage(Message):
    type = 0
    def fromBytes(self, messageData):
        raise NotImplementedError

class DefectiveMessage(Message):
    """Used rpm-internally to signal a message with corrupt data."""
    type = -1
    def fromBytes(self, messageData):
        raise NotImplementedError

class AckMessage(Message):
    type = 1
    def fromBytes(self, messageData):
        """AckMessage takes no data."""
        pass
    def toBytes(self):
        """AckMessage has no data."""
        return ""

class NackMessage(Message):
    type = 2
    def fromBytes(self, messageData):
        """NackMessage takes no data."""
        pass
    def toBytes(self):
        """AckMessage has no data."""
        return ""

class SyncMessage(Message):
    type = 3
    format = "BBB16s"
    parameters = {"RESET": False,
                  "RECEIVE_BUFFER_SIZE": 0,
                  "WORD_SIZE": 0,
                  "ID_STRING": "RPM on %s" % platform.python_version()}

    def __init__(self, **vargs):
        for (k,v) in vargs.items():
            self.parameters[k] = v

    def fromBytes(self, data):
        if len(data) < 19:
            raise Exception("Message is to short to be a SyncMessage.")
        (self["RESET"],
         self["RECEIVE_BUFFER_SIZE"],
         self["WORD_SIZE"],
         self["ID_STRING"]) = struct.unpack(self.format, data)

    def toBytes(self):
        return struct.pack(self.format, self["RESET"],
         self["RECEIVE_BUFFER_SIZE"],
         self["WORD_SIZE"],
         self["ID_STRING"])

class ThreadPrintItemMessage(Message):
    type = 4
    data = ""
    def fromBytes(self, messageData):
        self.data = messageData
    def toBytes(self):
        raise NotImplementedError
    def getThreadAddress(self, wordSize):
        result = 0
        for i in range(0, wordSize):
            result += ord(self.data[i])*(8**i)
        return result
    def getData(self, wordSize):
        return self.data[wordSize:]

class ThreadPrintExprMessage(ThreadPrintItemMessage):
    type = 5
    data = ""
    def toBytes(self):
        raise NotImplementedError

class ThreadListMessage(Message):
    type = 6
    data = ""
    def fromBytes(self, messageData):
        self.data = messageData
    def toBytes(self):
        return ""
    def getThreadList(self, wordSize):
        """Message data should contain a list of memory addresses, each wordSize
        bytes long."""
        if len(self.data) % wordSize != 0:
            raise rpm.exceptions.InvalidData("Thread list should hold complete words.")
        threadList = []
        for i in range(0,len(self.data)/wordSize):
            address = 0
            for b in range(0,wordSize):
                address *= 2**8
                address += ord(self.data[i*wordSize+b])
            threadList.append(address)
        return threadList

class MemListMessage(Message):
    type = 7
    data = ""
    def fromBytes(self, messageData):
        self.data = messageData
    def toBytes(self):
        return ""
    def getMemList(self):
        return eval(self.data)

class MemAddBlockMessage(Message):
    type = 8
    data = None
    memspace = None
    def __init__(self, memspace, blockData):
        self.data = blockData
        self.memspace = memspace
    def fromBytes(self, messageData):
        raise NotImplementedError
    def toBytes(self):
        return chr(self.memspace)+self.data

class AutorunListMessage(Message):
    type = 9
    data = ""
    def __init__(self, data=""):
        self.data = data
    def fromBytes(self, messageData):
        self.data = messageData
    def toBytes(self):
        return self.data
    def getAutorunList(self):
        """Message data should contain a list of memory addresses, each wordSize
        bytes long."""
        if len(self.data) == 0:
            return []
        else:
            return self.data.split(",")

class ThreadStartMessage(Message):
    type = 10
    data = ""
    def __init__(self, data):
        self.data = data
    def fromBytes(self, messageData):
        raise NotImplementedError
    def toBytes(self):
        return self.data

# Gather the message types into a hash table
messageByType = {}
myLocals = locals().copy()
for (key,val) in myLocals.items():
    try:
        if val != Message and issubclass (val,Message):
            messageByType[val.type] = val
    except TypeError:
        # Will happen on non-class-objects
        pass
del(myLocals)
