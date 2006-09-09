#! python
# :mode=c:
#
# Exceptions module
#

import object

#
# Exception is a Lame Class
#
def Exception(code, name):
    inst = Instantiate(Exception)
    inst.__code__ = code
    inst.__name__ = name
    _ini(inst)
    return inst

#
# Set Exception's parent class and other exception attributes
#
Exception.__p = object
Exception.__code__ = 0xE0
Exception.__name__ = "Exception"

#
# Native code to register instances of Exception with VM.
# This is needed by the VM during try/except blocks.
#
def _ini(inst):
    """__NATIVE__
    /* register exn in gVmGlobals.exns dict[code]=instance */
    """
    pass

#
# Create instances of exceptions.
# Code argument must match value in PyReturn_t
#
SystemExit = Exception(0xE1, "SystemExit")
FloatingPointError = Exception(0xE2, "FloatingPointError")
ZeroDivisionError = Exception(0xE3, "ZeroDivisionError")
AssertionError = Exception(0xE4, "AssertionError")
AttributeError = Exception(0xE5, "AttributeError")
ImportError = Exception(0xE6, "ImportError")
IndexError = Exception(0xE7, "IndexError")
KeyError = Exception(0xE8, "KeyError")
MemoryError = Exception(0xE9, "MemoryError")
NameError = Exception(0xEA, "NameError")
RuntimeError = Exception(0xEB, "RuntimeError")
SyntaxError = Exception(0xEC, "SyntaxError")
SystemError = Exception(0xED, "SystemError")
TypeError = Exception(0xEE, "TypeError")
ValueError = Exception(0xEF, "ValueError")
Warning = Exception(0xD0, "Warning")
