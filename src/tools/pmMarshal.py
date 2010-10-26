#!/usr/bin/env python

# Copyright 2010 Dean Hall.  All rights reserved.

# Marshals Python objects to a serial format
# and unmarshals the serial format to a Python object.
# The serialization format used here differs from that of Python's:
# http://docs.python.org/library/marshal.html
# http://daeken.com/python-marshal-format


import marshal, struct
import pmConstantPool


co = compile("None", "", "single")
code_t = type(co)
one_byte = pmConstantPool.pmConstantPool()
one_byte[None] = "N"
one_byte[True] = "T"
one_byte[False] = "F"
one_byte[0] = "0"
one_byte[1] = "1"
one_byte[2] = "2"
one_byte[3] = "3"
one_byte[4] = "4"
one_byte[5] = "5"
one_byte[6] = "6"
one_byte[7] = "7"
one_byte[8] = "8"
one_byte[9] = "9"

rev_one_byte = one_byte.reverse()


def _gen_co_field(s):
    """Yields the next item in the code object.
    """
    assert s[0] == 'C'
    i = 1
    for n in xrange(7):
        o = loads(s[i:])
        yield o
        i += len(dumps(o))
    yield struct.unpack("<H", s[i:i+2])[0]
    yield ord(s[i+2])
    yield ord(s[i+3])
    yield ord(s[i+4])
    yield ord(s[i+5])
    yield ord(s[i+6])


def _gen_names(s, n):
    for i in xrange(n):
        yield "%s%d" % (s,i)


def translate_co(s):
    """Translates a serialized code object from pmMarshal's format to Python's.
    """
    # Get the fields from the pmMarshal format
    (co_name, co_filename, co_code, co_lnotab, co_names, co_consts, co_cellvars,
     co_firstlineno, co_argcount, co_flags, co_stacksize, co_nlocals,
     co_nfreevars) = tuple(_gen_co_field(s))

    # Create fields
    co_varnames = tuple(_gen_names("nm", co_nlocals))
    co_freevars = tuple(_gen_names("fv", co_nfreevars))

    # Create CPython marshal format string from fields (see Python/marshal.c)
    ba = bytearray('c')
    for i in (co_argcount, co_nlocals, co_stacksize, co_flags):
        ba.extend(struct.pack("<l", i))
    for i in (co_code, co_consts, co_names, co_varnames, co_freevars,
              co_cellvars, co_filename, co_name):
        ba.extend(marshal.dumps(i))
    ba.extend(struct.pack("<l", co_firstlineno))
    ba.extend(marshal.dumps(co_lnotab))
    return str(ba)


def loads(s):
    """Returns a new object described by the serialzed data.
    """
    t = s[0]
    if t in rev_one_byte:
        return rev_one_byte[t]

    if t == 'I':
        return struct.unpack("<i", s[1:5])[0]
    elif t == 'H':
        return struct.unpack("<h", s[1:3])[0]
    elif t == 'B':
        return struct.unpack("<b", s[1])[0]

    elif t == 'R':
        return struct.unpack("<f", s[1:5])[0]

    elif t in 's':
        l = struct.unpack("<B", s[1])[0]
        return str(s[2:2+l])
    elif t in 'S':
        l = struct.unpack("<H", s[1:3])[0]
        return str(s[3:3+l])

    elif t in "()[]":
        l = []
        if t in "([":
            n = struct.unpack("<B", s[1])[0]
            s = s[2:]
        else:
            n = struct.unpack("<H", s[1:3])[0]
            s = s[3:]
        for i in xrange(n):
            o = loads(s)
            len_o = len(dumps(o))
            l.append(o)
            s = s[len_o:]
        if t in "()":
            return tuple(l)
        return l

    elif t in '{}':
        d = {}
        if t == "{":
            n = struct.unpack("<B", s[1])[0]
            s = s[2:]
        else:
            n = struct.unpack("<H", s[1:2])[0]
            s = s[3:]
        for i in xrange(n):
            k = loads(s)
            len_o = len(dumps(k))
            s = s[len_o:]
            v = loads(s)
            len_o = len(dumps(v))
            s = s[len_o:]
            d[k] = v
        return d

    elif t == 'C':
        return marshal.loads(translate_co(s))

    else:
        print "Offending string:", repr(s)
        raise TypeError


