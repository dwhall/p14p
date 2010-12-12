#! /usr/bin/env python

# This file is Copyright 2010 Dean Hall.
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



class pmConstantPool(dict):
    """A constant pool (based on dict type) that can distinguish True from 1
    and False from 0.
    """
    def __init__(self, d={}):
        dict.__init__(self)
        self.data = d
        self.trueValue = None
        self.falseValue = None


    def __contains__(self, o):
        if o is True: return self.trueValue != None
        if o is False: return self.falseValue != None
        return o in self.data.keys()


    def __setitem__(self, k, v):
        if k is True: self.trueValue = v; return v
        if k is False: self.falseValue = v; return v
        self.data[k] = v
        return v


    def __getitem__(self, k):
        if k is True: return self.trueValue
        if k is False: return self.falseValue
        return self.data[k]


    def reverse(self,):
        d = dict((v,k) for k,v in self.data.iteritems())
        if self.trueValue:
            d[self.trueValue] = True
        if self.falseValue:
            d[self.falseValue] = False
        return d


def test():
    c = pmConstantPool()
    c[0] = "0"
    c[1] = "1"
    c[None] = "N"
    c[True] = "T"
    c[False] = "F"

    assert 0 in c and 1 in c and None in c
    assert c.reverse() == {"0":0, "1":1, "N":None, "T":True, "F":False}
    print "Tests pass."


if __name__ == "__main__":
    test()
