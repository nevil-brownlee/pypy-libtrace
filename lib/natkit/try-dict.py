
import plt, ipp
import os, string
print "Starting try-trace.py: dir() = %s" % dir()

# try-trace.py: test program for pypy plt

print "- - - - -"

print "NO_COMRESSION = %d" % plt.NO_COMPRESSION

#base = "/Users/jbro111"  # OSX
base = "/home/nevil"      # Ubuntu

#fn = "pypy/small-sample.erf"
#fn = "tcp-analyse/fdt-p5.pcap"
#fn = "pypy/small-sample.pcap"
fn = "pypy/1000packets.pcap.gz"

full_fn = base + '/' + fn

print "%s: isfile %s" % (full_fn, os.path.isfile(full_fn))
#try:
#    with open(full_fn) as file:
#        print "File opened OK"
#        file.close()
#except IOError as e:
#    print "Unable to open file" #Does not exist OR no read permissions

trace_format = "pcapfile"
#trace_format = "erf"

uri = trace_format + ':' + full_fn
print ">> uri = %s" % uri
t = plt.trace(uri)
t.start()

test_dict = {}

def print_first(s, n):
    for x in range(n):
        if x%8 == 0:
            print "",
        print "%02x" % ord(s[x]),

for n,pkt in enumerate(t):
    ip = pkt.ip
    print "--- n=%d ---" % n

    print "pkt    linktype %d, ethertype %04x, vlan_id %d" % (
        pkt.linktype, pkt.ethertype, pkt.vlan_id)
    print "ip.seconds = %.6f, ip.ts_sec = %d, ip.time = %s" % (
        ip.seconds, ip.ts_sec, ip.time)
    print "ip.erf_time = %s" % ip.erf_time
    print "ip.wire_len = %s, ip.capture_len = %s, direction = %s" % (
        ip.wire_len, ip.capture_len, ip.direction)

    ba = ip.data
    print "@@ 1 @@ ba = %s" % ba
    print "IP.data:",
    for b in ba:
        print "%02x" % b,  # ba[x],
    print
    
    sa = ip.src_prefix;  da = ip.dst_prefix
    print "*** %s -> %s" % (sa, da)
    print "sa.addr = %s" % sa.addr[0:4]
    for v in sa.addr:
        print "%02x" % v,
    print
    print "- - - - -"
    bsa = bytearray(sa.addr)
    for v in bsa:
        print "%02x" % v,
    print
    print "ba = %s" % plt.ipp.IPprefix(4, bsa)
        # If we import plt, ipp above, we couild say ipp.IPprefix here
    print "= = = = ="

    #exit()
    
    s = str(sa.addr) + '|' + str(da.addr)  # Have to convert to str explicitly
    print "*** %s -> %s  %s" % (sa, da, s)
    print_first(s, 9)
    print "   ident = %04x" % ip.ident

    v = test_dict.get(s)
    if not v:
        test_dict[s] = 1
    else:
        test_dict[s] = v+1

    if n == 0:  # Zero-org
        break
        
print "EOF - - -"


#exit()

def ca2str(cdata_array):
    s = string.join(cdata_array, '')
    n = string.find(s, '\x00')
    return s[0:n]

keys = sorted(test_dict)
for k in keys:
    print "%8d  %s" % (test_dict[k], k)
    ka = k.split('*')
    for j in range(0,4):
        print "%02x" % ord(ka[0][j]),
    print
    psa = ipp.IPprefix(4, bytearray(ka[0]))
    print "psa = %s" % psa
    print "%8d  %s -> %s" % (test_dict[k], 
        ka[0], ka[1])

    
