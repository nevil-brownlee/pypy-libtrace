[get_example_trace:23]fn = tunnel.pcap

[print_data:32]  <d>     60 00 00 00 04 d8 06 80   20 01 05 c0 10 00 00 0a   80 00 00 00 82 d8 26 2d   24 04 68 00 40 06 08 06 
[print_data:37]        00 00 00 00 00 00 10 16   db c2 01 bb 88 86 a5 3c   42 7b 34 36 50 10 01 01   56 71 00 00 17 03 03 0e 
[print_data:37]        72 00 00 00 00 00 00 00   03 06 77 8d 97 03 cd 24   37 d4 13 1f 42 cd 86 c8   86 96 ca b9 fa 1f 16 6e 
[print_data:37]        7c f0 49 5c 31 70 b1 4e   58 9c d0 eb 0d ca cb 7e   de 03 6e 98 50 57 8b 6f
[<module>:24]     1: 
[<module>:25:{n:1}][print_ip6:54] 2001:5c0:1000:a:8000:0:82d8:262d -> 2404:6800:4006:806::1016, proto=6, tclass=0,
[<module>:25:{n:1}][print_ip6:56]             ttl=128, hlen=None, plen=None flow_label=0, payload_len=1240, next_hdr=6
[<module>:30:{n:1}][print_tcp:75] TCP, 2001:5c0:1000:a:8000:0:82d8:262d -> 2404:6800:4006:806::1016, 56258 -> 443, seq=2290525500, ack=1115370550
[<module>:30:{n:1}][print_tcp:78]          flags=10 (A), window=257, checksum=5671, urg_ptr=0
[<module>:30:{n:1}][print_tcp:85][print_data:32]  
[<module>:30:{n:1}][print_tcp:85]          payload: 17 03 03 0e 72 00 00 00   00 00 00 00 03 06 77 8d   97 03 cd 24 37 d4 13 1f   42 cd 86 c8 86 96 ca b9 
[<module>:30:{n:1}][print_tcp:85][print_data:37]              fa 1f 16 6e 7c f0 49 5c   31 70 b1 4e 58 9c d0 eb   0d ca cb 7e de 03 6e 98   50 57 8b 6f 44 f5 16 dd
[print_data:32]  <d>     60 00 00 00 04 d8 06 80   20 01 05 c0 10 00 00 0a   80 00 00 00 82 d8 26 2d   24 04 68 00 40 06 08 06 
[print_data:37]        00 00 00 00 00 00 10 16   db c2 01 bb 88 86 aa 00   42 7b 34 36 50 10 01 01   0e fe 00 00 6d 0f 47 4a 
[print_data:37]        42 af 66 72 74 dd 56 d9   54 60 30 3a 32 98 f4 f5   0e 32 93 cb 9c 11 6a 40   bf b9 b1 77 1e 51 d8 c1 
[print_data:37]        50 99 d3 96 d2 03 c7 67   89 14 81 63 3e 0e 76 fe   15 8b e9 d8 f8 be 72 35
[<module>:24]     2: 
[<module>:25:{n:2}][print_ip6:54] 2001:5c0:1000:a:8000:0:82d8:262d -> 2404:6800:4006:806::1016, proto=6, tclass=0,
[<module>:25:{n:2}][print_ip6:56]             ttl=128, hlen=None, plen=None flow_label=0, payload_len=1240, next_hdr=6
[<module>:30:{n:2}][print_tcp:75] TCP, 2001:5c0:1000:a:8000:0:82d8:262d -> 2404:6800:4006:806::1016, 56258 -> 443, seq=2290526720, ack=1115370550
[<module>:30:{n:2}][print_tcp:78]          flags=10 (A), window=257, checksum=efe, urg_ptr=0
[<module>:30:{n:2}][print_tcp:85][print_data:32]  
[<module>:30:{n:2}][print_tcp:85]          payload: 6d 0f 47 4a 42 af 66 72   74 dd 56 d9 54 60 30 3a   32 98 f4 f5 0e 32 93 cb   9c 11 6a 40 bf b9 b1 77 
[<module>:30:{n:2}][print_tcp:85][print_data:37]              1e 51 d8 c1 50 99 d3 96   d2 03 c7 67 89 14 81 63   3e 0e 76 fe 15 8b e9 d8   f8 be 72 35 e9 cf 8a 8e
