# This file is Copyright 2011 Dean Hall.
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
# System Test 384
#

assert None == None
assert None is None
assert (None is not None) == False
assert (not None is None) == False
assert (None != None) == False
assert (1 is None) == False
assert (1 == None) == False
assert (1 is not None) == True
assert (not 1 is None) == True
assert (1 != None) == True


def foo(): pass
def bar(): pass

assert (foo is None) == False
assert foo is not None
assert (None is foo) == False
assert None is not foo

assert (foo is bar) == False
assert foo is not bar
assert (bar is foo) == False
assert bar is not foo
