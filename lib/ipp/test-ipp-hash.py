
import ipp

s = ipp.from_s("130.216.3.10")
print "s = %s" % s
print "version = %d, addr = %s, length = %d\n" % (
    s.version, s.addr, s.length)

iph4 = {}
iph6 = {}

def add_to_iph(pref_s):
    global iph4, iph6
    s = ipp.from_s(pref_s)
    if s.version == 4:
        v = iph4.get(s)
        if v:
            iph4[s] += 1
        else:
            iph4[s] = 1
    else:
        v = iph6.get(s)
        if v:
            iph6[s] += 1
        else:
            iph6[s] = 1
            
add_to_iph("130.216.3.10")
add_to_iph("130.216.3.11")
add_to_iph("130.216.3.10")
add_to_iph("130.216.5.10")
add_to_iph("130.216.5.10")


add_to_iph("130.216.3.10/24")
add_to_iph("130.216.3.11/24")
add_to_iph("130.216.3.10/23")
add_to_iph("130.216.3.11/23")

add_to_iph("2001:df0:0:321:1:2:3:4/128")
add_to_iph("2001:df0:0:abcd::1")
add_to_iph("2001:df0:0:321:1:2:3:4/64")


sk = sorted(iph4)
for dk in sk:
    print("%3d: %s" % (iph4[dk], dk))
print
sk = sorted(iph6)
for dk in sk:
    print("%3d: %s" % (iph6[dk], dk))
print

p32 = ipp.from_s("130.216.38.3")
print("p32 = %s" % p32)
p24 = p32.network(24)
print("p24 = %s" % p24)
p16 = p24.network(16)
print("p16 = %s" % p16)
print("p32=%s, p24=%s, p16=%s\n" % (p32, p24, p16))
