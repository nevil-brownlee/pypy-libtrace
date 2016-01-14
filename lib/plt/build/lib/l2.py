
# l2.py:


class LibtraceError(Exception):  # Create new Exception
    pass

class PltError(Exception):
    pass


def trace(uri):
    return _trace(uri)
        
class _trace:
    def __init__(self, uri):
        self.uri = uri
        self.ltrace = lib.trace_create(uri)       # Create trace
        self.pkt = _pkt_obj()
        self.started = False

    def __str__(self):
        ready = 'not '
        if self.started:
            ready = ''
        return "<Trace: %s, %sstarted>" % (self.uri, ready)

    def start(self):        
        if lib.trace_start(self.ltrace) < 0:
            raise LibtraceError("Trace not started")
        self.started = True

    def read_packet(self):
        if not self.started:
            raise LibtraceError("Trace not started")
        r = lib.trace_read_packet(self.ltrace, self.pkt.p)
        if r == 0:  # EOF
            return False
        elif r < 0:  # libtrace error
            raise LibtraceError("trace_read_packet() failed")
        r = lib.get_pkt_info(self.pkt.pi, self.pkt.p)
        if r < 0:
            raise LibtraceError("get_pkt_info(): error %d" % -r)
        self.pkt.pi.type = plt.TYPE_PKT;  self.pkt.pi.kind = plt.KIND_PKT
        return True
        
    def __iter__(self):
        while True:  # Read packets from the trace
            r = self.read_packet()
            if not r:  # EOF
                break
            yield self.pkt


class _pkt_obj(object):
    def __init__(self):
        self.p = lib.trace_create_packet()  # Instance variables
        self.pi =  ffi.new("struct pi *")
        plt.Data_dump(self.pi, "init _pkt_obj")

    def get_transport(self):
        if self.pi.kind != plt.KIND_PKT:
            raise PltError("Object didn't come from a plt Packet")
        new_pi = ffi.new("struct pi *")
        if lib.get_transport_info(self.pi, new_pi):
            new_pi.type = plt.TYPE_L4;  new_pi.kind = plt.KIND_PKT
            return _ip_obj(new_pi)
    transport = property(get_transport)

    def get_ip(self):
        return _ip_obj(self.pi)
    ip = property(get_ip)

    def get_udp(self):
        new_pi = ffi.new("struct pi *")
        if lib.get_trans_payload(self.pi, new_pi):
            new_pi.type = plt.TYPE_UDP;  new_pi.kind = self.pi.kind
            return _udp_obj(new_pi)
        return _udp_obj(self.pi)
    udp = property(get_udp)

    # Instance attributes ...
    
    def get_linktype(self):
        return self.pi.linktype
    linktype = property(get_linktype)

    def get_ethertype(self):
        return self.pi.ethertype
    ethertype = property(get_ethertype)

    def vlan_id(self):
        return self.pi.vlan_tag
    vlan_id = property(vlan_id)


class _transport_obj(_pkt_obj):
    def __init__(self, pi):
        self.pi = pi
        self.pi.type = plt.TYPE_Internet
 

class _inet_obj(_pkt_obj):
    def get_inet(self, x4, x6):
        version = self.pi.l3p[0] >> 4
        if version == 4 and self.pi.l3_rem >= x4:
            return 4
        if version == 6 and self.pi.l3_rem >= x6:
            return 6
        return 0  # Not enough bytes
        
    def get_src_prefix(self):
        version = self.get_inet(20, 40)
        if version == 4:
            print "src_v4 = %s" % ipp.IPprefix(4, self.pi.l3p[12:16])
            return ipp.IPprefix(4, self.pi.l3p[12:16])
        elif version == 6:
            return ipp.IPprefix(6, self.pi.pi.l3p[8:24])
    src_prefix = property(get_src_prefix)  #, set_param)
        
    def get_dst_prefix(self):
        version = self.get_inet(20, 40)
        if version == 4:
            print "dst_v4 = %s" % ipp.IPprefix(4, self.pi.l3p[8:20], None)
            return ipp.IPprefix(4, self.pi.l3p[8:20], None)
        elif version == 6:
            return ipp.IPprefix(4, self.pi.l3p[24:40], None)
    dst_prefix = property(get_dst_prefix)  #, set_param)


class _ip_obj(_inet_obj):
    def __init__(self, pi):
        print "pkt.etherype = %04x" % pkt.pi.ethertype
        self.pi = pi
        self.pi.type = plt.TYPE_IP
        plt.Data_dump(self.pi, "init _ip_obj")
    
        
    def get_ident(self):
        if self.pi.l3_rem >= 6:
            return lib.get_short(self.pi.l3p, 4)
        else:
            return None
    ident = property(get_ident)
        

class _udp_obj(_inet_obj):
    def __init__(self, pi):
        self.pi = pi
 
