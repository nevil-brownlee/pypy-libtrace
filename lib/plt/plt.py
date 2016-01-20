# 1249, Wed 20 Jan 2016 (NZDT)
#
# plt.py:  plt objects iplemented in python
#
# Copyright (C) 2016 by Nevil Brownlee, U Auckland | WAND
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from cplt import ffi, lib
import ipp
import datetime, string

PLTversion    = "2.0"  # 23 Nov 15 (NZDT)

KIND_PKT      =  1  # data points to a libtrace_packet_t
KIND_STR      =  2  # data points to a C byte array
KIND_CPY      =  3  # data points to another plt_obj (MOM)
def bad_kind(v):
    return v < KIND_PKT or v > KIND_COPY

kind_strings = {KIND_PKT:"packet", KIND_STR:"String", KIND_CPY:"copy"}

def plt_kind(k):
    v = kind_strings.get(k)
    if v:
        return v
    return "unknown"

TYPE_DATA     =  5  # rlt_obj data types
TYPE_PKT      = 10
TYPE_L2       = 20
TYPE_L3       = 30
TYPE_Internet = 31
TYPE_IP       = 32
TYPE_IP6      = 33
TYPE_L4       = 40
TYPE_TCP      = 41
TYPE_UDP      = 42
TYPE_ICMP     = 43
TYPE_ICMP6    = 44
TYPE_L5       = 50

type_strings = {TYPE_PKT:"Packet", TYPE_DATA:"Data", TYPE_L2:"Layer2",
    TYPE_L3:"Layer3", TYPE_Internet:"Internet", TYPE_IP:"IP", TYPE_IP6:"IP6",
    TYPE_L4:"Transport", TYPE_TCP:"TCP", TYPE_UDP:"UDP",
    TYPE_L5:"Payload", TYPE_ICMP:"ICMP", TYPE_ICMP6:"ICMP6"}

def plt_type(t):
    v = type_strings.get(t)
    if v:
        return v
    return "unknown"

def Data_dump(pi, mom, msg):
    print("%s:" % msg)
    print("    type=%s, kind=%s,(data=%x)" % (  # not using mom, was here
        plt_type(pi.o_type), plt_kind(pi.o_kind),
        ffi.cast("uintptr_t", pi.data)))
    print("    l2p=%x, %d, %d, %04x, %04x" % (
        ffi.cast("uintptr_t", pi.l2p), pi.l2_rem, pi.linktype,
        pi.ethertype, pi.vlan_tag))
    print("    l3p=%x, %d,  %d, dp=%x, %d" % (
        ffi.cast("uintptr_t", pi.l3p), pi.l3_rem,
        pi.proto, ffi.cast("uintptr_t", pi.dp), pi.rem))
    print("    mom=%s" % mom)

def new_pi(type, kind, data,  l3p, l3_rem,  proto, dp, rem):
    pi =  ffi.new("struct pi *")
    pi.o_type = type;  pi.o_kind = kind;  pi.data = data
        # Don't remember l2 info
    pi.l3p = l3p;
    pi.l3_rem = l3_rem;
    pi.proto = proto
    pi.dp = dp;   pi.rem = rem
    return pi

def mk_data(pi, hlen):  # Make a .mom for a pi
    if isinstance(pi, ffi.CData):
        data = ffi.new("uint8_t[]", pi.rem-hlen)
        ffi.memmove(data, pi.dp[hlen:pi.rem], pi.rem-hlen)
    elif isinstance(obj, bytearray):
        data = ffi.new("uint8_t[]", tuple(pi))
    else:
        raise TypeError("Expected 'cdata uint8_t[]' or bytearray!")
    return data

def mk_new_obj(type_in, data_in):  # Make pi and mom for a new object
    if isinstance(data_in, ffi.CData):
        mom = data_in;  in_len = ffi.sizeof(data_in)
    elif isinstance(data_in, bytearray):
        mom = ffi.new("uint8_t[]", tuple(data_in));  in_len = len(data_in)
    elif str(type(data_in))[-6:-2] == '_obj':
        pi_in = data_in.pi
        if pi_in.o_type < TYPE_L3 or pi_in.o_type >= TYPE_L4:
            raise PltError("Expected a layer 3 plt object")
        # Data_dump(data_in.pi, data_in.mom, "mk_new_obj(): data_in object")
        if pi_in.l3_rem != 0:  # We have l3 data
            pi_new = ffi.new("struct pi *")
            if lib.get_trans_payload(pi_new, data_in.pi):
                # Data_dump(pi_new, data_in.mom, "pi_new")
                return pi_new, data_in.mom
            raise PltError("Layer 3 plt object has no payload")
        raise PltError("Expected a plt object with layer 3 data")
    else:
        raise TypeError("Expected 'cdata uint8_t[]' or bytearray!")
    pi = new_pi(type_in, KIND_CPY, mom,  # type, kind, data
        mom, in_len,  0,  mom, in_len)  # l3p, l3_rem,  proto,  dp, rem
    return pi, mom
    

def sub_trans_obj(pi, hlen):
    l5_data = mk_data(pi, hlen)
    pi = new_pi(pi.o_type, pi.o_kind, pi.data,
        pi.l3p, pi.l3_rem,  1, pi.dp[hlen:pi.rem], pi.rem - hlen)
    return (pi, l5_data)

