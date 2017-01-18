# 1529, Tue  5 Jan 2016 (NZDT)
#
# natkit.py:  natkit objects iplemented in python
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


from cnatkit import ffi, lib
from cplt import lib as plt_lib
#from ipp import lib as ipp_lib
import ipp, plt


def ba_get_short(bp, x):
    if isinstance(bp, ffi.CData):
        if x+2 > ffi.sizeof(bp):
            return None
        return plt.get_short(bp, x)
    elif isinstance(bp, bytearray):
        if x+2 > len(bp):
            return None
        data = ffi.new("uint8_t[]", tuple(bp))
        return plt.get_short(data, x)
    raise TypeError("Expected 'cdata uint8_t[]' or bytearray!")

def ba_get_long(bp, x):
    if isinstance(bp, ffi.CData):
        if x+4 > ffi.sizeof(bp):
            return None
        return plt.get_long(bp, x)
    elif isinstance(bp, bytearray):
        if x+4 > len(bp):
            return None
        data = ffi.new("uint8_t[]", tuple(bp))
        return plt.get_long(data, x)
    raise TypeError("Expected 'cdata uint8_t[]' or bytearray!")


def seq_add(a, b):
    return lib.seq_add(a, b)

def seq_sub(a, b):
    return lib.seq_sub(a, b)

def seq_lt(a, b):
    return bool(lib.seq_lt(a, b))

def seq_ge(a, b):
    return bool(lib.seq_ge(a, b))

def seq_gt(a, b):
    return bool(lib.seq_gt(a, b))


FT_FIRST_PKT = 0  # Default, first pkt src->dst
FT_HOME_FLOW = 1

class _IPflow_obj(object):
    def __init__(self, ftype, fkey, p2):
        self.ftype = ftype
        if self.ftype == FT_FIRST_PKT:
            self.home_flow = False
        elif ftype == FT_HOME_FLOW:
            self.home_flow = True
            self.prefixes = p2
        self.fkey = fkey

    def get_version(self):
        return self.fkey.version
    version = property(get_version)
        
    def get_proto(self):
        return self.fkey.proto
    proto = property(get_proto)
        
    def get_src_port(self):
        return plt.lib.get_short(self.fkey.sport, 0)
    src_port = property(get_src_port)
        
    def get_dst_port(self):
        return plt.lib.get_short(self.fkey.dport, 0)
    dst_port = property(get_dst_port)
        
    def get_src_prefix(self):
        return ipp.IPprefix(self.fkey.version, self.fkey.saddr)
    src_prefix = property(get_src_prefix)
        
    def get_dst_prefix(self):
        return ipp.IPprefix(self.fkey.version, self.fkey.daddr)
    dst_prefix = property(get_dst_prefix)
        
    def get_fwd_key(self):
        return str(ffi.buffer(self.fkey))
    fwd_key = property(get_fwd_key)
        
    def get_rev_key(self):
        if self.fkey.version == 4:
            rkey = ffi.new("struct fkey4 *");  alen = 4
        else:
            rkey = ffi.new("struct fkey6 *");  alen = 16
        rkey.version = self.fkey.version
        rkey.proto = self.fkey.proto
        ffi.memmove(rkey.sport, self.fkey.dport, 2)
        ffi.memmove(rkey.dport, self.fkey.sport, 2)
        ffi.memmove(rkey.saddr, self.fkey.daddr, alen)
        ffi.memmove(rkey.daddr, self.fkey.saddr, alen)
        return str(ffi.buffer(rkey))
    rev_key = property(get_rev_key)

    def addr_in_home(self, a):
        apref = ipp.IPprefix(self.fkey.version, a)
        for p in self.prefixes:
            if p.version == self.fkey.version:  # Avoid PltError
                if p.is_prefix(apref):
                    return True
        return False
    
    def get_home_key(self):
        if self.home_flow:
            if self.addr_in_home(self.fkey.daddr):
                return str(ffi.buffer(self.fkey))
            else:
                return self.get_rev_key()
        else:
           return None
    home_key = property(get_home_key)

    def get_src_in_home(self):
        if self.home_flow:
            return self.addr_in_home(self.fkey.saddr)
        else:
           raise plt.PltError("Expected a HomeFlow")
    src_in_home = property(get_src_in_home)
        
    def get_dst_in_home(self):
        if self.home_flow:
            return self.addr_in_home(self.fkey.daddr)
        else:
           raise plt.PltError("Expected a HomeFlow")
    dst_in_home = property(get_dst_in_home)
        
    def get_is_inward(self):
        if self.home_flow:
            if self.addr_in_home(self.fkey.daddr):
                return not self.addr_in_home(self.fkey.saddr)
            else:
                return self.addr_in_home(self.fkey.saddr)
        else:
           raise plt.PltError("Expected a HomeFlow")
    is_inward = property(get_is_inward)

    def flow(self, pkt):
        if self.home_flow:
            fkey = make_fkey(pkt)
            return _IPflow_obj(FT_HOME_FLOW, fkey, self.prefixes)
        else:
           raise plt.PltError("Expected a HomeFlow")

def make_fkey(obj):
    if obj.pi.o_kind != plt.KIND_PKT:
        raise plt.PltError("Object didn't come from a plt Packet")
    if obj.pi.proto == 0:
        new_pi = plt.ffi.new("struct pi *")
        if not bool(plt_lib.get_transport_info(new_pi, obj.pi)):
            return None
        pi = new_pi
    if pi.ethertype == 0x0800:
        fkey = ffi.new("struct fkey4 *")
        fkey.version = 4
        ffi.memmove(fkey.saddr, pi.l3p[12:16], 4)
        ffi.memmove(fkey.daddr, pi.l3p[16:20], 4)
    elif pi.ethertype == 0x86DD:
        fkey = ffi.new("struct fkey6 *")
        fkey.version = 6;
        ffi.memmove(fkey.saddr, pi.l3p[8:24], 16)
        ffi.memmove(fkey.daddr, pi.l3p[24:40], 16)
    else:  # Not IP or IP6
        return None
    fkey.proto = pi.proto
    if pi.proto == 6 or pi.proto == 17:  # TCP|UDP port numbers
        ffi.memmove(fkey.sport, pi.dp[0:2], 2)
        ffi.memmove(fkey.dport, pi.dp[2:4], 2)
        #alen = 4
        #if fkey.version == 6:
        #    alen = 16
        #klen = 6+2*alen
        #bkey = ffi.new("uint8_t[]", klen)  # Don't use ([%d], klen) !
        #ffi.memmove(bkey, fkey, klen)
        #print "!!! bkey = %s" % bkey
        #plt.print_cdata(bkey, klen)
    return fkey

    
def IPflow(obj):  # Make IPflow from plt object
    # plt.Data_dump(obj.pi, None, "starting IPflow()")
    if not isinstance(obj, plt._pkt_obj):
        raise plt.PltError("IPflow: arg not plt_pkt object")
    fkey = make_fkey(obj)
    if fkey:
        return _IPflow_obj(FT_FIRST_PKT, fkey, None)
    return None


def FlowHome(*prefixes):  # Make IPflow from plt object
    return _IPflow_obj(FT_HOME_FLOW, None, prefixes)

