#!/usr/bin/env python

# Copyright 2010 Dean Hall.  All rights reserved.

# Generates a C source file from one or more .py source files.
# The output C code is structs that define the code objects from the .py files


import pprint, sys


none = type(None)
code = type(compile("None", "None", "exec"))

constant_pool = {}
c_file_lines = []


def header():
    return "0xEE"


def gen_obj_name():
    """Generates object names and returns the name as a string."""
    obj_count = 0
    while True:
        yield "o%d" % obj_count
        obj_count += 1
gen_next_obj_name = gen_obj_name().next

# obj_to_cvar() gets or puts object in constant_pool dict
# The other *_to_cvar() funcs do 2 things:
# 1) Puts formatted C code in c_file_lines
# 2) Returns variable name as string

def none_to_cvar(n):
    d = {}
    d['hdr'] = header()
    d['val'] = 0
    d['nm'] = gen_next_obj_name()
    c_file_lines.append("PmNone_t const %(nm)s = {%(hdr)s, %(val)d};\n" % d)
    return d['nm']


def int_to_cvar(n):
    d = {}
    d['hdr'] = header()
    d['val'] = n
    d['nm'] = gen_next_obj_name()
    c_file_lines.append("PmInt_t const %(nm)s = {%(hdr)s, %(val)d};\n" % d)
    return d['nm']


def float_to_cvar(n):
    d = {}
    d['hdr'] = header()
    d['val'] = n
    d['nm'] = gen_next_obj_name()
    c_file_lines.append("PmFloat_t const %(nm)s = {%(hdr)s, %(val)f};\n" % d)
    return d['nm']


def string_to_cvar(s):
    len_s = len(s)
    d = {}
    d['hdr'] = header()
    d['len'] = len_s
    tuple_s = tuple(s)
    d['val'] = r"'%s'," * len_s % tuple_s + r"'\0'"
    d['nm'] = gen_next_obj_name()
    c_file_lines.append(
        "PmString_t const %(nm)s = {%(hdr)s, %(len)d, {%(val)s}};\n" % d)
    return d['nm']


def tuple_to_cvar(t):
    len_t = len(t)
    d = {}
    d['hdr'] = header()
    d['len'] = len_t
    d['objs'] = "(pPmObj_t)&%s," * len_t % tuple(map(obj_to_cvar, t))
    d['nm'] = gen_next_obj_name()
    c_file_lines.append(
        "PmTuple_t const %(nm)s = {%(hdr)s, %(len)d, {%(objs)s}};\n" % d)
    return d['nm']


def bc_to_cvar(bc):
    len_bc = len(bc)
    d = {}
    d['hdr'] = header()
    d['len'] = len_bc
    d['val'] = "%d," * len_bc % tuple(ord(b) for b in bc)
    d['nm'] = gen_next_obj_name()
    c_file_lines.append(
        "PmString_t const %(nm)s = {%(hdr)s, %(len)d, {%(val)s}};\n" % d)
    return d['nm']


def co_to_cvar(co):
    d = {}
    d['hdr'] = header()
    # No MemSpace?
    d['co_names'] = obj_to_cvar(co.co_names)
    d['co_consts'] = obj_to_cvar(co.co_consts)
    d['co_nfreevars'] = len(co.co_freevars)
    # Bytecode should not be looked up or stored in the constant pool
    d['co_code'] = bc_to_cvar(co.co_code)
    # TODO: Cell vars tuple (of ints)
    d['co_argcount'] = co.co_argcount
    d['co_flags'] = co.co_flags
    d['nm'] = gen_next_obj_name()
    c_file_lines.append(
        "PmCo_t const %(nm)s = {%(hdr)s, (pPmTuple_t)&%(co_names)s, "
        "(pPmTuple_t)&%(co_consts)s, %(co_nfreevars)d, (pPmString_t)&%(co_code)s, "
        "%(co_argcount)d, %(co_flags)d};\n" % d)
    return d['nm']


# Object type to cvar function lookup table
obj_to_cvar_func = {
    none: none_to_cvar,
    int: int_to_cvar,
    float: float_to_cvar,
    str: string_to_cvar,
    tuple: tuple_to_cvar,
    code: co_to_cvar,
}


def obj_to_cvar(o):
    """Returns varname of o.  o's obj is fetched from const pool or created."""
    
    # Use the object if it is in the constant pool
    if o in constant_pool.keys():
        c_file_lines.append("/* re-using %s from constant pool */\n" % constant_pool[o])
        return constant_pool[o]

    # Otherwise create the object and put it in the constant pool
    nm = obj_to_cvar_func[type(o)](o)
    constant_pool[o] = nm
    return nm


def main():
    def compile_file(fn):
        return compile(open(fn).read(), fn, 'exec')
    fns = sys.argv[1:]
    cos = map(compile_file, fns)
    d = {}
    c_file_lines.append('#include <stdint.h>\n#include "pm.h"\n')
    for fn,co in zip(fns,cos):
        c_file_lines.append("/* File: %s */\n" % fn)
        d[fn] = co_to_cvar(co)
    print "".join(c_file_lines)
#    pprint.pprint(d)


if __name__ == "__main__":
    main()
