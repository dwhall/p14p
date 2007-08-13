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

import rpm
import rpm.comm
import rpm.handler
import sys, time
import rpm.comm.messages
import pdb

delay = 0.2
# Get a channel
pymite1 = rpm.handler.PyMite(rpm.comm.pipe.PipeFactory().getChannels()[0])
print "1"
#pdb.set_trace()
time.sleep(delay)
pymite1.synchronize()
time.sleep(delay)
print "pymite params: " + str(pymite1.pymiteParameters)
pymite1.readThreadList()
print "List of Threads: " + str(pymite1.threadList)
pymite1.readMemList()
print "List of memory spaces: "
for (num, params) in pymite1.memList.items():
    print "#%02i: %-15s Size: %8i byte, writeable: %i" % (num,
         params[0], params[1], params[2])
time.sleep(delay)
pymite1.addModule("mod1", 'print "Hello mod1!"')
time.sleep(delay)
pymite1.readAutorunList()
print "Autorun list: %s" % pymite1.autorunList
newList = pymite1.autorunList[:]
print "Sending new list: %s" % newList
newList.append("neu")
time.sleep(delay)
pymite1.sendAutorunList(newList)
time.sleep(delay)
pymite1.readAutorunList()
print "Autorun list: %s" % pymite1.autorunList
time.sleep(delay)
pymite1.sendAutorunList([])
time.sleep(delay)
pymite1.startThread("mod1")
time.sleep(delay)
pymite1.readAutorunList()
print "Autorun list: %s" % pymite1.autorunList
time.sleep(delay)
time.sleep(delay)
time.sleep(delay)

pymite1.terminate()
