
import plt
import os

# try-layers.py: test program for pypy plt

print "- - - - -"

base = "/Users/jbro111"  # OSX
#base = "/home/nevil"      # Ubuntu

#fn = "pypy/small-sample.erf"
#fn = "pypy/small-sample.pcap"
#fn = "tcp-analyse/fdt-p5.pcap"
#fn = "pypy/anon-v4.pcap"
fn = "pypy/1000packets.pcap.gz"  # vlan id 1051 (Waikato)

full_fn = base + '/' + fn
print "%s: isfile %s" % (full_fn, os.path.isfile(full_fn))

trace_format = "pcapfile"
#trace_format = "erf"

uri = trace_format + ':' + full_fn
print ">> uri = %s" % uri

t = plt.trace(uri)
t.start()

def print_all(s):
    for x in range(0,len(s)):
        print "%02x" % s[x],
        if x%32 == 31:
            print
        elif x%8 == 7:
            print "",
    print

for n,pkt in enumerate(t):
    ip = pkt.ip
    print "--- n=%d ---  cap_len=%d" % (n, pkt.capture_len)

    l2 = pkt.layer2
    print_all(l2.data)
    print
    l3 = pkt.layer3
    print_all(l3.data)

    l3.set_checksums()
    
    if n == 0:  # Zero-org
        break
        
print "EOF - - -"
t.close()