def dumps(o):
    """Returns the serialized format of the given object.
    """
    if o in one_byte:
        return one_byte[o]

    type_o = type(o)

    if type_o == int:
        if o >= -128 and o < 128:
            return "B%s" % struct.pack("<b", o)
        elif o >= -32768 and o < 32768:
            return "H%s" % struct.pack("<h", o)
        else:
            return "I%s" % struct.pack("<i", o)

    elif type_o == float:
        return "R%s" % struct.pack("<f", f)

    elif type_o == str:
        if len(o) < 256:
            return "s%s%s" % (struct.pack("<B", len(o)), o)
        return "S%s%s" % (struct.pack("<H", len(o)), o)

    elif type_o == tuple or type_o == list:
        len_o = len(o)
        if len_o < 256:
            if type_o == tuple:
                buff = bytearray("(")
            else:
                buff = bytearray("[")
            buff.extend(struct.pack("<B", len_o))
        else:
            if type_o == tuple:
                buff = bytearray(")")
            else:
                buff = bytearray("]")
            buff.extend(struct.pack("<H", len_o))
        for i in o:
            buff.extend(dumps(i))
        return str(buff)

    elif type_o == dict:
        len_o = len(o)
        assert len_o < 2**16
        if len_o < 256:
            buff = bytearray("{%s" % struct.pack("<b", len_o))
        else:
            buff = bytearray("}%s" % struct.pack("<h", len_o))
        for k,v in o.iteritems():
            buff.extend(dumps(k))
            buff.extend(dumps(v))
        return str(buff)

    elif type_o == code_t:
        buff = bytearray("C")
        buff.extend(dumps(o.co_name))
        buff.extend(dumps(o.co_filename))
        buff.extend(dumps(o.co_code))
        buff.extend(dumps(o.co_lnotab))
        buff.extend(dumps(o.co_names))
        buff.extend(dumps(o.co_consts))
        buff.extend(dumps(o.co_cellvars))
        buff.extend(struct.pack("<H", o.co_firstlineno))
        buff.append(o.co_argcount)
        buff.append(o.co_flags)
        buff.append(o.co_stacksize)
        buff.append(o.co_nlocals)
        buff.append(len(o.co_freevars))
        return str(buff)

    else:
        raise TypeError


def _test_dumps():
    for i in range(-4, 11):
        print repr(dumps(i))
    print repr(dumps(30583))
    print repr(dumps(43690))
    d = {}
    d[0] = "zero"
    t = (256, "tfs", 9, co)
    d['t'] = t
    s = dumps(t)
    print len(s), ":", repr(s)
    s = dumps(d)
    print len(s), ":", repr(s)
    for o in t:
        s = dumps(o)
        print len(s), ":", repr(s)
    r = range(5,20,3)
    s = dumps(r)
    print len(s), ":", repr(s)


def _test_loads():
    print loads("N")
    print loads("T")
    print loads("F")
    print loads("0")
    print loads("I\x20\x00\x00\x00")
    print loads("I\x20\x00\x00\x80")
    print loads(dumps((1,2,3)))
    print loads(dumps(range(5,20,3)))
    print loads(dumps(_test_loads.__code__))


def _test():
    def foo():
        print 42

    assert dumps((False,)) == "(\x01F"
    assert dumps((True,)) == "(\x01T"
    assert loads(dumps(False)) == False
    assert loads(dumps((False,))) == (False,)
    for i in (-50000, -200, -64, -1, 0):
        assert loads(dumps(i)) == i
        assert loads(dumps(-i)) == -i
    for i in ((), [], {}, foo.__code__):
        assert loads(dumps(i)) == i
    for i in ((True,), (False,), (0,1),):
        assert loads(dumps(i)) == i


if __name__ == "__main__":
    _test()
    _test_dumps()
    _test_loads()
