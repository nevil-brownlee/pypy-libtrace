# 1350, Mon  7 Dec 2015 (NZDT)
#
# test-ipp.py: Test program for the ipp module

import sys
from plt_testing import *

def test_string(name, val):
    test_println(" %s = %s  " % (name, val), get_tag())
    try:
        prefix = ipp.from_s(val)
        test_println("%s = %s" % (name, prefix), get_tag())
        ps = str(prefix)
        if ps.find("::") >= 0:
            sc = ps.replace("::", "!")
            if sc.count(":") == 5:
                nps = sc.replace("!", ":0:");
                test_println(">>> nps = %s" % nps, get_tag())
            
    except Exception as err:  # py3 as instead of ,
        #traceback.print_exc()
        test_println("err = %s" % err, get_tag())

test_string("p61", "2001:df0:0:321:1:2:3:4/128")
test_string("p62", "2001:df0:0:0:1:2:3:4/128")
test_string("p63", "2001:df0:0:0:1:2::")
test_string("p64", "2001:df0:0:abcd::1")
test_string("p65", "2001:0:0df0::2")
test_string("p66", "2001::def0::2")
test_string("p67", "::ffff:1.2.3.4")  # From RFC 5952
test_string("p68", "1:2:3:0:5:6:7:8")  # Single 0 pair can't be ::
test_string("p69", "1:2:3::5:6:7:8")

test_string("\np41", "130.216.38.7/24")
test_string("p42", "130.256.0.0")
test_string("p43", "130.216.0.0/33")
test_string("p44", "130.216.0.0/-1")
test_string("p45", "130.216/24")

p4 = ipp.from_s("130.216.0.0/24")
test_println("\np4 = %s, type(p4) = %s" % (p4, type(p4)), get_tag())
ba = bytearray([130, 216, 128, 0])
q4 = ipp.IPprefix(4, ba, 25)
test_println("q4 = %s, type(q4) = %s\n" % (q4, type(q4)), get_tag())

test_println("p4 <  q4: result = %s, %s (T, F)" % (p4 < q4, q4 < p4))
test_println("p4 <= q4: result = %s, %s (T, F)" % (p4 <= q4, q4 <= p4), get_tag())
test_println("p4 == q4: result = %s, %s (F, F)" % (p4 == q4, q4 == p4), get_tag())
test_println("p4 != q4: result = %s, %s (T, T)" % (p4 != q4, q4 != p4), get_tag())
test_println("p4 >= q4: result = %s, %s (F, T)" % (p4 >= q4, q4 >= p4), get_tag())
test_println("p4 >  q4: result = %s, %s (F, T)" % (p4 > q4, q4 > p4), get_tag())

test_println("\np4.is_prefix(q4):  result = %s, %s (T, F)" % (
    p4.is_prefix(q4),  q4.is_prefix(p4)), get_tag())
test_println("p4.is_prefix(p4):  result = %s, %s (T, T)\n" % (
    p4.is_prefix(p4),  q4.is_prefix(q4)), get_tag())

n1918_16 = ipp.from_s("192.169.1.1/24")
test_println("n1918_16.isrfc1918 = %s (F)" % n1918_16.is_rfc1918, get_tag())
r1918_16 = ipp.from_s("192.168.1.1/24")
test_println("r1918_16.isrfc1918 = %s (T)" % r1918_16.is_rfc1918, get_tag())
r1918_12 = ipp.from_s("172.16.1.1/16")
test_println("r1918_12.isrfc1918 = %s (T)" % r1918_12.is_rfc1918, get_tag())
r1918_8 = ipp.from_s("10.1.1.1/16")
test_println("r1918_12.isrfc1918 = %s (T)\n" % r1918_8.is_rfc1918, get_tag())

n1918_20 = ipp.from_s("192.169.1.1/20")
test_println("n1918_20.equal(n1918_16) %s (T)" % n1918_20.equal(n1918_16), get_tag())
test_println("r1918_16.equal(r1918_12) %s (F)\n" % r1918_16.equal(r1918_12), get_tag())

test_println("n1918_20.width = %d (19)\n" % n1918_20.width, get_tag())

test_println("n1918_16.has_bit_set(1) %s (T)" % n1918_16.has_bit_set(1), get_tag())
test_println("n1918_16.has_bit_set(30) %s (F)" % n1918_16.has_bit_set(30), get_tag())
p61 = ipp.from_s("2001:df0:0:321:1:2:3:4/128")
test_println("p61.has_bit_set(125) %s (T)" % p61.has_bit_set(125), get_tag())
test_println("p61.has_bit_set(126) %s (F)\n" % p61.has_bit_set(126), get_tag())

test_println("p4.first_bit_different(q4) = %d (16)" % p4.first_bit_different(q4), get_tag())
p68 = ipp.from_s("2001:df0:0:321:5:2:3:4/128")
test_println("p61.first_bit_different(p68) = %d (77)\n" % p61.first_bit_different(p68), get_tag())

test_println("q4 = %s, complement = %s" % (q4, q4.complement), get_tag())
test_println("p68 = %s, complement = %s\n" % (p68, p68.complement), get_tag())
