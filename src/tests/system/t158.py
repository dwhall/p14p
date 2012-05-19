# This file is Copyright 2003 Dean Hall.
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
# System Test 158
# Fix builtin sum() to support float
#


l1 = [1, 2, 3]
assert sum(l1) == 6
print l1

l2 = [1, 2, 3, 4.0]
assert sum(l2) == 10.0
print l2

l3 = [1.0, 2.0, 3.0]
assert sum(l3) == 6.0                  
print l3

# PyMite VM uses float type, not double; so this test passes here,
# but would fail if run on the CPython VM.
l4 = [1.1, 2.2, 3.3]
assert sum(l4) != 6.6   # Rounding issue causes not-equal
print l4

l5 = [1.1, 2.2, 3.3, 4.4]
assert sum(l5) == 11.0
print l5
