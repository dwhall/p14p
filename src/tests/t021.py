#
# Feature Test for Issue #21
#
# Implement exceptions
#
# This test investigates how instances of objects are created
#
import sys

try:
    a = 6/0
except Exception, e:
    pass

try:
    a = 6/0
except ZeroDivisionError, e:
    pass

try:
    a = 6/0
except ImportError, e:
    #assert(0)
    sys.exit(1)
except ZeroDivisionError, e:
    pass
