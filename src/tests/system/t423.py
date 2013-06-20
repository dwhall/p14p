# This file is Copyright 2013 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
#
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING up one directory from this.

#
# System Test 423
# Test Slice1, 2 and 3 over built-in sequence types
#

# Create sequences
l = range(5)
s = "Some string"
t = (2,5,6)

# SLICE_1 Tests
assert l[0:] == range(5)
assert l[2:] == [2,3,4]
assert l[5:] == []
assert l[100:] == []
assert l[-3:] == [2,3,4]
assert l[-42:] == range(5)

assert s[0:] == "Some string"
assert s[5:] == "string"
assert s[len(s):] == ""
assert s[100:] == ""
assert s[-3:] == "ing"
assert s[-999:] == "Some string"

assert t[0:] == (2,5,6)
assert t[2:] == (6,)
assert t[len(t):] == ()
assert t[100:] == ()
assert t[-3:] == (2,5,6)
assert t[-9944:] == (2,5,6)

# SLICE_2 Tests
assert l[:0] == []
assert l[:2] == [0,1]
assert l[:5] == [0,1,2,3,4]
assert l[:100] == [0,1,2,3,4]
assert l[:-3] == [0,1]
assert l[:-42] == []

assert s[:0] == ""
assert s[:5] == "Some "
assert s[:len(s)] == "Some string"
assert s[:100] == "Some string"
assert s[:-3] == "Some str"
assert s[:-999] == ""

assert t[:0] == ()
assert t[:2] == (2,5)
assert t[:len(t)] == (2,5,6)
assert t[:100] == (2,5,6)
assert t[:-3] == ()
assert t[:-9944] == ()

# SLICE_3 Tests
assert l[0:0] == []
assert l[0:2] == [0,1]
assert l[0:5] == [0,1,2,3,4]
assert l[0:100] == [0,1,2,3,4]
assert l[0:-3] == [0,1]
assert l[0:-42] == []

assert l[2:0] == []
assert l[2:2] == []
assert l[2:5] == [2,3,4]
assert l[2:100] == [2,3,4]
assert l[2:-3] == []
assert l[2:-42] == []

assert l[5:0] == []
assert l[5:2] == []
assert l[5:5] == []
assert l[5:100] == []
assert l[5:-3] == []
assert l[5:-42] == []

assert l[100:0] == []
assert l[100:2] == []
assert l[100:5] == []
assert l[100:100] == []
assert l[100:-3] == []
assert l[100:-42] == []

assert l[-3:0] == []
assert l[-3:2] == []
assert l[-3:5] == [2,3,4]
assert l[-3:100] == [2,3,4]
assert l[-3:-3] == []
assert l[-3:-42] == []

assert l[-43:0] == []
assert l[-43:2] == [0,1]
assert l[-43:5] == [0,1,2,3,4]
assert l[-43:100] == [0,1,2,3,4]
assert l[-43:-3] == [0,1]
assert l[-43:-42] == []

#

assert s[0:0] == ""
assert s[0:5] == "Some "
assert s[0:len(s)] == "Some string"
assert s[0:100] == "Some string"
assert s[0:-3] == "Some str"
assert s[0:-999] == ""

assert s[5:0] == ""
assert s[5:5] == ""
assert s[5:len(s)] == "string"
assert s[5:100] == "string"
assert s[5:-3] == "str"
assert s[5:-999] == ""

assert s[11:0] == ""
assert s[11:5] == ""
assert s[11:len(s)] == ""
assert s[11:100] == ""
assert s[11:-3] == ""
assert s[11:-999] == ""

assert s[-3:0] == ""
assert s[-3:5] == ""
assert s[-3:len(s)] == "ing"
assert s[-3:100] == "ing"
assert s[-3:-3] == ""
assert s[-3:-999] == ""

assert s[-99:0] == ""
assert s[-99:5] == "Some "
assert s[-99:len(s)] == "Some string"
assert s[-99:100] == "Some string"
assert s[-99:-3] == "Some str"
assert s[-99:-999] == ""

#

assert t[0:0] == ()
assert t[0:2] == (2,5)
assert t[0:len(t)] == (2,5,6)
assert t[0:100] == (2,5,6)
assert t[0:-3] == ()
assert t[0:-9944] == ()

assert t[2:0] == ()
assert t[2:2] == ()
assert t[2:len(t)] == (6,)
assert t[2:100] == (6,)
assert t[2:-3] == ()
assert t[2:-9944] == ()

assert t[3:0] == ()
assert t[3:2] == ()
assert t[3:len(t)] == ()
assert t[3:100] == ()
assert t[3:-3] == ()
assert t[3:-9944] == ()

assert t[-3:0] == ()
assert t[-3:2] == (2,5)
assert t[-3:len(t)] == (2,5,6)
assert t[-3:100] == (2,5,6)
assert t[-3:-3] == ()
assert t[-3:-9944] == ()

assert t[-9944:0] == ()
assert t[-9944:2] == (2,5)
assert t[-9944:len(t)] == (2,5,6)
assert t[-9944:100] == (2,5,6)
assert t[-9944:-3] == ()
assert t[-9944:-9944] == ()
