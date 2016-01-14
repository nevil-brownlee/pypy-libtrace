import string, os

from cipp import lib, ffi

class IPprefix:
    def __init__(*args):  # (self, version, addr, length)
        self = args[0]
        if len(args) >= 2:
            version = args[1]
        if len(args) >= 3:
            addr = args[2]
        if len(args) == 4:
            length = args[3]
        if len(args) > 4:
            raise ValueError("IPprefix expects 1 to 3 arguments")
        if version == 4:
            self.addr = ffi.new("uint8_t[4]", tuple(addr))
        elif version == 6:
            self.addr = ffi.new("uint8_t[16]", tuple(addr))
        else:
            raise ValueError("IPprefix version not 4 or 6")
        self.version = version
        self.length = args[3]  # Prefix length (may be None)

    def __str__(self):
        if not self.addr:
            if not self.length:
                return '0'
            else:
                return '0/%d' % self.length
        else:
            a = ffi.string(lib.ipaddr2str(self.version, self.addr))
            if self.length:
                return a+'/'+str(self.length)
            else:
                return a
        
def from_s(s):
    sa = s.split('/', 2)
    length = None
    if len(sa) == 2:
        try:
            length = int(sa[1])
        except:
            raise ValueError("IPprefix length not an integer")
            print "length = %d" % len  # <<<
        if len < 0:
            raise ValueError("IPprefix length not an integer must be >= 0")

    ver = 4
    if s.find(':') >= 0:
        ver = 6
    if ver == 4:
        ba = ffi.new("uint8_t[4]")
    elif ver == 6:
        ba = ffi.new("uint8_t[16]")

    r = lib.str2ipaddr(ba, ver, sa[0])
    if r:
        return IPprefix(ver, ba, length)
    else:
        raise ValueError("Not a valid IPv4 or IPv6 address")


print "__name__ = %s" % __name__
    
#tpp = ffi.new("struct ip_prefix *")
#tpp.version = 4;  tpp.length = 16;
#tpp.addr = ffi.new("uint8_t[16]", tuple(ba))
#lib.t_ipp(tpp)

ba = bytearray([130, 216, 1, 2])

bp = IPprefix(4, ba, 24)
print "bp = %s (type = %s)" % (bp, bp.__class__.__name__)

print "bp.version=%d, bp.addr=%s, bp.length=%d" % (
    bp.version, bp.addr, bp.length)

    
s = "2001:df0::2006:20e:c6ff:fef7:1f9f/64"
p = from_s(s)
print "p1 = %s" % p
        
s = "2001:df0:0:2006:20e:c6ff:fef7:1f9f/64"
p6 = from_s(s)
print "p6 = %s" % p6

s = "130.213.88.99/23"
p = from_s(s)
print "p3 = %s" % p

s = "130.213.66.77"
q = from_s(s)
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

    
