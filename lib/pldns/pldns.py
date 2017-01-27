# 1241, Fri 18 Dec 2015 (NZDT)
#
# pldns.py:  pldns objects iplemented in python
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


from cpldns import ffi, lib
import plt
import string

def opcodestr(opcode):  # Module methods that return strings
    return ffi.string(lib.ldns_pkt_opcode2str(opcode)).decode('ascii')

def rcodestr(rcode):
    return ffi.string(lib.ldns_pkt_rcode2str(rcode)).decode('ascii')

def typestr(type):
    return ffi.string(lib.ldns_rr_type2str(type)).decode('ascii')

def errorstr(err_nbr):
    return ffi.string(lib.ldns_get_errorstr_by_id(err_nbr)).decode('ascii')


class _ldns_rr(object):
    def __init__(self, ldns_rr):
        self.rr = ldns_rr  # struct ldns_struct_rr *

    def get_owner(self):
        owner = lib.ldns_rr_owner(self.rr)
        return ffi.string(lib.ldns_rdf2str(owner)).decode('ascii')
    owner = property(get_owner)

    def get_type(self):
        return lib.ldns_rr_get_type(self.rr)
    type = property(get_type)

    def get_ttl(self):
        return lib.ldns_rr_ttl(self.rr)
    ttl = property(get_ttl)

    def get_rdata(self):
        rdl = []
        len = lib.ldns_rr_rd_count(self.rr)
        for j in range(len):
            rdf = lib.ldns_rr_rdf(self.rr, j)
            rdf_s = ffi.string(lib.ldns_rdf2str(rdf)).decode('ascii')
            rdl.append(rdf_s)
        return ' '.join(rdl)
    rdata = property(get_rdata)

    def get_str(self):
        return plt.ca2str(lib.ldns_rr2str(self.rr))
    str = property(get_str)
        

class _ldns_object(object):
    def __init__(self, ldns_info):
        self.ldns_info = ldns_info

    def is_ok(self):
        return bool(lib.ldns_ok(self.ldns_info))

    def get_status(self):
        return self.ldns_info.status
    status = property(get_status)

    def get_ident(self):
        return lib.ldns_pkt_id(self.ldns_info.ldpkt)
    ident = property(get_ident)

    def get_is_response(self):
        return bool(lib.ldns_pkt_qr(self.ldns_info.ldpkt))
    is_response = property(get_is_response)

    def get_opcode(self):
        return lib.ldns_pkt_get_opcode(self.ldns_info.ldpkt)
    opcode = property(get_opcode)

    def get_rcode(self):
        return lib.ldns_pkt_get_rcode(self.ldns_info.ldpkt)
    rcode = property(get_rcode)

    def pldns_rr_list(self, rr_list):
        rl = []
        len = lib.ldns_rr_list_rr_count(rr_list)
        for j in range(len):
            next_rr = lib.ldns_rr_list_rr(rr_list, j)
            rl.append(_ldns_rr(next_rr))
        return rl

    def get_query_rr_list(self):
        qlen = lib.ldns_pkt_qdcount(self.ldns_info.ldpkt)
        if qlen == 0:
            return None
        return self.pldns_rr_list(lib.ldns_pkt_question(self.ldns_info.ldpkt))
    query_rr_list = property(get_query_rr_list)

    def response_rr_list(self):
        qlen = lib.ldns_pkt_qdcount(self.ldns_info.ldpkt)
        if qlen == 0:
            return None
        return self.pldns_rr_list(lib.ldns_pkt_answer(self.ldns_info.ldpkt))
    response_rr_list = property(response_rr_list)

    def get_auth_rr_list(self):
        qlen = lib.ldns_pkt_qdcount(self.ldns_info.ldpkt)
        if qlen == 0:
            return None
        return self.pldns_rr_list(lib.ldns_pkt_authority(self.ldns_info.ldpkt))
    auth_rr_list = property(get_auth_rr_list)

    def get_addit_rr_list(self):
        qlen = lib.ldns_pkt_qdcount(self.ldns_info.ldpkt)
        if qlen == 0:
            return None
        return self.pldns_rr_list(lib.ldns_pkt_additional(self.ldns_info.ldpkt))
    addit_rr_list= property(get_addit_rr_list)


def ldns(obj):  # Objects have a .pi and a .mom
    if obj.pi.o_kind != plt.KIND_CPY or obj.pi.o_type != plt.TYPE_L5:
        raise plt.PltError("Expected a LEVEL_5 object")
    ldns_info = ffi.new("struct ldns_info *")
    dns_status = lib.get_ldns_info(ldns_info, obj.pi.dp, obj.pi.rem)
    # plt.Data_dump(obj.pi, obj.mom, "new _ldns_object")
    return _ldns_object(ldns_info)
        
                            
        
