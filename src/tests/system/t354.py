#
# PyMite - A flyweight Python interpreter for 8-bit and larger microcontrollers.
# Copyright 2010 Dean Hall.  All rights reserved.
# PyMite is offered through one of two licenses: commercial or open-source.
# See the LICENSE file at the root of this package for licensing details.
#

#
# System Test 354
#

assert int(1.0) == 1
assert int(1.5) == 1
assert int(1.9) == 1

assert int(-1.0) == -1
assert int(-1.5) == -1
assert int(-1.9) == -1

assert int("1.0") == 1
assert int("1.5") == 1
assert int("1.9") == 1

assert int("-1.0") == -1
assert int("-1.5") == -1
assert int("-1.9") == -1


assert float(1) == 1.0
assert float(-1) == -1.0
assert float("1.0") == 1.0
assert float("1.5") == 1.5
assert float("-1.5") == -1.5
