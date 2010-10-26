#!/usr/bin/env python

# Copyright 2010 Dean Hall.  All rights reserved.

# Generates a C source file from one or more .py source files.
# The output C code is structs that define the code objects from the .py files


# TODO: run COs through filter, handle native functions.


import os, sys
import pmConstantPool
from pmCoFilter import co_filter_factory

# filter_co = co_filter_factory(pmfeatures_filename)

GLOBAL_PREFIX = "pm_global_%s"

# Expose these COs from the builtins module for easy access in the VM
co_to_expose = {
    "Generator": GLOBAL_PREFIX % "co_generator",
    "Exception": GLOBAL_PREFIX % "co_exception",
}


none = type(None)
code = type(compile("None", "None", "exec"))
typeval = {
    none: "OBJ_TYPE_NON",
    bool: "OBJ_TYPE_BOOL",
    int: "OBJ_TYPE_INT",
    float: "OBJ_TYPE_FLT",
    str: "OBJ_TYPE_STR",
    tuple: "OBJ_TYPE_TUP",
    code: "OBJ_TYPE_COB",
}
ctype = {
    none: "PmNone%s_t",
    bool: "PmBool%s_t",
    int: "PmInt%s_t",
    float: "PmFloat%s_t",
    str: "PmString%s_t",
    tuple: "PmTuple%s_t",
    code: "PmCo%s_t",
}


cname_kpool = pmConstantPool.pmConstantPool()
crepr_kpool = {}
ordered_cnames = []
#c_file_lines = []
string_sizes = set()
tuple_sizes = set()


def gen_obj_name():
    """Generates object names and returns the name as a string."""
    obj_count = 0
    while True:
        yield "o%d" % obj_count
        obj_count += 1
gen_next_obj_name = gen_obj_name().next


def _byte_crepr(c):
    """Returns c as a printable ascii char surrounded by single quotes,
    or a decimal number as string.
    Apostrophe and backslash are excluded to avoid trouble with C char syntax.
    """
    n = ord(c)
    if n >= 32 and n < 127 and n != 39 and n != 92:
        return repr(c)
    else:
        return str(n)


def header(typ, nm, size=""):
    ctyp = ctype[typ] % str(size)
    return "%s PM_PLAT_PROGMEM %s = {PM_DECLARE_OD(%s, sizeof(%s))" \
           % (ctyp, nm, typeval[typ], ctyp)


def string_to_crepr(o, nm):
    len_o = len(o)
    string_sizes.add(len_o)
    chars = "%s," * len_o % tuple(map(_byte_crepr, o))
    return "%s, %d, {%s}};\n" % (header(str, nm, len_o), len_o, chars)


def tuple_to_crepr(o, nm):
    len_o = len(o)
    tuple_sizes.add(len_o)
    objs = "(pPmObj_t)&%s," * len_o % tuple(map(obj_to_cvar, o))
    return "%s, %d, {%s}};\n" % (header(tuple, nm, len_o), len_o, objs)


def co_to_crepr(co, nm):

    # DWH TODO:
    # co = filter_co(co)
    # if co_has_native(co): pass

    d = {}
    d['hdr'] = header(code, nm)
    d['co_name'] = obj_to_cvar(co.co_name)
    d['co_filename'] = obj_to_cvar(co.co_filename)
    d['co_code'] = obj_to_cvar(co.co_code)
    d['co_lnotab'] = obj_to_cvar(co.co_lnotab)
    d['co_names'] = obj_to_cvar(co.co_names)
    d['co_consts'] = obj_to_cvar(co.co_consts)

    # Cellvars is changed into a tuple of indices into co_varnames
    cellvars = [-1,] * len(co.co_cellvars)
    for i,name in enumerate(co.co_cellvars):
        if name in co.co_varnames:
            cellvars[i] = co.co_varnames.index(name)
    d['co_cellvars'] = obj_to_cvar(tuple(cellvars))

    crepr = bytearray(
        "%(hdr)s, "
        "(pPmString_t)&%(co_name)s, "
        "(pPmString_t)&%(co_filename)s, "
        "(pPmString_t)&%(co_code)s, "
        "(pPmString_t)&%(co_lnotab)s, "
        "(pPmTuple_t)&%(co_names)s, "
        "(pPmTuple_t)&%(co_consts)s, "
        "(pPmTuple_t)&%(co_cellvars)s, " % d)
    crepr.extend("%d, %d, %d, %d, %d, %d};\n" %
        (co.co_firstlineno,
         co.co_argcount,
         co.co_flags & 0xFF,
         co.co_stacksize,
         co.co_nlocals,
         len(co.co_freevars)))
    return str(crepr)


