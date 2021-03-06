# Thu, 13 Mar 14 (PDT)
# icmp.py: Demonstrate ICMP (v4) header decodes
# Copyright (C) 2016, Nevil Brownlee, U Auckland | WAND

from plt_testing import *

t = get_example_trace('icmp-sample.pcap')

n = 0;  nicmp = 0
offset = 12

for pkt in t:
    n += 1

    icmp = pkt.icmp
    if not icmp:
        continue

    print "%5d: " % (n),
    print_icmp(icmp, offset)
    print

    nicmp += 1
    #if nicmp == 10:
    #    break

t.close

print "%d packets examined\n" % (n)
