#!/usr/bin/env python

# Copyright 2010 Dean Hall.  All rights reserved.

# Generates a C source file from one or more .py source files.
# The output C code is structs that define the code objects from the .py files


# TODO: run COs through filter, handle native functions.


import os, sys
import pmConstantPool
from pmCoFilter import filter_co


# Some COs in the builtins module should be exposed for easy access
builtin_co_to_expose = {
    "Generator": "pm_co_global_co_generator",
    "Exception": "pm_co_global_co_exception",
}


none = type(None)
code = type(compile("None", "None", "exec"))
bc = ("BC",)
typeval = {
    none: "OBJ_TYPE_NON",
    bool: "OBJ_TYPE_BOOL",
    int: "OBJ_TYPE_INT",
    float: "OBJ_TYPE_FLT",
    str: "OBJ_TYPE_STR",
    tuple: "OBJ_TYPE_TUP",
    code: "OBJ_TYPE_COB",
    bc: "OBJ_TYPE_STR",
}
ctype = {
    none: "PmNone%s_t",
    bool: "PmBool%s_t",
    int: "PmInt%s_t",
    float: "PmFloat%s_t",
    str: "PmString%s_t",
    tuple: "PmTuple%s_t",
    code: "PmCo%s_t",
    bc: "PmString%s_t",
}


constant_pool = pmConstantPool.pmConstantPool()
c_file_lines = []
string_sizes = set()
tuple_sizes = set()


def header(typ, nm, size=""):
    ctyp = ctype[typ] % str(size)
    return "%s PM_PLAT_PROGMEM %s = {PM_DECLARE_OD(%s, sizeof(%s))" \
           % (ctyp, nm, typeval[typ], ctyp)


def gen_obj_name():
    """Generates object names and returns the name as a string."""
    obj_count = 0
    while True:
        yield "o%d" % obj_count
        obj_count += 1
gen_next_obj_name = gen_obj_name().next

# obj_to_cvar() gets or puts object in constant_pool dict
# The other *_to_cvar() funcs put formatted C code in c_file_lines

def none_to_cvar(o, nm):
    c_file_lines.append("%s, %d};\n" % (header(none, nm), 0))
    return nm


def bool_to_cvar(o, nm):
    c_file_lines.append("%s, %d};\n" % (header(bool, nm), (0,1)[o]))
    return nm


def int_to_cvar(o, nm):
    c_file_lines.append("%s, %d};\n" % (header(int, nm), o))
    return nm


def float_to_cvar(o, nm):
    c_file_lines.append("%s, %f};\n" % (header(float, nm), n))
    return nm


def _c_repr(n):
    """Returns n as a printable ascii char surrounded by single quotes,
    or a decimal number as string.
    Apostrophe and backslash are excluded to avoid trouble with C char syntax.
    """
    if n >= 32 and n < 127 and n != 39 and n != 92:
        return repr(chr(n))
    else:
        return str(n)


# C strings have null-terminator, but length field must keep Python length val
def string_to_cvar(o, nm):
    len_o = len(o)
    string_sizes.add(len_o + 1)
    chars = "%s," * len_o % tuple(_c_repr(ord(c)) for c in o) + r"'\0'"
    c_file_lines.append("%s, %d, {%s}};\n"
                        % (header(str, nm, len_o + 1), len_o, chars))
    return nm


# Bytecode is very similar to a string, but its bytes are printed numerically
# and there is no null terminator.
def bc_to_cvar(o, nm):
    len_o = len(o)
    string_sizes.add(len_o)
    bcode = "%d," * len_o % tuple(map(ord, o))
    c_file_lines.append("%s, %d, {%s}};\n"
                        % (header(bc, nm, len_o), len_o, bcode))
    return nm


def tuple_to_cvar(o, nm):
    len_o = len(o)
    tuple_sizes.add(len_o)
    objs = "(pPmObj_t)&%s," * len_o % tuple(map(obj_to_cvar, o))
    c_file_lines.append("%s, %d, {%s}};\n"
                        % (header(tuple, nm, len_o), len_o, objs))
    return nm


