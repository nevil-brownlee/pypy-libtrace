
import plt
import os

# try-trace.py: test program for pypy plt

print "- - - - -"

print "NO_COMRESSION = %d" % plt.NO_COMPRESSION

base = "/Users/jbro111"  # OSX
#base = "/home/nevil"      # Ubuntu

#fn = "pypy/small-sample.erf"
#fn = "tcp-analyse/fdt-p5.pcap"
#fn = "pypy/small-sample.pcap"
fn = "pypy/1000packets.pcap.gz"  # vlan id 1051 (Waikato)

full_fn = base + '/' + fn

print "%s: isfile %s" % (full_fn, os.path.isfile(full_fn))
try:
    with open(full_fn) as file:
        print "File opened OK"
        file.close()
except IOError as e:
    print "Unable to open file" #Does not exist OR no read permissions

trace_format = "pcapfile"
#trace_format = "erf"

uri = trace_format + ':' + full_fn
print ">> uri = %s" % uri

flt = plt.filter('vlan 1051 and tcp')
print "flt = %s" % flt

t = plt.trace(uri)
t.conf_filter(flt)
t.conf_snaplen(48)
t.start()
print "+++ started"
t.pause()
print "+++ paused"
t.start()
print "+++ started again"


#out_fn = base + '/' + "pypy/pylt/plt/test-out-zlib.pcap"
out_fn = base + '/' + "pypy/pylt/plt/test-out-none.pcap"
out_uri =  trace_format + ':' + out_fn
print ">> out_uri = %s" % out_uri

out_t = plt.output_trace(out_uri)
print "1: out_t = %s" % out_t
#out_t.conf_compress_type(plt.ZLIB_COMPRESSION)
out_t.conf_compress_level(9)
out_t.conf_compress_type(plt.NO_COMPRESSION)
print "2: out_t = %s" % out_t
out_t.start_output()
print "3: out_t = %s" % out_t

test_dict = {}

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

for n,pkt in enumerate(t):
    ip = pkt.ip
    print "--- n=%d ---  cap_len=%d" % (n, pkt.capture_len)

    sa = ip.src_prefix
    da = ip.dst_prefix
    print "sa = %s, da= %s" % (sa, da)


    tcp = pkt.tcp
    fr = pkt.apply_filter(flt)
    print "%5d: fr = %s" % (n, fr)

    out_t.write_packet(pkt)


    if n == 2:  # Zero-org
        break
        
print "EOF - - -"

out_t.close_output()

print "=== %d packet drops" % t.pkt_drops()
print "=== %d packet accepts" % t.pkt_accepts()

t.close()

#exit()

keys = sorted(test_dict)
for k in keys:
    #print "%8d  %s" % (test_dict[k], k)
    ka = k.split('*')
    print "%8d  %s -> %s" % (test_dict[k], 
        plt.ffi.buffer(ka[0]), plt.ffi.buffer(ka[1]))

    
