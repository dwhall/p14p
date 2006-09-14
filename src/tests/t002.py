#
# Feature Test for Issue #2
#
# Separate stdlib from user app
#
# The test below proves that push42() was called from the usrlib native code
# and assert was called from the stdlib native code.
#

#
# Pushes the int, 42, onto the stack
#
def push42():
    """__NATIVE__
    pPyObj_t pint = C_NULL;
    PyReturn_t retval;

    retval = int_new((S32)42, &pint);
    NATIVE_SET_TOS(pint);

    return retval;
    """
    pass

foo = push42()
bar = 6 * 7
assert foo == bar