def print_cdata(s, len):
    for x in range(0,len):
        print "%02x" % s[x],
        if x%32 == 31:
            print
        elif x%8 == 7:
            print "",
    print


class LibtraceError(Exception):  # Create new Exception
    pass

class PltError(Exception):
    pass


OUT_FILE_FLAGS = lib.OUT_FILE_FLAGS 

OUT_COMP_TYPE = lib.OUT_COMP_TYPE
NO_COMPRESSION   = lib.NO_COMPRESSION
ZLIB_COMPRESSION = lib.ZLIB_COMPRESSION
ZIP2_COMPRESSION = lib.ZIP2_COMPRESSION
LZO_COMPRESSION  = lib.LZO_COMPRESSION

OUT_COMP_LEVEL = lib.OUT_COMP_LEVEL

OPT_SNAPLEN = lib.OPT_SNAPLEN
OPT_PROMISC = lib.OPT_PROMISC

TRACE_ETH = lib.TRACE_ETH  # libtrace trace types
TRACE_ATM = lib.TRACE_ATM
TRACE_80211 = lib.TRACE_80211
TRACE_NONE = lib.TRACE_NONE
TRACE_LINUX_SLL = lib.TRACE_LINUX_SLL
TRACE_PFLOG = lib.TRACE_PFLOG
TRACE_DUCK = lib.TRACE_DUCK
TRACE_80211_RADIO = lib.TRACE_80211_RADIO
TRACE_LLCSNAP = lib.TRACE_LLCSNAP
TRACE_PPP = lib.TRACE_PPP
TRACE_METADATA = lib.TRACE_METADATA

TRACE_DIR_OUTGOING = lib.DIR_OUTGOING  # libtrace packet directions
TRACE_DIR_INCOMING = lib.DIR_INCOMING
TRACE_DIR_OTHER = lib.DIR_OTHER
TRACE_DIR_UNKNOWN = lib.DIR_UNKNOWN


def get_short(ca, x):  # Get 16-bit integer from cdata uint8_t[]
    return lib.get_short(ca, x)
def set_short(ca, bx, x):  # Set 16-bit integer into cdata uint8_t[]
    lib.set_short(ca, bx, x)
def get_long(ca, x):  # Get 32-bit integer from cdata uint8_t[]
    return lib.get_long(ca, x)
def set_long(ca, bx, x):  # Set 32-bit integer into cdata uint8_t[]
    lib.set_long(ca, bx, x)
def set_bytes(ca, sa, nbytes):  # Copy bytes into a cdata uint8_t[]
    lib.set_bytes(ca, sa, nbytes)

def ca2str(ca):  # Convert cdata char array to string
    return ffi.string(ca)  # Up to the first null

def output_trace(uri):
    return _output_trace(uri)
        
class _output_trace:
    def __init__(self, uri):
        self.uri = uri
        self.lt_out = lib.trace_create_output(uri)  # Create trace
        self.started = False

    def __str__(self):
        ready = 'not '
        if self.started:
            ready = ''
        return "<Output trace: %s, %sstarted>" % (self.uri, ready)

    def conf_file_flags(self, file_flags):
        if lib.set_config_output(self.lt_out, OUT_FILE_FLAGS, file_flags) < 0:
            raise PltError("Couldn't set output file flags = %d" % file_flags)
        return None

    def conf_compress_type(self, comp_t):
        if lib.set_config_output(self.lt_out, OUT_COMP_TYPE, comp_t) < 0:
            raise PltError("Couldn't set output compress type %d" % comp_t)
        return None

    def conf_compress_level(self, level_v):
        if lib.set_config_output(self.lt_out, OUT_COMP_LEVEL, level_v) < 0:
            raise PltError("Couldn't set output compress level %d" % level_v)
        return None
    
    def start_output(self):
        if self.started:
            raise PltError("Trace already started")
        r = lib.trace_start_output(self.lt_out)
        if r < 0:
            trace_err = lib.trace_get_err_output(self.lt_out)
            raise LibtraceError(ca2str(trace_err.problem))
        self.started = True
        
    def close_output(self):
        if self.started:
            lib.trace_destroy_output(self.lt_out)
        return None

    def write_packet(self, out_pkt):
        if not self.started:
            raise PltError("Output trace not started")
        r = lib.trace_write_packet(self.lt_out, out_pkt.p)
        if r <= 0:  # libtrace error
            trace_err = lib.trace_get_err_output(self.lt_out)
            raise LibtraceError(ca2str(trace_err.problem))
        return True
        

def filter(fstring):
    return _filter(fstring)
    
class _filter:
    def __init__(self, filterstring):
        self.fstring = filterstring
        self.filter = lib.trace_create_filter(filterstring)
        self.used = False
        
    def __str__(self):
        ready = 'not '
        if self.used:
            ready = ''
        return "<Filter: %s, %sused>" % (self.fstring, ready)

def trace(uri):
    return _trace(uri)
        
