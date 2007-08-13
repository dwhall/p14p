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

from rpm.comm.messages import *
import rpm.comm.channel
import rpm.exceptions
import time
import tools.pmImgCreator
import logging


class IncomingMessageReceiver(rpm.comm.channel.MessageReceiver):
    threadPrintBuffer = ""
    
    def __init__(self, pymite):
        self.pymite = pymite

    def fireIncomingMessage(self, message):
        logging.debug("Incoming message: %s" % message)
        if not isinstance(message, Message):
            raise Exception("Incoming message not appropriate. type=%s" \
                            % type(message))

        if isinstance(message, SyncMessage):
            self.pymite.pymiteParameters = message.parameters.copy()
            logging.debug("New PyMite parameters: "+str(self.pymite.pymiteParameters))
            if self.pymite.pymiteParameters["RESET"] == 1:
                # TODO Handle a reset target (invalidate PyMite status data).
                self.pymite.send(AckMessage())
            self.pymite.synchronizedIncoming = True

        if isinstance(message, AckMessage):
            if isinstance(self.pymite.lastMessage, SyncMessage):
                # Received the confirmation for a SyncMessage -> synchronized.
                self.pymite.synchronizedOutgoing = True
                logging.debug("Our sync message was received and acknowledged.")
            # Signal the message was successfully sent
            self.pymite.lastMessage = None

        if message.__class__ in [ThreadPrintItemMessage, ThreadPrintExprMessage]:
            # TODO Enforce synchronization before interpreation to get rid
            # of this ugly hack.
            if not self.pymite.pymiteParameters:
                wordSize = 4
            else:
                wordSize = self.pymite.pymiteParameters["WORD_SIZE"]
            logging.debug("Thread 0x%X sent data '%s'" % 
                (message.getThreadAddress(wordSize),
                 rpm.util.hexdump(message.getData(wordSize))) )
            self.threadPrintBuffer += message.getData(wordSize)
            linelength = self.threadPrintBuffer.find('\x0A')
            if linelength > -1:
                logging.info("Thread 0x%X: %s" % 
                    (message.getThreadAddress(wordSize),
                    self.threadPrintBuffer[:linelength]))
                self.threadPrintBuffer = self.threadPrintBuffer[linelength+1:]

        if isinstance(message, ThreadListMessage):
            self.pymite.threadList = message.getThreadList(
               self.pymite.pymiteParameters["WORD_SIZE"])
            self.pymite.send(AckMessage())
            # Signal the message was successfully sent
            # Need to check if we requested this message as target is free to
            # send a ThreadList at any time.
            if isinstance(self.pymite.lastMessage, ThreadListMessage):
                self.pymite.lastMessage = None

        if isinstance(message, MemListMessage):
            try:
                self.pymite.memList = message.getMemList()
                self.pymite.lastMessage = None
            except SyntaxError, e:
                # Happens if the data is somehow bad.
                logging.warn("SyntaxError while parsing memory spaces list: %s" % e)
                logging.warn("Data was: %s" % message.data)
            self.pymite.send(AckMessage())

        if isinstance(message, AutorunListMessage):
            self.pymite.autorunList = message.getAutorunList()
            self.pymite.lastMessage = None

