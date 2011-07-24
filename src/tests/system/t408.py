# This file is Copyright 2010 Dean Hall.
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
# System Test 408
#

from pmMarshal import *


# Test dumps() for None
print "dumps(None) = ", dumps(None)
assert dumps(None) == "N"

# Test dumps() for bool
assert dumps(True) == "T"
assert dumps(False) == "F"

# Test dumps() for ints
assert dumps(0) == "0"
assert dumps(1) == "1"
assert dumps(9) == "9"
assert dumps(-1) == "B\xFF"
assert dumps(42) == "B*"
assert dumps(500) == "H\xf4\x01"
print "dumps(30000) =", dumps(30000)
assert dumps(30000) == "H0u"
print "dumps(61374) =", dumps(61374)
assert dumps(61374) == "I\xbe\xef\x00\x00"
print "dumps(8675309) =", dumps(8675309)
assert dumps(8675309) == "I\xed_\x84\x00"
print "dumps(42) = %s" % dumps(42)

# Test dumps() for str
assert dumps("") == "s\x00"
assert dumps("Hello") == "s\x05Hello"

# Test dumps() for tuple
t = ()
assert dumps(t) == "(\x00"
print dumps((1,2,3,))
assert dumps((1,2,3,)) == "(\x03123"
assert dumps((10,20,30,)) == "(\x03B\x0AB\x14B\x1E"
assert dumps((100,)) == "(\x01B\x64"
assert dumps(300) == "H\x2c\x01"
assert dumps((100,200,"")) == "(\x03B\x64H\xc8\x00s\x00"
assert dumps((100,200,1)) == "(\x03B\x64H\xc8\x001"
assert dumps((100,200,None)) == "(\x03B\x64H\xc8\x00N"
assert dumps((100,200,False)) == "(\x03B\x64H\xc8\x00F"
assert dumps(("Test",)) == "(\x01s\x04Test"
assert dumps((100,200,"Test")) == "(\x03B\x64H\xc8\x00s\x04Test"
assert dumps((100,200,"Hello")) == "(\x03B\x64H\xc8\x00s\x05Hello"
assert dumps((100,200,100)) == "(\x03B\x64H\xc8\x00B\x64"
assert dumps((100,200,200)) == "(\x03B\x64H\xc8\x00H\xc8\x00"

# Test dumps() for code object
def fim():
    print "in fim"
c = dumps(fim.__code__)
print "dumps(fim.__code__)=", c
assert c[0] == "C" and len(c) >= 18 # the minimum size of a dumped code obj


# Test loads()

objs = (-222222, -22222, -222, -2, -1, 0, 1, 2, 22, 222, 22222, 222222,
    0.0, 3.14, -6.02E24,
    None, True, False, "", (), [],
    {},
    "foo", "\x00bar\x00",
    ("foo", 222222, {}, True, 3.14, ("bar",)),
    ["foo", 222222, {}, True, 3.14, ("bar",)],
    ["foo", 222222, {}, True, 3.14, ["bar",]],
    {0:0, None:None, 0:{"Nil":0}, '1':1111111, "pi-ish":3.14, "foo":"bar"}
    )

for o in objs:
    print o, dumps(o), loads(dumps(o))
    assert o == loads(dumps(o))

print "t408 done."