def co_to_cvar(co, nm):

    if co.co_name in builtin_co_to_expose:
        nm = builtin_co_to_expose[co.co_name]
        
    # DWH
    # filter co, extract native
    #filter_co(co)

    d = {}
    d['hdr'] = header(code, nm)
    d['co_name'] = obj_to_cvar(co.co_name)
    d['co_filename'] = obj_to_cvar(co.co_filename)
    d['co_code'] = obj_to_cvar(co.co_code, None, bc)
    d['co_lnotab'] = obj_to_cvar(co.co_lnotab, None, bc)
    d['co_names'] = obj_to_cvar(co.co_names)
    d['co_consts'] = obj_to_cvar(co.co_consts)

    # Cellvars is changed into a tuple of indices into co_varnames
    cellvars = [-1,] * len(co.co_cellvars)
    for i,name in enumerate(co.co_cellvars):
        if name in co.co_varnames:
            cellvars[i] = co.co_varnames.index(name)
    d['co_cellvars'] = obj_to_cvar(tuple(cellvars))

    c_file_lines.append(
        "%(hdr)s, "
        "(pPmString_t)&%(co_name)s, "
        "(pPmString_t)&%(co_filename)s, "
        "(pPmString_t)&%(co_code)s, "
        "(pPmString_t)&%(co_lnotab)s, "
        "(pPmTuple_t)&%(co_names)s, "
        "(pPmTuple_t)&%(co_consts)s, "
        "(pPmTuple_t)&%(co_cellvars)s, " % d)
    c_file_lines.append("%d, %d, %d, %d, %d, %d};\n" %
        (co.co_firstlineno,
         co.co_argcount,
         co.co_flags & 0xFF,
         co.co_stacksize,
         co.co_nlocals,
         len(co.co_freevars)))
    return nm


obj_type_to_cvar_func_lookup = {
    none: none_to_cvar,
    bool: bool_to_cvar,
    int: int_to_cvar,
    float: float_to_cvar,
    str: string_to_cvar,
    tuple: tuple_to_cvar,
    code: co_to_cvar,
    bc: bc_to_cvar,
}


def obj_to_cvar(o, nm=None, typ=None):
    """Returns varname of o; either fetched from the constant pool or created"""

    if o in constant_pool:
        return constant_pool[o]

    # Otherwise create the object and put it in the constant pool
    varnm = nm or gen_next_obj_name()
    vartype = typ or type(o)
    cvar_func = obj_type_to_cvar_func_lookup[vartype]
    varnm = cvar_func(o, varnm)
    constant_pool[o] = varnm
    return varnm


def gen_globals():
    """Adds designated globals to the constant pool"""
    c_file_lines.append('\n/* Globals */\n')
    obj_to_cvar(None, "pm_co_global_none")
    obj_to_cvar(-1, "pm_co_global_negone")
    obj_to_cvar(0, "pm_co_global_zero")
    obj_to_cvar(1, "pm_co_global_one")
    obj_to_cvar(2, "pm_co_global_two")
    obj_to_cvar(3, "pm_co_global_three")
    obj_to_cvar(4, "pm_co_global_four")
    obj_to_cvar(5, "pm_co_global_five")
    obj_to_cvar(6, "pm_co_global_six")
    obj_to_cvar(7, "pm_co_global_seven")
    obj_to_cvar(8, "pm_co_global_eight")
    obj_to_cvar(9, "pm_co_global_nine")
    obj_to_cvar(True, "pm_co_global_true")
    obj_to_cvar(False, "pm_co_global_false")

    # Generator and Exception strings not needed, only their COs
    obj_to_cvar("__bi", "pm_co_global_string_bi")
    obj_to_cvar("code", "pm_co_global_string_code")
    obj_to_cvar("__init__", "pm_co_global_string_init")
    obj_to_cvar("next", "pm_co_global_string_next")

    # Other useful objects
    obj_to_cvar((), "pm_co_global_empty_tuple")


def gen_module_table(mods):
    """Generates a lookup table of a module name to its code-object"""

    c_file_lines.append("\n/* Previously undeclared module names */\n")
    map(obj_to_cvar, mods)

    c_file_lines.append("\nPmModuleEntry_t PM_PLAT_PROGMEM module_table[] =\n{\n")
    for mod, varnm in mods.iteritems():
        c_file_lines.append("    {(pPmString_t)&%s, (pPmCo_t)&%s},\n" %
                            (obj_to_cvar(mod), varnm))
    c_file_lines.append("    {C_NULL, C_NULL},\n};\n")


def _compile_file(fn):
    return compile(open(fn).read(), fn, 'exec')


def gen_modules(fns):
    """Creates table of filename, code-object-varname pairs"""
    mods = {}
    splitext = os.path.splitext
    basename = os.path.basename
    for fn in fns:
        c_file_lines.append("\n/* File: %s */\n" % fn)
        modulename = splitext(basename(fn))[0]
        co = _compile_file(fn)
        mods[modulename] = obj_to_cvar(co)

    gen_module_table(mods)


def gen_dyn_lens():
    """Puts tuple and string type declarations at the top of the file."""
    for size in string_sizes:
        c_file_lines.insert(0, "PM_DECLARE_STRING_TYPE(%d);\n" % size)
    for size in tuple_sizes:
        c_file_lines.insert(0, "PM_DECLARE_TUPLE_TYPE(%d);\n" % size)


if __name__ == "__main__":
    filenames = sys.argv[1:]

    gen_globals()
    gen_modules(filenames)
    gen_dyn_lens()
    c_file_lines.insert(0, '#include <stdint.h>\n#include "pm.h"\n\n')
    print "".join(c_file_lines)