class _trace:
    def __init__(self, uri):
        self.uri = uri
        self.ltrace = lib.trace_create(uri)  # Create trace
        self.pkt = _pkt_obj()
        self.started = False

    def __str__(self):
        ready = 'not '
        if self.started:
            ready = ''
        return "<Trace: %s, %sstarted>" % (self.uri, ready)

    def conf_filter(self, flt):
        if lib.set_config_filter(self.ltrace, flt.filter) < 0:
            raise PltError("Couldn't set trace filter %s" % flt.fstring)
        return None

    def conf_snaplen(self, len):
        if lib.set_config(self.ltrace, OPT_SNAPLEN, len) < 0:
            raise PltError("Couldn't set snaplen %d" % len)
        return None

    def conf_promisc(self, prom_v):
        if lib.set_config(self.ltrace, OPT_PROMISC, prom_v) < 0:
            raise PltError("Couldn't set trace promiscuous %d" % prom_v)
        return None

    def start(self):
        if self.started:
            raise PltError("Trace already started")
        r = lib.trace_start(self.ltrace)
        if r < 0:
            trace_err = lib.trace_get_err(self.ltrace)
            raise PltError(ca2str(trace_err.problem))
        self.started = True

    def pause(self):
        if not self.started:
            raise PltError("Trace not started")
        if lib.trace_pause(self.ltrace) != 0:
            raise PltError("Couldn't pause trace %s" % self.uri)
        self.started = False
        return None

    def close(self):
        if self.started:
            lib.trace_destroy(self.ltrace)
        return None

    def read_packet(self, pkt):
        if not self.started:
            raise PltError("Trace not started")
        r = lib.trace_read_packet(self.ltrace, pkt.p)
        if r == 0:  # EOF
            return False
        elif r < 0:  # libtrace error
            trace_err = lib.trace_get_err(self.ltrace)
            raise LibtraceError(ca2str(trace_err.problem))
        r = lib.get_pkt_info(pkt.pi, pkt.p)
        if r < 0:
            raise LibtraceError("get_pkt_info(): error %d" % -r)
        pkt.pi.o_type = TYPE_PKT;  pkt.pi.o_kind = KIND_PKT
        # Data_dump(pkt.pi, None, "read_packet")
        return True
        
    def __iter__(self):
        while True:  # Read packets from the trace
            r = self.read_packet(self.pkt)
            if not r:  # EOF
                break
            yield self.pkt

    def pkt_drops(self):
        return lib.trace_get_dropped_packets(self.ltrace)

    def pkt_accepts(self):
        return lib.trace_get_accepted_packets(self.ltrace)


def cp2ba(cp):  # Make bytearray slice from cdata pointer
    # Note: this returns a pointer to an existing cdata object
    #   Use str() to make a new copy of its bytes!
    return ffi.buffer(cp)

def packet():
    return _pkt_obj()

