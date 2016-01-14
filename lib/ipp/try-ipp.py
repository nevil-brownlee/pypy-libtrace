import sys
#print sys.path

import ipp

print "__name__ = %s" % __name__
    
#tpp = ffi.new("struct ip_prefix *")
#tpp.version = 4;  tpp.length = 16;
#tpp.addr = ffi.new("uint8_t[16]", tuple(ba))
#lib.t_ipp(tpp)

ba = bytearray([130, 216, 1, 2])
print "type(ba) = %s" % type(ba)

#nba = ffi.new("uint8_t[4]")
#ffi.memmove(nba, ba, len(nba))
#for v in nba:
#    print "%02x" % v,
#print

#print "bp.version=%d, bp.addr=%s, bp.length=%d" % (
#    bp.version, bp.addr, bp.length)

    
s = "2001:df0::2006:20e:c6ff:fef7:1f9f/64"
p1 = ipp.from_s(s)
print "p1.version=%d, p1.addr=%s, p1.length=%s" % (
    p1.version, p1.addr, p1.length)
print "p1 = %s" % p1
        
bp = ipp.IPprefix(4, ba, 24)
print "bp = %s (type = %s)" % (bp, bp.__class__.__name__)

try:
    xxx = ipp.IPprefix(4, "1.2.3.4")
except:
    pass
print

s = "2001:df0:0:2006:20e:c6ff:fef7:1f9f/64"
p6 = ipp.from_s(s)
print "p6 = %s" % p6

s = "130.213.88.99/23"
p = ipp.from_s(s)
print "p3 = %s" % p

s = "130.213.66.77"
q = ipp.from_s(s)
print "p4.version=%d, p4.addr=%s, p4.length=%s" % (
    q.version, q.addr, q.length)
print "p4 = %s" % q

try:
    s = "130.213.88.99/-2"
except:
    pass

for j in range(4):
    print " %d/%s" % (p.addr[j], type(p.addr[j])),
print

test_dictionary = {}
test_dictionary[p.addr] = (1, p)
test_dictionary[q.addr] = (2, q)
test_dictionary[p6.addr] = (3, p6)

for k,v in test_dictionary.items():
    print "%3d  %s" % (v[0], v[1])


print "- - - - -"
