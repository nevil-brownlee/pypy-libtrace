
import plt, ipp
import os

# try-ip.py: test program for pypy plt

print "- - - - -"

base = "/Users/jbro111"  # OSX
#base = "/home/nevil"      # Ubuntu

#fn = "pypy/1000packets.pcap.gz"  # vlan id 1051 (Waikato)
#fn = "pypy/anon-v6.pcap"
fn = "pypy/anon-v4.pcap"

trace_format = "pcapfile"

full_fn = base + '/' + fn
print "%s: isfile %s" % (full_fn, os.path.isfile(full_fn))

uri = trace_format + ':' + full_fn
print ">> uri = %s" % uri

t = plt.trace(uri)
t.start()
print "+++ started"

out_fn = base + '/' + "pypy/pylt/plt/test-out.pcap"
out_uri =  trace_format + ':' + out_fn
print ">> out_uri = %s" % out_uri

out_t = plt.output_trace(out_uri)
#out_t.start_output()

def print_all(s):
    for x,b in enumerate(s):
        if x%8 == 0:
            print "",
        print "%02x" % b,
    print
    
def qfmt(fs, v):
    if v:
        return fs % v
    else:
        return str(v)
    
nip = n = 0
for pkt in t:
    n += 1
    if n == 13:  # 1-org
        break
        
    pip = pkt.ip
    if not pip:
        print "pkt %d: not IP" % n
        continue
    nip += 1
    print "--- n=%d ---  cap_len=%d" % (n, pkt.capture_len)

    udp = pkt.udp
    if not udp:
        continue

    plt.Data_dump(udp.pi, None, "udp object")
    uuu = plt.ip(udp)
    plt.Data_dump(uuu.pi, uuu.mom, "New IP object")
    
    break

    
    sa = ip.src_prefix;  da = ip.dst_prefix
    print "sa = %s, da= %s" % (sa, da)
        
#    if ip.version == 4:
#        nsa = ipp.from_s("1.2.3.4");  nda = ipp.from_s("5.6.7.8")
#    else:
#        nsa = ipp.from_s("1:2:3:4::");  nda = ipp.from_s("::5:6:7:8")
#    ip.src_prefix = nsa;  ip.dst_prefix = nda
#    sa = ip.src_prefix;  da = ip.dst_prefix
#    print "sa = %s, da= %s" % (sa, da)

    print "ver=%d, proto=%d, traffic_class=%s, hop_limit=%d" % (
        ip.version, ip.proto, qfmt("%x", ip.traffic_class), ip.hop_limit)
#    ip.traffic_class = 0x34
#    print "ip.traffic_class = %s" % qfmt("%x", ip.traffic_class)

    print "pkt_len=%s, hdr_len=%s" % (
        qfmt("%d",ip.pkt_len), qfmt("%d", ip.hdr_len))

    print "ver=%d, traffic_class=%d, proto=%d" % (
        ip.version, ip.traffic_class, ip.proto)
    if ip.version == 4:
        print "v4: pkt_len=%d, r/d/m flags=%s/%s/%s, frag_offset=%d, ttl=%d" % (
            ip.pkt_len, ip.has_rf, ip.has_df, ip.has_mf, ip.frag_offset, ip.ttl,)
        print "ident=%d, hdr_len=%d, cksm=%04x, checksum_ok=%s" % (
            ip.ident, ip.hdr_len, ip.hdr_checksum, ip.checksum_ok)
    else:
        print "v6: flow_label=%d, payload_len=%d, next_hdr=%d" % (
            ip.flow_label, ip.payload_len, ip.next_hdr)

    print "payload = ",
    print_all(ip.payload)
    print "test_trans_cksm() = %s" % ip.test_trans_cksm()

#    ip.set_l3_cksm()
#    ip.set_trans_cksm(
#    out_t.write_packet(pkt)

    if nip == 1:
        break

print "EOF - - -"

out_t.close_output()
