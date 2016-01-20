[test_string:9]  p61 = 2001:df0:0:321:1:2:3:4/128  
[test_string:12] p61 = 2001:df0:0:321:1:2:3:4
[test_string:9]  p62 = 2001:df0:0:0:1:2:3:4/128  
[test_string:12] p62 = 2001:df0::1:2:3:4
[test_string:9]  p63 = 2001:df0:0:0:1:2::  
[test_string:12] p63 = 2001:df0::1:2:0:0
[test_string:9]  p64 = 2001:df0:0:abcd::1  
[test_string:12] p64 = 2001:df0:0:abcd::1
[test_string:9]  p65 = 2001:0:0df0::2  
[test_string:12] p65 = 2001:0:df0::2
[test_string:9]  p66 = 2001::def0::2  
[test_string:22] err = Not a valid IPv4 or IPv6 address
[test_string:9]  p67 = ::ffff:1.2.3.4  
[test_string:12] p67 = ::ffff:1.2.3.4
[test_string:9]  p68 = 1:2:3:0:5:6:7:8  
[test_string:12] p68 = 1:2:3:0:5:6:7:8
[test_string:9]  p69 = 1:2:3::5:6:7:8  
[test_string:12] p69 = 1:2:3:0:5:6:7:8
[test_string:9]  
p41 = 130.216.38.7/24  
[test_string:12] 
p41 = 130.216.38.7/24
[test_string:9]  p42 = 130.256.0.0  
[test_string:22] err = Not a valid IPv4 or IPv6 address
[test_string:9]  p43 = 130.216.0.0/33  
[test_string:22] err = IPv4 length must be <= 32
[test_string:9]  p44 = 130.216.0.0/-1  
[test_string:22] err = Prefix length must be > 0
[test_string:9]  p45 = 130.216/24  
[test_string:22] err = Not a valid IPv4 or IPv6 address
[<module>:41] 
p4 = 130.216.0.0/24, type(p4) = <class 'ipp.IPprefix'>
[<module>:44] q4 = 130.216.128.0/25, type(q4) = <class 'ipp.IPprefix'>

 p4 <  q4: result = True, False (T, F)
[<module>:47] p4 <= q4: result = True, False (T, F)
[<module>:48] p4 == q4: result = False, False (F, F)
[<module>:49] p4 != q4: result = True, True (T, T)
[<module>:50] p4 >= q4: result = False, True (F, T)
[<module>:51] p4 >  q4: result = False, True (F, T)
[<module>:54] 
p4.is_prefix(q4):  result = False, False (T, F)
[<module>:56] p4.is_prefix(p4):  result = True, True (T, T)

[<module>:59] n1918_16.isrfc1918 = False (F)
[<module>:61] r1918_16.isrfc1918 = True (T)
[<module>:63] r1918_12.isrfc1918 = True (T)
[<module>:65] r1918_12.isrfc1918 = True (T)

[<module>:68] n1918_20.equal(n1918_16) True (T)
[<module>:69] r1918_16.equal(r1918_12) False (F)

[<module>:71] n1918_20.width = 19 (19)

[<module>:73] n1918_16.has_bit_set(1) True (T)
[<module>:74] n1918_16.has_bit_set(30) False (F)
[<module>:76] p61.has_bit_set(125) True (T)
[<module>:77] p61.has_bit_set(126) False (F)

[<module>:79] p4.first_bit_different(q4) = 16 (16)
[<module>:81] p61.first_bit_different(p68) = 77 (77)

[<module>:83] q4 = 130.216.128.0/25, complement = 125.39.127.255/25
[<module>:84] p68 = 2001:df0:0:321:5:2:3:4, complement = dffe:f20f:ffff:fcde:fffa:fffd:fffc:fffb