class PyMite:
    channel = None
    pymiteParameters = {}
    # Did we receive a sync message from the target?
    synchronizedIncoming = False
    # Did we already send a sync message with reset=1 and received an answer?
    synchronizedOutgoing = False
    # Timeout in milliseconds. After this time without an answer a message is
    # considered lost.
    timeoutMs = 3000
    # Number of times a message is sent and no answer is received within
    # timeoutMs before a fatal connection problem is assumed.
    commRetries = 3
    lastMessage = None
    threadList = None
    # Dict with memspace number as key and a tuple of parameters as value.
    memList = None
    # List of strings that denote one code image name each
    autorunList = None

    def __init__(self, channel):
        self.channel = channel
        self.channel.addMessageHandler(IncomingMessageReceiver(self))
        self.channel.connect()

    def terminate(self):
        if self.channel:
            self.channel.quit()

    def send(self, message):
        # Before a normal message can be sent, enforce synchronization
        if not self.synchronizedOutgoing \
            and not message.__class__ in (AckMessage, SyncMessage):
            self.synchronize()

        # Wait for last message to be acknowledged (signalled by setting
        # lastMessage to None) before sending new message.
        if self.lastMessage != None:
            tries = 1
            while tries <= self.commRetries:
                # Resend on second and following tries
                if tries > 1:
                    logging.warn("Resending message %s" % self.lastMessage)
                    self.channel.sendMessage(self.lastMessage)
                    self.lastMessageTimestamp = time.time()
                while not self.lastMessage == None \
                    and (time.time()-self.lastMessageTimestamp)*1000 < self.timeoutMs:
                    time.sleep(0.05)
                if self.lastMessage == None:
                    break
                tries += 1
            if self.lastMessage:
                # Sending did not yield an answer
                raise rpm.exceptions.CommunicationTimeout("Sending message %s " \
                     "%i times did not result in an answer.""" % \
                     (self.lastMessage, self.commRetries))
        if not message.__class__ in [AckMessage,NackMessage,ThreadListMessage]:
            self.lastMessage = message
            self.lastMessageTimestamp = time.time()
        # TODO Handle overlong messages
        self.channel.sendMessage(message)

    def synchronize(self):
        """Does a blocking synchronization with the target by sending a
        synchronization message until we receive acknowledgment. Also block
        until the partner sends its synchronization message."""
        tries = 1
        while tries <= self.commRetries:
            self.send(SyncMessage(RESET=not self.synchronizedOutgoing))
            startTime = time.time()
            while not self.synchronizedOutgoing \
                and not self.synchronizedIncoming \
                and (time.time()-startTime)*1000 < self.timeoutMs:
                time.sleep(0.05)
            if self.synchronizedOutgoing and self.synchronizedIncoming:
                break
            tries += 1
        if not self.synchronizedOutgoing:
            raise rpm.exceptions.CommunicationTimeout("Synchronization failed.")

    def readThreadList(self):
        """Does a blocking request for a new thread list."""
        tries = 1
        self.threadList = None
        while tries <= self.commRetries:
            self.send(ThreadListMessage())
            startTime = time.time()
            while self.threadList == None \
                and (time.time()-startTime)*1000 < self.timeoutMs:
                time.sleep(0.05)
            if self.threadList != None:
                break
            tries += 1
            logging.debug("threadlist: %s" % self.threadList)
        if self.threadList == None:
            raise rpm.exceptions.CommunicationTimeout("Getting thread list failed.")

    def readMemList(self):
        """Does a blocking request for a new memory space list."""
        tries = 1
        self.memList = None
        while tries <= self.commRetries:
            self.send(MemListMessage())
            startTime = time.time()
            while self.memList == None \
                and (time.time()-startTime)*1000 < self.timeoutMs:
                time.sleep(0.05)
            if self.memList  != None:
                break
            tries += 1
        if self.memList  == None:
            raise rpm.exceptions.CommunicationTimeout("Getting memory space list failed.")

    def readAutorunList(self):
        """Does a blocking request for a new autorun list."""
        tries = 1
        self.autorunList = None
        while tries <= self.commRetries:
            self.send(AutorunListMessage())
            startTime = time.time()
            while self.autorunList == None \
                and (time.time()-startTime)*1000 < self.timeoutMs:
                time.sleep(0.05)
            if self.autorunList != None:
                break
            tries += 1
        if self.autorunList == None:
            raise rpm.exceptions.CommunicationTimeout("Getting autorun list failed.")

    def sendAutorunList(self, list):
        """Formats the list to a string and sends it for storage on the
        target."""
        if len(list) == 0:
            m = AutorunListMessage(data="\x00")
        else:
            m = AutorunListMessage(data=",".join(list))
        self.send(m)

    def startThread(self, moduleName):
        """Start a new thread on the target from a module."""
        self.send(ThreadStartMessage(moduleName))

    def addBlock(self, memspace, data):
        """Append data to the end of memspace."""
        self.send(MemAddBlockMessage(memspace, data))

    def addModule(self, modname, sourceCode):
        """Add a module with the given name that contains the given
        sourceCode."""
        pic = tools.pmImgCreator.PmImgCreator()
        pic.set_options(None, "bin", "usr", None, None, None)
        pic.convert_single_function(modname, sourceCode)
        if len(pic.nativemods) > 0:
            raise Exception("Native functions in a runtime-transferred module"+
                            " are not supported.")
        image = pic.format_img_as_bin()
        logging.debug("PyImgCreator delivered the following: %s (%i bytes)" % \
            (rpm.util.hexdump(image), len(image)))
        self.addBlock(7, image)
