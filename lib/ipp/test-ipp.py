# 1350, Mon  7 Dec 2015 (NZDT)
#
# test-ipp.py: Test program for the ipp module

import ipp, sys, re


def test_string(name, val):
    print "%s = %s  " % (name, val),
    try:
        prefix = ipp.from_s(val)
        print "%s = %s" % (name, prefix)
        ps = str(prefix)
        if ps.find("::") >= 0:
            sc = ps.replace("::", "!")
            if sc.count(":") == 5:
                nps = sc.replace("!", ":0:");
                print ">>> nps = %s" % nps
            
    except Exception, err:
        #traceback.print_exc()
        print "err = %s" % err

test_string("p61", "2001:df0:0:321:1:2:3:4/128")
test_string("p62", "2001:df0:0:0:1:2:3:4/128")
test_string("p63", "2001:df0:0:0:1:2::")
test_string("p64", "2001:df0:0:abcd::1")
test_string("p65", "2001:0:0df0::2")
test_string("p66", "2001::def0::2")
test_string("p67", "::ffff:1.2.3.4")  # From RFC 5952

test_string("\np41", "130.216.38.7/24")
test_string("p42", "130.256.0.0")
test_string("p43", "130.216.0.0/33")
test_string("p44", "130.216.0.0/-1")
test_string("p45", "130.216/24")

p4 = ipp.from_s("130.216.0.0/24")
print "\np4 = %s, type(p4) = %s" % (p4, type(p4))
ba = bytearray([130, 216, 128, 0])
q4 = ipp.IPprefix(4, ba, 25)
print "q4 = %s, type(q4) = %s\n" % (q4, type(q4))

print "p4 <  q4: result = %s, %s (T, F)" % (p4 < q4, q4 < p4)
print "p4 <= q4: result = %s, %s (T, F)" % (p4 <= q4, q4 <= p4)
print "p4 == q4: result = %s, %s (F, F)" % (p4 == q4, q4 == p4)
print "p4 != q4: result = %s, %s (T, T)" % (p4 != q4, q4 != p4)
print "p4 >= q4: result = %s, %s (F, T)" % (p4 >= q4, q4 >= p4)
print "p4 >  q4: result = %s, %s (F, T)" % (p4 > q4, q4 > p4)

print "\np4.is_prefix(q4):  result = %s, %s (T, F)" % (
    p4.is_prefix(q4),  q4.is_prefix(p4))
print "p4.is_prefix(p4):  result = %s, %s (T, T)\n" % (
    p4.is_prefix(p4),  q4.is_prefix(q4))

n1918_16 = ipp.from_s("192.169.1.1/24")
print "n1918_16.isrfc1918 = %s (F)" % n1918_16.is_rfc1918
r1918_16 = ipp.from_s("192.168.1.1/24")
print "r1918_16.isrfc1918 = %s (T)" % r1918_16.is_rfc1918
r1918_12 = ipp.from_s("172.16.1.1/16")
print "r1918_12.isrfc1918 = %s (T)" % r1918_12.is_rfc1918
r1918_8 = ipp.from_s("10.1.1.1/16")
print "r1918_12.isrfc1918 = %s (T)\n" % r1918_8.is_rfc1918

n1918_20 = ipp.from_s("192.169.1.1/20")
print "n1918_20.equal(n1918_16) %s (T)" % n1918_20.equal(n1918_16)
print "r1918_16.equal(r1918_12) %s (F)\n" % r1918_16.equal(r1918_12)

print "n1918_20.width = %d (19)\n" % n1918_20.width

print "n1918_16.has_bit_set(1) %s (T)" % n1918_16.has_bit_set(1)
print "n1918_16.has_bit_set(30) %s (F)" % n1918_16.has_bit_set(30)
p61 = ipp.from_s("2001:df0:0:321:1:2:3:4/128")
print "p61.has_bit_set(125) %s (T)" % p61.has_bit_set(125)
print "p61.has_bit_set(126) %s (F)\n" % p61.has_bit_set(126)

print "p4.first_bit_different(q4) = %d (16)" % p4.first_bit_different(q4)
p68 = ipp.from_s("2001:df0:0:321:5:2:3:4/128")
print "p61.first_bit_different(p68) = %d (77)\n" % p61.first_bit_different(p68)

print "q4 = %s, complement = %s" % (q4, q4.complement)
print "p68 = %s, complement = %s\n" % (p68, p68.complement)
