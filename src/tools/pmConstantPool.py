#! /usr/bin/env python


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


if __name__ == "__main__":
    # Testing
    c = pmConstantPool()
    c[0] = "0"
    c[1] = "1"
    c[None] = "N"
    c[True] = "T"
    c[False] = "F"

    assert 0 in c and 1 in c and None in c
    print c
    print c.reverse()