objtype_to_crepr_func_table = {
    none: lambda o, nm: "%s, %d};\n" % (header(none, nm), 0),
    bool: lambda o, nm: "%s, %d};\n" % (header(bool, nm), int(o)),
    int: lambda o, nm: "%s, %d};\n" % (header(int, nm), o),
    float: lambda o, nm: "%s, %f};\n" % (header(float, nm), o),
    str: string_to_crepr,
    tuple: tuple_to_crepr,
    code: co_to_crepr,
}


def obj_to_cvar(o, name=None, typ=None):
    """Returns varname of o; either fetched from the constant pool or created.
    Puts the C variable name in the constant pool and the C representation
    in a table (keyed by C variable name).
    """

    if o in cname_kpool:
        return cname_kpool[o]

    # If object is a code-object that needs to be exposed, use its special name
    vartype = typ or type(o)
    if vartype == code and o.co_name in co_to_expose:
        name = co_to_expose[o.co_name]

    # Otherwise use the given name or generate one
    varname = name or gen_next_obj_name()
    cname_kpool[o] = varname

    # Create the C representation of the object and store it
    _obj_to_crepr = objtype_to_crepr_func_table[vartype]
    crepr_kpool[varname] = _obj_to_crepr(o, varname)

    ordered_cnames.append(varname)

    return varname


def process_globals():
    """Adds VM globals and useful constants to the constant pool"""
    cobjs = (None, -1,0,1,2,3,4,5,6,7,8,9,True,False,
             "__bi","code","__init__","next",(),"")
    cnames = tuple(GLOBAL_PREFIX % nm for nm in
                   ("none", "negone", "zero", "one", "two", "three", "four",
                    "five", "six", "seven", "eight", "nine", "true", "false",
                    "string_bi", "string_code", "string_init", "string_next",
                    "empty_tuple", "empty_string",))
    map(obj_to_cvar, cobjs, cnames)


def process_modules(fns):
    global module_table_lines

    varname = GLOBAL_PREFIX % "module_table"
    module_table_lines = [
        "\n/* Module table */\nPmModuleEntry_t PM_PLAT_PROGMEM %s[] =\n{\n" 
        % varname]

    splitext = os.path.splitext
    basename = os.path.basename
    for fn in fns:
        modulename = splitext(basename(fn))[0]
        co = compile(open(fn).read(), fn, 'exec')
        nm = obj_to_cvar(co)
        module_table_lines.append("    {(pPmString_t)&%s, (pPmCo_t)&%s},\n" %
                                  (obj_to_cvar(modulename), nm))
    module_table_lines.append("    {C_NULL, C_NULL},\n};\n")


def process_and_write_modules(fns, fout=sys.stdout):
    # Order of process_* is important
    process_globals()
    process_modules(fns)

    cfile_lines = ['#include <stdint.h>\n#include "pm.h"\n\n'
                   '/* Decls for types with various sizes */\n']
    for size in string_sizes:
        cfile_lines.append("PM_DECLARE_STRING_TYPE(%d);\n" % size)
    for size in tuple_sizes:
        cfile_lines.append("PM_DECLARE_TUPLE_TYPE(%d);\n" % size)

    cfile_lines.append("\n/* Constant pool */\n")
    for cname in ordered_cnames:
        cfile_lines.extend(crepr_kpool[cname])

    cfile_lines.extend(module_table_lines)

    fout.write("".join(cfile_lines))


if __name__ == "__main__":
    filenames = sys.argv[1:]
    process_and_write_modules(filenames)

