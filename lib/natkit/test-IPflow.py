#!/usr/bin/env python

# Thu, 13 Mar 14 (PDT)
# ip6.py:  Demonstrate IPv6 objects
# Copyright (C) 2016, Nevil Brownlee, U Auckland | WAND

import plt
import natkit

class flow():
    def __init__(self, ipf, first_n):
        self.ipf = ipf;  self.first_n = first_n
        self.fwd = 1;  self.rev = 0

test_dict = {}

def test_uri(uri):
    t = plt.trace(uri)
    t.start()

    nip = n = 0
    for pkt in t:
        n += 1
        print "n = %4d" % n,
        ipf = natkit.IPflow(pkt)
        if not ipf:
            print
            continue
        nip += 1

        print "  %d %3d  %5d->%5d  %s->%s" % (
           ipf.version, ipf.proto, ipf.src_port, ipf.dst_port,
           ipf.src_prefix, ipf.dst_prefix)

        fwd = ipf.fwd_key
        #print "fwd = %s" % fwd
        v = test_dict.get(fwd)
        if v:
            v.fwd += 1
        else:
            rev = ipf.rev_key
            #print "rev = %s" % rev
            v =test_dict.get(rev)
            if v:
                v.rev += 1
            else:
                test_dict[fwd] = flow(ipf, n)

        #fwd = ipf.fwd_key
        #test_dict[fwd] = flow(ipf, n)

        #rev = ipf.rev_key
        #test_dict[rev] = flow(ipf, n+1000)

        if nip == 10:
            break

    t.close()

    print
    sk = sorted(test_dict)
    for dk in sk:
        #print "dk = >%s<, val[k] = %d" % (dk, test_dict[dk])
        val = test_dict[dk]
        f = val.ipf
        print "      %3d: %d+%d; %d %d  %d->%d  %s->%s" % (
            val.first_n, val.fwd, val.rev,
            f.version, f.proto, f.src_port, f.dst_port,
            f.src_prefix, f.dst_prefix)
        #bp = natkit.ffi.buffer(f)
        #for b in bp:
        #    print "%02x" % ord(b),

pcap_dir = '../..'
#test_uri('pcapfile:'+pcap_dir+'/anon-v4.pcap')

test_uri('pcapfile:'+pcap_dir+'/anon-v6.pcap')