class _pkt_obj(object): # New-style class, child of object
    def __init__(self):
        self.p = lib.trace_create_packet()  # Instance variables
        self.pi = ffi.new("struct pi *")
        self.mom = self
        # Data_dump(self.pi, self.mom, "init _pkt_obj")

    def apply_filter(self, fltr):
        if not isinstance(fltr, _filter):
            raise PltError("Expected a Filter object")
        self.used = True
        return lib.trace_apply_filter(fltr.filter, self.p) > 0

    def check_pkt(self):
        if self.pi.o_kind != KIND_PKT:
            raise PltError("Object didn't come from a plt Packet")
                
    def get_layer2(self):
        self.check_pkt()
        self.pi.o_type = TYPE_L2
        self.pi.dp = self.pi.l2p;  self.pi.rem = self.pi.l2_rem
        return _layer2_obj(self.pi, self.mom)
    layer2 = property(get_layer2)

    def get_layer3(self):
        self.check_pkt()
        self.pi.o_type = TYPE_L3
        self.pi.dp = self.pi.l3p;  self.pi.rem = self.pi.l3_rem
        return _layer3_obj(self.pi, self.mom)
    layer3 = property(get_layer3)

    def get_transport(self):
        self.check_pkt()
        new_pi = ffi.new("struct pi *")
        if lib.get_transport_info(new_pi, self.pi):
            new_pi.o_type = TYPE_L4;  new_pi.o_kind = KIND_PKT
            return _transport_obj(new_pi, self.mom)
    transport = property(get_transport)

    def get_ip(self):
        self.check_pkt()
        if self.pi.ethertype == 0x0800:
             return _ip_obj(self.pi, self.mom)
        elif self.pi.ethertype == 0x86DD:
             return _ip6_obj(self.pi, self.mom)
        return None  # Not IP or IP6
    ip = property(get_ip)

    def get_ip4(self):
        self.check_pkt()
        if self.pi.ethertype == 0x0800:
             return _ip_obj(self.pi, self.mom)
        return None  # Not IP
    ip4 = property(get_ip4)

    def get_ip6(self):
        self.check_pkt()
        if self.pi.ethertype == 0x86DD:
             return _ip6_obj(self.pi, self.mom)
        return None  # Not IP6
    ip6 = property(get_ip6)

    def get_tcp(self):
        self.check_pkt()
        new_pi = ffi.new("struct pi *")
        if lib.get_trans_payload(new_pi, self.pi):
            if new_pi.proto == 6:
                return _tcp_obj(new_pi, self.mom)
        return None
    tcp = property(get_tcp)

    def get_tcp_payload(self):
        self.check_pkt()
        new_pi = ffi.new("struct pi *")
        if lib.get_trans_payload(new_pi, self.pi):
            if new_pi.proto == 6:
                return _tcp_obj(new_pi, self.mom).payload
        return None
    tcp_payload = property(get_tcp_payload)

    def get_udp(self):
        self.check_pkt()
        new_pi = ffi.new("struct pi *")
        if lib.get_trans_payload(new_pi, self.pi):
            if new_pi.proto == 17:
                return _udp_obj(new_pi, self.mom)
        return None
    udp = property(get_udp)

    def get_udp_payload(self):
        self.check_pkt()
        new_pi = ffi.new("struct pi *")
        if lib.get_trans_payload(new_pi, self.pi):
            if new_pi.proto == 17:
                return _udp_obj(new_pi, self.mom).payload
        return None
    udp_payload = property(get_udp_payload)

    def get_icmp(self):
        self.check_pkt()
        if self.pi.ethertype == 0x0800:
            # Data_dump(self.pi, self.mom, "starting get_icmp")
            new_pi = ffi.new("struct pi *")
            if lib.get_trans_payload(new_pi, self.pi):
                # Data_dump(new_pi, self.mom, "get_icmp  new_pi")
                if new_pi.proto == 1:
                    return _icmp_obj(new_pi, self.mom)
        return None
    icmp = property(get_icmp)

    def get_icmp6(self):
        self.check_pkt()
        if self.pi.ethertype == 0x86DD:
            new_pi = ffi.new("struct pi *")
            if lib.get_trans_payload(new_pi, self.pi):
                if new_pi.proto == 58:
                    return _icmp6_obj(new_pi, self.mom)
        return None
    icmp6 = property(get_icmp6)

    # Instance attributes ...
    
    def get_data(self):  # A plt object's data is *always* (pi.dp, pi.rem)
        if self.pi.o_kind == KIND_PKT or self.pi.o_kind == KIND_CPY:
            return self.pi.dp[0:self.pi.rem]
        raise PltError(".data kind not PKT or CPY!")
    def set_data(self, new_data):
        if self.pi.o_kind != KIND_PKT and self.pi.o_kind != KIND_CPY:
            raise PltError(".data kind not PKT or CPY!")
        if len(new_data) != self.pi.rem:
            raise PltError("New data not same number of bytes as old data")
        ffi.memmove(self.pi.dp[0:self.pi.rem], new_data, len(new_data))
        return None
    data = property(get_data, set_data)

    def get_time(self):
        self.check_pkt()
        return datetime.datetime.fromtimestamp(lib.get_time(self.pi))
    time = property(get_time)

    def get_seconds(self):
        self.check_pkt()
        return lib.get_time(self.pi)
    seconds = property(get_seconds)

    def get_ts_sec(self):
        self.check_pkt()
        return lib.get_time(self.pi)
    ts_sec = property(get_ts_sec)

    def get_erf_time(self):
        self.check_pkt()
        return lib.get_erf_time(self.pi)
    erf_time = property(get_erf_time)

    def get_wire_len(self):  # May include FCS (e.g. for erf: traces)
        self.check_pkt()
        return lib.get_wire_len(self.pi)
    wire_len = property(get_wire_len)

    def get_capture_len(self):
        self.check_pkt()
        return lib.get_capture_len(self.pi)
    capture_len = property(get_capture_len)

    def get_ethertype(self):
        self.check_pkt()
        return self.pi.ethertype
    ethertype = property(get_ethertype)

    def vlan_id(self):
        self.check_pkt()
        return self.pi.vlan_tag
    vlan_id = property(vlan_id)

    def get_linktype(self):
        self.check_pkt()
        return self.pi.linktype
    linktype = property(get_linktype)

    def get_kind(self):  # plt object kind
        self.check_pkt()
        return plt_kind(self.pi.o_kind)
    kind = property(get_kind)

    def get_type(self):  # plt object type
        self.check_pkt()
        return plt_type(self.pi.o_type)
    type = property(get_type)

    def get_size(self):  # plt object rem bytes
        self.check_pkt()
        return self.pi.rem
    size = property(get_size)

    def get_direction(self):  # libtrace direction (for erf: traces)
        self.check_pkt()
        return lib.trace_get_direction(self.p)
    direction = property(get_direction)


