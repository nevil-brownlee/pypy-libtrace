
import ipp, plt
import os, string
print "Starting try-trace.py: dir() = %s" % dir()

# try-trace.py: test program for pypy plt

print "- - - - -"

print "NO_COMRESSION = %d" % plt.NO_COMPRESSION

base = "/Users/jbro111/pypy-libtrace/test/pypy-test-cases"  # OSX
#base = "/home/nevil/pypy-libtrace/test"      # Ubuntu

#fn = "small-sample.erf"
#fn = "fdt-p5.pcap"
#fn = "small-sample.pcap"
#fn = "1000packets.pcap.gz"
fn = "anon-v4.pcap"

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
td_2 = {}

def print_first(s):
    for x in range(16):
        if x%8 == 0:
            print "",
        print "%02x" % s[x],

def print_last(s, rem):
    x = (rem-8)/8*8
    while x < rem:
        if x%8 == 0:
            print "",
        print "%02x" % s[x],
        x += 1

n = 0
for pkt in t:
    n += 1
    ip = pkt.ip
    print "--- n=%d ---" % n

    print "pkt    linktype %d, ethertype %04x, vlan_id %d" % (
        pkt.linktype, pkt.ethertype, pkt.vlan_id)
    print "pkt.seconds = %.6f, pkt.ts_sec = %d, pkt.time = %s" % (
        pkt.seconds, pkt.ts_sec, pkt.time)
    print "pkt.erf_time = %s" % pkt.erf_time
    print "pkt.wire_len = %s, pkt.capture_len = %s, direction = %s" % (
        pkt.wire_len, pkt.capture_len, pkt.direction)

    if not ip:
        continue
    
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
    
    s = str(sa.addr) + '*' + str(da.addr)  # Have to convert to str explicitly
    print "*** %s -> %s  %s" % (sa, da, s)
    print "   ident = %04x" % ip.ident

    l2 = pkt.transport
    plt.Data_dump(l2.pi, "init _pkt _transport_obj", None)

    tcp = pkt.tcp
    if tcp:
        print "TCP: src_port = %u, dst_port = %u" % (tcp.src_port, tcp.dst_port)
        ba = tcp.data
        print "@@ 2 @@ ba = %s" % ba
        print "tcp.data:",
        for b in ba:
            print "%02x" % b,  # ba[x],
        print
        pp = plt.cp2ba(ba)
        #pp =plt.ffi.buffer(ba)
        print "ba = %s, pp = %s" % (ba, pp)
        print "str(pp) = %s" % str(pp)

        s = "abc: " + str(pp) + "xyz"
        print "@@@ s = %s" % s
        
    udp = pkt.udp
    if udp:
        print "UDP: src_port = %u, dst_port = %u" % (udp.src_port, udp.dst_port)
        s = "udp"
        
    v = test_dict.get(s)
    if not v:
        test_dict[s] = 1
    else:
        test_dict[s] = v+1

    if n == 10:  # Zero-org
        break
        
print "EOF - - -"


exit()

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
    print "len(ka)=%d, ka[0]=%s" % (len(ka), ka[0])
    #psa = ipp.IPprefix(4, ka[0])
    #print "psa = %s" % psa
    #print "%8d  %s -> %s" % (test_dict[k], 
    #    ka[0], ka[1])

    
