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
# System Test 370
#


import string

# Create a string longer than 256 bytes to expose a too-small datatype in
# src/lib/string.py's find()
s = "0123456789012345678901234567890123456789" \
    "0123456789012345678901234567890123456789" \
    "0123456789012345678901234567890123456789" \
    "0123456789012345678901234567890123456789" \
    "0123456789012345678901234567890123456789" \
    "0123456789012345678901234567890123456789" \
    "0123456789012345678901234567890123456789" \
    "A"

loc_A = string.find(s, "A")
assert loc_A  == len(s) - 1