class _layer2_obj(_pkt_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        # Data_dump(self.pi, self.mom, "init _layer2_obj")

    def __len__(self):
        return self.pi.l2_rem
    #len = property(get_len)
        

class _layer3_obj(_pkt_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        # Data_dump(self.pi, self.mom, "init _layer3_obj")

    def set_checksums(self):
        if self.pi.o_kind != KIND_PKT:
            raise PltError("Object didn't come from a plt Packet")
        if self.pi.o_type < TYPE_L3:
            raise PltError("Expected a Layer 3 object")
        if lib.l3_checksum(self.pi, 1) < 0:
            return None
        if lib.transport_checksum(self.pi, 1) < 0:
            return None
        return True  #  Both checksums set
 

class _inet_obj(_pkt_obj):
    def check_inet(self, x4, x6):
        version = self.pi.l3p[0] >> 4
        if version == 4 and self.pi.l3_rem >= x4:
            return 4
        if version == 6 and self.pi.l3_rem >= x6:
            return 6
        return 0  # Not enough bytes

    def get_version(self):
        version = self.check_inet(1, 1)
        if version != 0:
            return self.pi.l3p[0] >> 4
        return None
    version = property(get_version)

    def get_proto(self):
        version = self.check_inet(10, 11)
        if version != 0:
            new_pi = ffi.new("struct pi *")
            r = lib.get_transport_info(new_pi, self.pi)
            return new_pi.proto
        return None
    proto = property(get_proto)

    def get_traffic_class(self):
        version = self.check_inet(2, 2)
        if version == 4:
            return self.pi.l3p[1]
        elif version == 6:
            return (lib.get_short(self.pi.l3p, 0) >> 4) & 0x00ff
        return None
    def set_traffic_class(self, new_val):
        version = self.check_inet(2, 2)
        if version == 4:
            self.pi.l3p[1] = new_val
        elif version == 6:
            val = lib.get_short(self.pi.l3p, 0)
            lib.set_short(self.pi.l3p, 0, (val & 0xF00F) | (new_val << 4))
        else:
            raise PltError("Data too short to set traffic_class")
        return None
    traffic_class = property(get_traffic_class, set_traffic_class)

    def get_hop_limit(self):
        version = self.check_inet(9, 8)
        if version == 4:
            return self.pi.l3p[8]
        elif version == 6:
            return self.pi.l3p[7]
        return None
    def set_hop_limit(self, new_val):
        version = self.check_inet(9, 8)
        if version == 4:
            self.pi.l3p[9] = new_val
        elif version == 6:
            self.pi.l3p[8] = new_val
        else:
            raise PltError("Data too short to set hop_limit")
        return None
    hop_limit = property(get_hop_limit, set_hop_limit)

    def get_pkt_len(self):
        version = self.check_inet(4, 4)
        if version == 4:
            return lib.get_short(self.pi.l3p, 2)
        return None
    pkt_len = property(get_pkt_len)

    def get_hdr_len(self):
        version = self.check_inet(1, 1)
        if version == 4:
            return (self.pi.l3p[0] & 0x0F)
        return None
    hdr_len = property(get_hdr_len)
    
    def get_src_prefix(self):
        version = self.check_inet(16, 24)
        if version == 4:
            return ipp.IPprefix(4, self.pi.l3p[12:16])
        elif version == 6:
            return ipp.IPprefix(6, self.pi.l3p[8:24])
        return None
    def set_src_prefix(self, new_sa):
        version = self.check_inet(16, 24)
        if version == 4:
            set_bytes(self.pi.l3p[12:16], new_sa.i_addr, 4)
        elif version == 6:
            set_bytes(self.pi.l3p[8:24], new_sa.i_addr, 16)
        else:
            raise PltError("Data too short to set src_prefix")
        return None
    src_prefix = property(get_src_prefix, set_src_prefix)
        
    def get_dst_prefix(self):
        version = self.check_inet(20, 40)
        if version == 4:
            return ipp.IPprefix(4, self.pi.l3p[16:20])
        elif version == 6:
            return ipp.IPprefix(6, self.pi.l3p[24:40])
        return None
    def set_dst_prefix(self, new_da):
        version = self.check_inet(16, 24)
        if version == 4:
            set_bytes(self.pi.l3p[16:20], new_da.i_addr, 4)
        elif version == 6:
            set_bytes(self.pi.l3p[24:40], new_da.i_addr, 16)
        else:
            raise PltError("Data too short to set dst_prefix")
        return None
    dst_prefix = property(get_dst_prefix, set_dst_prefix)

    def check_pkt(self):
        if self.pi.o_kind != KIND_PKT:
            raise PltError("Object didn't come from a plt Packet")
        if self.pi.o_type < TYPE_L3:
            raise PltError("Expected a Layer 3 object")
                
    def test_l3_cksm(self):
        self.check_pkt()
        r = lib.l3_checksum(self.pi, 0)
        if r > 0:
            return True
        if r == 0:
            return False
        return None

    def set_l3_cksm(self):
        self.check_pkt()
        if lib.l3_checksum(self.pi, 1) < 0:
            raise PltError("Data too short to set l3_cksm")
        return None
    
    def test_trans_cksm(self):
        self.check_pkt()
        r = lib.transport_checksum(self.pi, 0)
        if r > 0:
            return True
        if r == 0:
            return False
        return None

    def set_trans_cksm(self):
        self.check_pkt()
        if lib.transport_checksum(self.pi, 1) < 0:
            raise ValueError("Data too short to set l3_cksm")
        return None


class _ip_obj(_inet_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        self.pi.o_type = TYPE_IP
        # Data_dump(self.pi, self.mom, "init _ip_obj")

    def check_ip(self, x4):
        if self.pi.o_type != TYPE_IP:
            raise PltError("Expected an IPv4 object")
        return self.pi.l3_rem >= x4
            
    def get_ident(self):
        if self.check_ip(6):
            return lib.get_short(self.pi.l3p, 4)
        return None
    ident = property(get_ident)
        
    def get_has_rf(self):
        if self.check_ip(7):
            return bool(self.pi.l3p[6] & 0x80)
        return None
    has_rf = property(get_has_rf)
            
    def get_has_df(self):
        if self.check_ip(6):
            return bool(self.pi.l3p[6] & 0x40)
        return None
    has_df = property(get_has_df)
            
    def get_has_mf(self):
        if self.check_ip(6):
            return bool(self.pi.l3p[6] & 0x20)
        return None
    has_mf = property(get_has_mf)
            
    def get_frag_offset(self):
        if self.check_ip(8):
            return lib.get_short(self.pi.l3p, 6) & 0x1FFF
        return None
    frag_offset = property(get_frag_offset)
            
    def get_ttl(self):
        if self.check_ip(9):
            return self.pi.l3p[8]
        return None
    ttl = property(get_ttl)
            
    def get_checksum_ok(self):
        if self.check_ip(12):
            return bool(lib.l3_checksum(self.pi, 0))
            return r
        return None
    checksum_ok = property(get_checksum_ok)
            
    def get_checksum(self):
        if self.check_ip(12):
            return lib.get_short(self.pi.l3p, 10)
        return None
    def set_checksum(self, cks_v):
        if self.pi.o_kind != KIND_PKT:
            raise PltError("Object didn't come from a plt Packet")
        if not self.check_ip(12):
            raise PltError("Data too short to set IP checksum")
        if cks_v < 0 or cks_v > 0xFFFF:
            raise PltError("Checksum not 16-bit unsigned integer")
        lib.set_short(self.pi.l3p, 10, cks_v)
        return None
    checksum = property(get_checksum, set_checksum)
            
    def get_payload(self):
        new_pi = ffi.new("struct pi *")
        if lib.get_trans_payload(new_pi, self.pi):
            return new_pi.dp[0:new_pi.rem]
        return None
    payload = property(get_payload)

    
def ip(data_in):
    pi, mom = mk_new_obj(TYPE_IP, data_in)
    # Data_dump(pi, mom, "new ip()")
    return _ip_obj(pi, mom)
        

class _ip6_obj(_inet_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        self.pi.o_type = TYPE_IP6
        # Data_dump(self, self.mom, "init _ip6_obj")

    def check_ip6(self, x6):
        if self.pi.o_type != TYPE_IP6:
            raise PltError("Expected an IPv6 object")
        return self.pi.l3_rem >= x6
            
    def get_flow_label(self):
        if self.check_ip6(4):
            return lib.get_long(self.pi.l3p, 0) & 0x000FFFFF
        return None
    flow_label = property(get_flow_label)

    def get_payload_len(self):
        if self.check_ip6(6):
            return lib.get_short(self.pi.l3p, 4)
        return None
    payload_len = property(get_payload_len)
            
    def get_next_hdr(self):
        if self.check_ip6(7):
            return self.pi.l3p[6]
        return None
    next_hdr = property(get_next_hdr)
            
    def get_payload(self):
        new_pi = ffi.new("struct pi *")
        if lib.get_trans_payload(new_pi, self.pi):
            return new_pi.dp[0:new_pi.rem]
        return None
    payload = property(get_payload)


def ip6(data_in):
    pi, mom = mk_new_obj(TYPE_IP6, data_in)
    # Data_dump(pi, mom, "new ip6()")
    return _ip6_obj(pi, mom)
        

class _transport_obj(_inet_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        # Data_dump(self.pi, self.mom, "init _transport_obj")
 

class _tcp_obj(_inet_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        self.pi.o_type = TYPE_TCP

    def check_tcp(self, x):
        if self.pi.o_type != TYPE_TCP:
            raise PltError("Expected a TCP object")
        return self.pi.rem >= x
                
    def get_src_port(self):
        if self.check_tcp(2):
            return lib.get_short(self.pi.dp, 0)
        return None
    src_port = property(get_src_port)

    def get_dst_port(self):
        if self.check_tcp(4):
            return lib.get_short(self.pi.dp, 2)
        return None
    dst_port = property(get_dst_port)

    def get_seq_nbr(self):
        if self.check_tcp(8):
            return lib.get_long(self.pi.dp, 4)
        return None
    seq_nbr = property(get_seq_nbr)

    def get_ack_nbr(self):
        if self.check_tcp(12):
            return lib.get_long(self.pi.dp, 8)
        return None
    ack_nbr = property(get_ack_nbr)

    def get_doff(self):
        if self.check_tcp(13):
            return self.pi.dp[12] >> 4
        return None
    doff = property(get_doff)

    def get_flags(self):
        if self.check_tcp(14):
            return lib.get_short(self.pi.dp, 12) & 0x0FFF
        return None
    flags = property(get_flags)

    def get_urg_flag(self):
        if self.check_tcp(14):
            return bool(self.pi.dp[13] & 0x20)
        return None
    urg_flag = property(get_urg_flag)

    def get_ack_flag(self):
        if self.check_tcp(14):
            return bool(self.pi.dp[13] & 0x10)
        return None
    ack_flag = property(get_ack_flag)

    def get_psh_flag(self):
        if self.check_tcp(14):
            return bool(self.pi.dp[13] & 0x08)
        return None
    psh_flag = property(get_psh_flag)

    def get_rst_flag(self):
        if self.check_tcp(14):
            return bool(self.pi.dp[13] & 0x04)
        return None
    rst_flag = property(get_rst_flag)

    def get_syn_flag(self):
        if self.check_tcp(14):
            return bool(self.pi.dp[13] & 0x02)
        return None
    syn_flag = property(get_syn_flag)

    def get_fin_flag(self):
        if self.check_tcp(14):
            return bool(self.pi.dp[13] & 0x01)
        return None
    fin_flag = property(get_fin_flag)

    def get_window(self):
        if self.check_tcp(16):
            return lib.get_short(self.pi.dp, 14)
        return None
    window = property(get_window)

    def get_checksum(self):
        if self.check_tcp(18):
            return lib.get_short(self.pi.dp, 16)
        return None
    def set_checksum(self, cks_v):
        self.check_tcp(18)
        if cks_v < 0 or cks_v > 0xFFFF:
            raise PltError("Checksum not 16-bit unsigned integer")
        lib.set_short(self.pi.dp, 16, cks_v)
        return None
    checksum = property(get_checksum, set_checksum)

    def get_urg_ptr(self):
        if self.check_tcp(20):
            return lib.get_short(self.pi.dp, 18)
        return None
    urg_ptr = property(get_urg_ptr)

    def get_payload(self):
        hdr_len = (self.pi.dp[12] >> 4)*4  # TCP header length
        if self.pi.rem > hdr_len:
            l5_data = mk_data(self.pi, hdr_len)
            pi = new_pi(TYPE_L5, KIND_CPY, self.pi.data,
                self.pi.l3p, self.pi.l3_rem,  6,
                self.pi.dp[hdr_len:self.pi.rem], self.pi.rem - hdr_len)
            # Data_dump(pi, l5_data, "New TCP payload object")
            return _l5_obj(pi, l5_data)
        return None
    payload = property(get_payload)


def tcp(data_in):
    pi, mom = mk_new_obj(TYPE_TCP, data_in)
    pi.l3_rem = 0  # No l3 data for a new object
    # Data_dump(pi, mom, "new tcp()")
    return _tcp_obj(pi, mom)
        

class _udp_obj(_inet_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        self.pi.o_type = TYPE_UDP
        # Data_dump(self.pi, self.mom, "new _udp_obj ***")

    def check_udp(self, x):
        if self.pi.o_type != TYPE_UDP:
            raise PltError("Expected a UDP object")
        return self.pi.rem >= x
                
    def get_src_port(self):
        if self.check_udp(2):
            return lib.get_short(self.pi.dp, 0)
        return None
    src_port = property(get_src_port)

    def get_dst_port(self):
        if self.check_udp(4):
            return lib.get_short(self.pi.dp, 2)
        return None
    dst_port = property(get_dst_port)

    def get_len(self):
        if self.check_udp(6):
            return lib.get_short(self.pi.dp, 4)
        return None
    len = property(get_len)

    def get_checksum(self):
        if self.check_udp(8):
            return lib.get_short(self.pi.dp, 6)
        return None
    def set_checksum(self, cks_v):
        self.check_udp(8)
        if cks_v < 0 or cks_v > 0xFFFF:
            raise PltError("Checksum not 16-bit unsigned integer")
        lib.set_short(self.pi.dp, 6, cks_v)
        return None
    checksum = property(get_checksum, set_checksum)

    def get_payload(self):
        if self.pi.rem >= 8:  # UDP has 8-byte header
            l5_data = mk_data(self.pi, 8)
            pi = new_pi(TYPE_L5, KIND_CPY, self.pi.data,
                self.pi.l3p, self.pi.l3_rem,  17,
                self.pi.dp[8:self.pi.rem], self.pi.rem - 8)
            # Data_dump(pi, l5_data, "New UDP payload object")
            return _l5_obj(pi, l5_data)
        return None
    payload = property(get_payload)


def udp(data_in):
    pi, mom = mk_new_obj(TYPE_UDP, data_in)
    pi.l3_rem = 0  # No l3 data for a new object
    # Data_dump(pi, mom, "new udp()")
    return _udp_obj(pi, mom)
        

class _l5_obj(_inet_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom


class _icmp_obj(_ip_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        self.pi.o_type = TYPE_ICMP
        # Data_dump(self.pi, self.mom, "new _icmp_obj ***")

    def check_icmp(self, x):
        if self.pi.o_type != TYPE_ICMP:
            raise PltError("Expected an ICMP object")
        return self.pi.rem >= x
                
    def get_type(self):
        if self.check_icmp(1):
            return self.pi.dp[0]
        return None
    type = property(get_type)

    def get_code(self):
        if self.check_icmp(2):
            return self.pi.dp[1]
        return None
    code = property(get_code)

    def get_checksum(self):
        if self.check_icmp(4):
            return lib.get_short(self.pi.dp, 2)
        return None
    def set_checksum(self, cks_v):
        self.check_icmp(4)
        if cks_v < 0 or cks_v > 0xFFFF:
            raise PltError("Checksum not 16-bit unsigned integer")
        lib.set_short(self.pi.dp, 2, cks_v)
        return None
    checksum = property(get_checksum, set_checksum)

    def get_payload(self):
        if self.pi.rem < 8:  # ICMP 4-byte hdr + 4 bytes reserved
            return None
        pi, l5_data = sub_trans_obj(self.pi, 8)
        ityp = self.pi.dp[0]  # ICMP type
        if ityp == 3 or ityp == 4 or ityp == 5 or ityp == 11 or ityp == 12:
            pi.o_type = TYPE_IP
            return _ip_obj(pi, l5_data)
        else:
            return _l5_obj(pi, l5_data)
    payload = property(get_payload)

    def get_echo(self):
        if self.pi.rem >= 8:  # ICMP hdr + echo ident and sequence
            return _echo_obj(self.pi, self.mom)
        return None
    echo = property(get_echo)

    def get_redirect(self):
        if self.pi.rem >= 8:  # ICMP hdr + 4-byte gateway address
            pi, l5_data = sub_trans_obj(self.pi, 8)
            return _redirect_obj(self.pi, self.mom)
        return None
    redirect = property(get_redirect)

class _echo_obj(_icmp_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        print
    
    def get_ident(self):
        if self.pi.rem >= 6:
            return lib.get_short(self.pi.dp, 4)
        return None
    ident = property(get_ident)

    def get_sequence(self):
        if self.pi.rem >= 8:
            return lib.get_short(self.pi.dp, 6)
        return None
    sequence = property(get_sequence)


class _redirect_obj(_icmp_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
    
    def get_gateway(self):
        if self.pi.rem >= 8:
            return ipp.IPprefix(4, self.pi.dp[4:8])
        return None
    gateway = property(get_gateway)

    
def icmp(data_in):
    pi, mom = mk_new_obj(TYPE_ICMP, data_in)
    pi.l3_rem = 0  # No l3 data for a new object
    # Data_dump(pi, mom, "new icmp()")
    return _icmp_obj(pi, mom)
        

class _icmp6_obj(_ip_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
        self.pi.o_type = TYPE_ICMP6
        # Data_dump(self.pi, self.mom, "new _icmp6_obj ***")

    def check_icmp6(self, x):
        if self.pi.o_type != TYPE_ICMP6:
            raise PltError("Expected an ICMP6 object")
        return self.pi.rem >= x
                
    def get_type(self):
        if self.check_icmp6(1):
            return self.pi.dp[0]
        return None
    type = property(get_type)

    def get_code(self):
        if self.check_icmp6(2):
            return self.pi.dp[1]
        return None
    code = property(get_code)

    def get_checksum(self):
        if self.check_icmp6(4):
            return lib.get_short(self.pi.dp, 2)
        return None
    def set_checksum(self, cks_v):
        self.check_icmp6(4)
        if cks_v < 0 or cks_v > 0xFFFF:
            raise PltError("Checksum not 16-bit unsigned integer")
        lib.set_short(self.pi.dp, 2, cks_v)
        return None
    checksum = property(get_checksum, set_checksum)

    def get_payload(self):
        if self.pi.rem < 8:  # ICMP 4-byte hdr + 4 bytes reserved
            return None
        pi, l5_data = sub_trans_obj(self.pi, 8)
        ityp = self.pi.dp[0]  # ICMP type
        if ityp == 1 or ityp == 2 or ityp == 3 or ityp == 4:
            pi.o_type = TYPE_IP6
            return _ip_obj(pi, l5_data)
        else:
            return _l5_obj(pi, l5_data)
    payload = property(get_payload)

    def get_echo(self):
        if self.pi.rem >= 8:  # ICMP hdr + reserved
            return _echo_obj(self.pi, self.mom)
        return None
    echo = property(get_echo)

    def get_toobig(self):
        if self.pi.rem >= 8:  # ICMP hdr + reserved
            return _toobig_obj(self.pi, self.mom)
        return None
    toobig = property(get_toobig)

    def get_param(self):
        if self.pi.rem >= 8:  # ICMP hdr + reserved
            return _param_obj(self.pi, self.mom)
        return None
    param = property(get_param)

    def get_neighbour(self):
        if self.pi.rem >= 8:  # ICMP hdr + reserved
            return _neighbour_obj(self.pi, self.mom)
        return None
    neighbour = property(get_neighbour)


class _echo_obj(_icmp_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
    
    def get_ident(self):
        if self.pi.rem >= 6:
            return lib.get_short(self.pi.dp, 4)
        return None
    ident = property(get_ident)

    def get_sequence(self):
        if self.pi.rem >= 8:
            return lib.get_short(self.pi.dp, 6)
        return None
    sequence = property(get_sequence)

    
class _toobig_obj(_icmp_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
    
    def get_mtu(self):
        if self.pi.rem >= 8:
            return lib.get_long(self.pi.dp, 4)
        return None
    mtu = property(get_mtu)

    
class _param_obj(_icmp6_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
    
    def get_pointer(self):
        if self.pi.rem >= 8:
            return lib.get_long(self.pi.dp, 4)
        return None
    pointer = property(get_pointer)

    
class _neighbour_obj(_icmp6_obj):
    def __init__(self, pi, mom):
        self.pi = pi;  self.mom = mom
    
    def get_target_prefix(self):
        if self.pi.rem >= 24:
            return ipp.IPprefix(6, self.pi.dp[8:24])
        return None
    target_prefix = property(get_target_prefix)


def icmp6(data_in):
    pi, mom = mk_new_obj(TYPE_ICMP6, data_in)
    pi.l3_rem = 0  # No l3 data for a new object
    # Data_dump(pi, mom, "new icmp6()")
    return _icmp6_obj(pi, mom)
