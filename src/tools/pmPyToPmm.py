#! /usr/bin/env python

# This file is Copyright 2012 Dean Hall.
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
# Compiles a python source file and converts it to a marshalled code object.
# Also known as a P14p Marshalled Module (pmm)
#


import sys, os.path
import pmMarshal


# The following must match epynomous var in src/vm/marshal.h
PM_MARSHAL_FILE_EXTENSION = ".pmm"


def pyToPmm(fn):
    if fn: fin = open(fn, 'r')
    else: fin = sys.stdin
    src = "".join(fin.readlines())
    co = compile(src, str(fn), "exec",)
    com = pmMarshal.dumps(co)
    
    if fn: fout = open(os.path.splitext(sys.argv[1])[0] +
                       PM_MARSHAL_FILE_EXTENSION,
                       'wb')
    else: fout = sys.stdout
    fout.write(com)


if __name__ == "__main__":
    fn = None
    if len(sys.argv) != 1:
        fn = sys.argv[1]
    pyToPmm(fn)
