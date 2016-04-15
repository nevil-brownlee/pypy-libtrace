# icmp.py: Demonstrate UDP header decodes
# Copyright (C) 2016, Nevil Brownlee, U Auckland | WAND

from plt_testing import *
from array import *

#icmp_types = {}  # Empty Dictionary
icmp_array = array('i', [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])

t = get_example_trace('icmp-sample.pcap')

out_uri = 'pcapfile:icmp-out.pcap'
of = plt.output_trace(out_uri)
of.start_output()

n = 0
for pkt in t:
    n += 1

    icmp = pkt.icmp
    if not icmp:
        continue

    it = icmp.type
    icmp_array[it] += 1   

    if icmp_array[it] <= 4:
        of.write_packet(pkt)

t.close();  of.close_output()

print "%d packets examined\n" % (n)

print icmp_array

for j in range(0, 12):
   print "%2d: %6d" % (j, icmp_array[j])
