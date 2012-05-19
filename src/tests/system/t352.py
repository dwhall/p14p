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
# System Test 352
# Fix modulus with negative integers
#

a = 2
n = 3

print "      2 % 3 in pymite:", (a % n), " and in cpython:", 2
print "   (-2) % 3 in pymite:", ((-a) % n), " and in cpython:", 1
print "   2 % (-3) in pymite:", (a % (-n)), " and in cpython:", -1
print "(-2) % (-3) in pymite:", ((-a) % (-n)), " and in cpython:", -2

assert (a % n) == 2
assert ((-a) % n) == 1
assert (a % (-n)) == -1
assert ((-a) % (-n)) == -2

assert 12 % 1 == 0
assert 12 % -1 == 0

# Division

assert -3 / 4 == -1
assert -4 / 3 == -2
assert 12 / 1 == 12
assert 12 / -1 == -12


import sys
assert (-sys.maxint-1) % -1 == 0

print "Expect OverflowError"
assert  (-sys.maxint-1) / -1