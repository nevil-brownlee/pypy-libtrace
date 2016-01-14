#!/usr/bin/env python

# Thu, 13 Mar 14 (PDT)
# ip6.py:  Demonstrate IPv6 objects
# Copyright (C) 2016, Nevil Brownlee, U Auckland | WAND

import plt
import natkit as nk

from cplt import ffi, lib
import string

ba = bytearray.fromhex(u"1002 2004 3005 4006")
for b in ba: print "%02x " % b,
print

print "short[2] = %04x" % nk.ba_get_short(ba,2),
print "short[4] = %04x" % nk.ba_get_short(ba,4),
print "long[2]  = %08x" % nk.ba_get_long(ba,2),
print "long[5]  = %s\n" % nk.ba_get_long(ba,5)

s = string.join(str(ba), '')  # Make a cdata version of ba
cba = ffi.new("uint8_t[%d]" % len(s), s)
for b in cba: print "%02x " % b,
print

print "short[2] = %04x" % nk.ba_get_short(cba,2),
print "short[4] = %04x" % nk.ba_get_short(cba,4),
print "long[2]  = %08x" % nk.ba_get_long(cba,4),
print "long[5]  = %s\n" % nk.ba_get_long(cba,5)

a=5;  b=6
print "a=%x, b=%x,  a>b = %s" % (a, b, nk.seq_gt(a,b))
print "a=%x, b=%x, a>=b = %s" % (a, b, nk.seq_ge(a,b))
print "a=%x, b=%x, a<b  = %s\n" % (a, b, nk.seq_lt(a,b))

a = 0xffff0000;  b = 0xfffeffff  # b < a
print "a=%x, b=%x,  a+b= %x" % (a, b, nk.seq_add(a,b))
print "a=%x, b=%x,  a-b= %x\n" % (a, b, nk.seq_sub(a,b))

print "a=%x, b=%x,  a>b = %s" % (a, b, nk.seq_gt(a,b))
print "a=%x, b=%x, a>=b = %s" % (a, b, nk.seq_ge(a,b))
print "a=%x, b=%x, a<b  = %s\n" % (a, b, nk.seq_lt(a,b))

b = 0xffff0001  # b > a
print "a=%x, b=%x,  a>b = %s" % (a, b, nk.seq_gt(a,b))
print "a=%x, b=%x, a>=b = %s" % (a, b, nk.seq_ge(a,b))
print "a=%x, b=%x, a<b  = %s\n" % (a,b, nk.seq_lt(a,b))

b = a
print "a=%x, b=%x,  a>b = %s" % (a, b, nk.seq_gt(a,b))
print "a=%x, b=%x, a>=b = %s" % (a, b, nk.seq_ge(a,b))
print "a=%x, b=%x, a<b  = %s\n" % (a, b, nk.seq_lt(a,b))
