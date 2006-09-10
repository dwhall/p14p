#! python
# :mode=c:
#
# Exceptions module
#

import object

#
# Native code to register instances of Exception with VM.
# This is needed by the VM during try/except blocks.
#
def _reg(code, inst):
    """__NATIVE__
    pPyObj_t pcode = C_NULL;
    pPyObj_t pinst = C_NULL;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 2)
    {
        return PY_RET_EX_TYPE;
    }

    /* Get the args */
    pcode = NATIVE_GET_LOCAL(0);
    pinst = NATIVE_GET_LOCAL(1);

    /* Throw type exception if necessary */
    if ((pcode->od.od_type != OBJ_TYPE_INT)
        || ((pinst->od.od_type != OBJ_TYPE_CLI) 
            && (pinst->od.od_type != OBJ_TYPE_FXN)))
    {
        return PY_RET_EX_TYPE;
    }
    
    /* register exn in gVmGlobals.pexns dict[code]=instance */
    return dict_setItem(PY_EXNS_DICT, pcode, pinst);
    """
    pass


#
# Exception is a Lame Class
#
def Exception(code, name):
    inst = Instantiate(Exception)
    inst.__code__ = code
    inst.__name__ = name
    _reg(code, inst)
    return inst

#
# Set Exception's parent class and other exception attributes
#
Exception.__p = object
Exception.__code__ = 0xE0
Exception.__name__ = "Exception"
_reg(0xE0, Exception)


#
# Create instances of exceptions.
# Code argument must match value in PyReturn_t
#
AssertionError = Exception(0xE4, "AssertionError")
#SystemExit = Exception(0xE1, "SystemExit")
#FloatingPointError = Exception(0xE2, "FloatingPointError")
#ZeroDivisionError = Exception(0xE3, "ZeroDivisionError")
#AttributeError = Exception(0xE5, "AttributeError")
#ImportError = Exception(0xE6, "ImportError")
#IndexError = Exception(0xE7, "IndexError")
#KeyError = Exception(0xE8, "KeyError")
#MemoryError = Exception(0xE9, "MemoryError")
#NameError = Exception(0xEA, "NameError")
#RuntimeError = Exception(0xEB, "RuntimeError")
#SyntaxError = Exception(0xEC, "SyntaxError")
#SystemError = Exception(0xED, "SystemError")
#TypeError = Exception(0xEE, "TypeError")
#ValueError = Exception(0xEF, "ValueError")
#Warning = Exception(0xD0, "Warning")
