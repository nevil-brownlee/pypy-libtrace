# 1222, Fri 18 Dec 2015 (NZDT)
#
# build-cpldns.py:  makes pypy cpldns module
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


from cffi import FFI

ffi = FFI()

# Our C functions
ffi.cdef("int get_ldns_info(struct ldns_info *ldi, uint8_t *dns_msg, int dns_len);")
ffi.cdef("int ldns_ok(struct ldns_info *ldi);")

ffi.cdef("uint16_t get_short(uint8_t *bp, uint16_t x);")

ffi.set_source("cpldns",  # .so to be created
    """ // passed to the real C compiler
#include <ldns/ldns.h>

struct ldns_info {
   struct ldns_struct_pkt *ldpkt;  /* ldns_pkt typedef in ldns/packet.h */
   int status;  /* enum in ldns */
   };

int get_ldns_info(struct ldns_info *ldi, uint8_t *dns_msg, int dns_len) {
   ldi->status = ldns_wire2pkt(&ldi->ldpkt, dns_msg, dns_len);
   return ldi->ldpkt != NULL;  /* True if all's well */
   }

int ldns_ok(struct ldns_info *ldi) {
   return ldi->status == LDNS_STATUS_OK;
   }

uint16_t get_short(uint8_t *bp, uint16_t x) {
   uint16_t v = *(uint16_t *)&bp[x];
   return ntohs(v);
   }
    """,
    libraries=["ldns", "c"])   # list of libraries to link with

ffi.cdef(
    """
struct ldns_info {
   struct ldns_struct_pkt *ldpkt;  /* ldns_pkt typedef in ldns/packet.h */
   int status;  /* enum in ldns */
   };

extern const char *ldns_pkt_opcode2str(int rcode);  /* enum */
extern const char *ldns_pkt_rcode2str(int rcode);  /* enum */
extern const char *ldns_rr_type2str(int type);  /* enum */
extern const char *ldns_get_errorstr_by_id(int errnbr);  /* enum */

extern uint16_t ldns_pkt_id(struct ldns_struct_pkt *p);
extern bool ldns_pkt_qr(struct ldns_struct_pkt *p);
extern int ldns_pkt_get_opcode(struct ldns_struct_pkt *p);  /* enum */
extern int ldns_pkt_get_rcode(struct ldns_struct_pkt *p);  /* enum */

extern uint16_t ldns_pkt_qdcount(struct ldns_struct_pkt *p);
extern struct ldns_struct_rr_list *ldns_pkt_question(
   struct ldns_struct_pkt *p);
extern struct ldns_struct_rr_list *ldns_pkt_answer(
   struct ldns_struct_pkt *p);
extern struct ldns_struct_rr_list *ldns_pkt_authority(
   struct ldns_struct_pkt *p);
extern struct ldns_struct_rr_list *ldns_pkt_additional(
   struct ldns_struct_pkt *p);

extern int ldns_rr_list_rr_count(struct ldns_struct_rr_list *rr_list);
extern struct ldns_struct_rr *ldns_rr_list_rr(
   struct ldns_struct_rr_list *rr_list, size_t nr);

extern int ldns_rr_get_type(struct ldns_struct_rr *rr);
extern size_t ldns_rdf_size(struct ldns_struct_rdf *rd);
extern int ldns_rdf_get_type(struct ldns_struct_rdf *rd);

extern struct ldns_struct_rdf *ldns_rr_owner(struct ldns_struct_rr *rr);
extern char *ldns_rdf2str(struct ldns_struct_rdf *rdf);

extern size_t ldns_rr_rd_count(struct ldns_struct_rr *rr);
   // get number of rdata fields
extern uint32_t ldns_rr_ttl(struct ldns_struct_rr *rr);
extern char *ldns_rr2str(struct ldns_struct_rr *rr);

extern struct ldns_struct_rdf *ldns_rr_rdf(struct ldns_struct_rr *rr, size_t n);
   // get nth rdf fromm rr

struct ldns_struct_rdf {  /* From ldata/rdata.h */
   size_t _size;   /* The size of the data (in octets) */
   int _type;  /* enum: The type of the data */      
   void  *_data;   /* Pointer to the data (raw octets) */
   };

    """)

if __name__ == "__main__":
    ffi.compile()
    
