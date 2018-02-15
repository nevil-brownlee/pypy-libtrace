# 1027, Mon 13 Dec 2015 (NZDT)
#
# build-cplt.py:  makes pypy cplt module
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


import os

from cffi import FFI

ffi = FFI()

ffi.cdef("int get_pkt_info(struct pi *spi, struct libtrace_packet_t *packet);")
ffi.cdef("int get_transport_info(struct pi *new_spi, struct pi *spi);")
ffi.cdef("int get_trans_payload(struct pi *new_spi, struct pi *spi);")
ffi.cdef("int get_ip6_proto(struct pi *pi);")
ffi.cdef("double get_time(struct pi *spi);")
ffi.cdef("int get_ts_sec(struct pi *spi);")
ffi.cdef("uint64_t get_erf_time(struct pi *spi);")
ffi.cdef("int get_wire_len(struct pi *spi);")
ffi.cdef("int get_capture_len(struct pi *spi);")
ffi.cdef("int get_direction(struct pi *spi);")

ffi.cdef("int l3_checksum(struct pi *p, int reset_cks);")
ffi.cdef("int transport_checksum(struct pi *d, int reset_cks);")

ffi.cdef("int NO_COMPRESSION;")  # libtrace constants
ffi.cdef("int ZLIB_COMPRESSION;")
ffi.cdef("int ZIP2_COMPRESSION;")
ffi.cdef("int LZO_COMPRESSION;")
ffi.cdef("int OUT_FILE_FLAGS;")
ffi.cdef("int OUT_COMP_TYPE;")
ffi.cdef("int OUT_COMP_LEVEL;")
ffi.cdef("int OPT_SNAPLEN;")
ffi.cdef("int OPT_PROMISC;")

ffi.cdef("int TRACE_ETH;")
ffi.cdef("int TRACE_ATM;")
ffi.cdef("int TRACE_80211;")
ffi.cdef("int TRACE_NONE;")
ffi.cdef("int TRACE_LINUX_SLL;")
ffi.cdef("int TRACE_PFLOG;")
ffi.cdef("int TRACE_DUCK;")
ffi.cdef("int TRACE_80211_RADIO;")
ffi.cdef("int TRACE_LLCSNAP;")
ffi.cdef("int TRACE_PPP;")
ffi.cdef("int TRACE_METADATA;")

ffi.cdef("int DIR_OUTGOING;")
ffi.cdef("int DIR_INCOMING;")
ffi.cdef("int DIR_OTHER;")
ffi.cdef("int DIR_UNKNOWN;")

ffi.cdef("int set_config(struct libtrace_t *trace, int which, int val);")
ffi.cdef("int set_config_filter(struct libtrace_t *trace, struct libtrace_filter_t *filter);")
ffi.cdef("int set_config_output(struct libtrace_out_t *trace, int which, int val);")
ffi.cdef("int event_read_packet(struct libtrace_t *lt_trace, struct libtrace_packet_t *packet, long user_timeout);")

ffi.cdef("uint16_t get_short(uint8_t *bp, uint16_t x);")
ffi.cdef("void set_short(uint8_t *bp, int bx, uint16_t v);")
ffi.cdef("uint32_t get_long(uint8_t *bp, int32_t x);")
ffi.cdef("void set_long(uint8_t *bp, int bx, uint32_t v);")
ffi.cdef("uint32_t get_bytes(uint8_t *bp, uint16_t x, int nbytes);")
ffi.cdef("void set_bytes(uint8_t *dstp, uint8_t *srcp, int x);")

ffi.set_source("cplt",  # .so to be created
    """ // passed to the real C compiler
#include "libtrace.h"
#include <arpa/inet.h>

struct pi {  /* DataObject info */
   int o_type;      /* Data type, RLT_TYPE values above */
   int o_kind;      /* Object kind, defined abve */
   uint8_t *data;   /* Pointer to the object's libtrace_packet_t or string */
   uint8_t *l2p;    /* Pointer to link-layer header */
   int l2_rem;      /* Bytes remaining at layer 2 */
   int linktype;    /* link type from original pkt, so we
		         can use trace_get_payload_from_layer2() */
   int ethertype;   /* So we can see its network protocol */
   int vlan_tag;    /* 802.1Q tag, NULL if there isn't one */
   uint8_t *l3p;    /* Pointer to the object's Internet struct */
   int l3_rem;      /* Remaining bytes for encapsulating python object */
   int proto;       /* Transport protocol */
   uint8_t *dp;     /* Pointer to an object within a DataObject
                         e.g. to a libtrace_tcp_t in a packet */
   int rem;         /* Number of bytes in the object */
   };

int get_pkt_info(struct pi *spi, struct libtrace_packet_t *packet) {
   libtrace_linktype_t linktype;  uint32_t l2_rem; int vlan = 0;
   memset(spi, 0, sizeof(struct pi));
   void *l2p = trace_get_layer2(packet, &linktype, &l2_rem);
   if (!l2p) return -2;
   uint16_t ethertype;  uint32_t l3_rem = l2_rem;
   void *l3p = trace_get_payload_from_layer2(
      l2p, linktype, &ethertype, &l3_rem);
   if (!l3p) return -3;
   if (ethertype == 0x8100) {  /* 802.1 Q VLAN tag */
      uint16_t vlan_et;  uint32_t vlan_rem;
      uint16_t *vltp = (uint16_t *)l3p;  vlan = ntohs(vltp[0]);
      vlan_rem = l3_rem;
      void *vlp = trace_get_payload_from_vlan(l3p, &vlan_et, &vlan_rem);
      vlan_rem = l3_rem-4;  /* libtrace 3.0.22 doesn't allow for vlan hdr! */
      if (!vlp) return -4;
      ethertype = vlan_et;
      l2_rem -= vlp-l3p;  l3p = vlp;  l3_rem = vlan_rem;
      }
   spi->data = (uint8_t *)packet;
   spi->l2_rem = l2_rem;  spi->l2p = l2p;
   spi->linktype = linktype;
   spi->ethertype = ethertype;  spi->vlan_tag = vlan;
   spi->rem = spi->l3_rem = l3_rem;  spi->dp = spi->l3p = l3p;
   return l3_rem > 0;
   }

int get_transport_info(struct pi *new_spi, struct pi *spi) {
   uint8_t proto, *trans;  uint32_t remaining = spi->l3_rem;
   trans = (uint8_t *)trace_get_transport(
      (libtrace_packet_t *)spi->data, &proto, &remaining);
   if (trans) {
      memmove(new_spi, spi, sizeof(struct pi));
      new_spi->proto = proto;
      new_spi->dp = trans;  new_spi->rem = remaining;
      return 1;
      }
   return 0;
   }

int get_ip6_proto(struct pi *pi) {
   uint32_t remaining = pi->l3_rem;  uint8_t proto;
   libtrace_ip6_t *lip6 = (libtrace_ip6_t *)pi->l3p;
   void *trans = trace_get_payload_from_ip6(lip6, &proto, &remaining);
   if (trans) {
      pi->proto = proto;  return 1;
      }
   return 0;
   }

int get_trans_payload(struct pi *new_spi, struct pi *spi) {
   uint8_t *l4p = NULL;  int rem = 0;
   uint8_t proto;  uint32_t remaining = spi->l3_rem;
   void *datap = spi->l3p;
   if (spi->ethertype == 0x0800)
      l4p = trace_get_payload_from_ip(
         (libtrace_ip_t *)datap, &proto, &remaining);
   else if (spi->ethertype == 0x86DD)
      l4p = trace_get_payload_from_ip6(
         (libtrace_ip6_t *)datap, &proto, &remaining);
   if (l4p) {
      if (spi->ethertype == 0x0800) {  /* Use IPv4/IPv6 payload len */
         libtrace_ip_t *lip = datap;
         rem = ntohs(lip->ip_len) - lip->ip_hl*4;
         }
      else if (spi->ethertype == 0x86DD) {
         libtrace_ip6_t *l6p = datap;
         rem = ntohs(l6p->plen);
         }
      if (remaining-rem <= 4) remaining = rem;
         /* DIfference <= 4-bte Ethernet FCS */
      if (remaining >= 4) {  /* Enough for tcp/udp source port */
         memmove(new_spi, spi, sizeof(struct pi));
         new_spi->proto = proto;
         new_spi->dp = l4p;  new_spi->rem = remaining;
         }
      return 1;
      }
   return 0;
   }

double get_time(struct pi *spi) {
   return trace_get_seconds((libtrace_packet_t *)spi->data);
   }
double get_ts_sec(struct pi *spi) {
   struct timeval tv = trace_get_timeval((libtrace_packet_t *)spi->data);
   return tv.tv_sec;
   }
uint64_t get_erf_time(struct pi *spi) {
   return trace_get_erf_timestamp((libtrace_packet_t *)spi->data);
   }
int get_wire_len(struct pi *spi) {
   return trace_get_wire_length((libtrace_packet_t *)spi->data);
   }
int get_capture_len(struct pi *spi) {
   return trace_get_capture_length((libtrace_packet_t *)spi->data);
   }
int get_direction(struct pi *spi) {
   return trace_get_direction((libtrace_packet_t *)spi->data);
   }

int checksum(void *buffer, uint16_t len) {
   /* Returns correct checksum (after final complememt) */
   uint32_t sum = 0;
   uint16_t *buff = (uint16_t *)buffer;
   uint16_t count = len;
   while (count > 1) {
      sum += *buff++;  count = count-2;
      }
   if (count > 0) sum += *(uint8_t *)buff;
   while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
   return sum;
   }

int l3_checksum(struct pi *d, int reset_cks) {
   if (d->ethertype != 0x0800)  /* IPv6 doesn't have a header checksum */ 
      return -1;
   libtrace_ip_t *lip = (libtrace_ip_t *)d->l3p;
   if (d->l3_rem < 1) return -1;  /* Not enough bytes */
   int hlen = lip->ip_hl*4;
   if (d->l3_rem < hlen) return -1;
   uint16_t save_cks = lip->ip_sum;
   lip->ip_sum = 0;  /* Set IP checksum */
   uint16_t sum = ~checksum(lip, hlen);
   if (reset_cks) { lip->ip_sum = sum;  return 1; }
   else { lip->ip_sum = save_cks;  return sum == save_cks; }
   }

int transport_checksum(struct pi *d, int reset_cks) {
   /* Returns 1 = cks ok, 0 = bad cks, -1 = can't compute checksum.
      NOTE: If it's correct, the computed transport checksum will be 0 */
   uint16_t save_cks;
   uint16_t *check = NULL, temp;  uint32_t tmp32, sum = 0;
   uint16_t payload_len;
   if (d->proto == 0) {
      uint32_t remaining = d->l3_rem;  uint8_t proto = 0;
      uint8_t *l4p = NULL;
      if (d->ethertype == 0x0800)
         l4p = trace_get_payload_from_ip(
            (libtrace_ip_t *)d->l3p, &proto, &remaining);
      else if (d->ethertype == 0x86DD)
         l4p = trace_get_payload_from_ip6(
            (libtrace_ip6_t *)d->l3p, &proto, &remaining);
      d->dp = l4p;  d->rem = remaining;
      d->proto = proto;
      }
   if (d->ethertype == 0x0800) {  /* v4 */
      libtrace_ip_t *ip = (libtrace_ip_t *)d->l3p;
      payload_len = ntohs(ip->ip_len);
      if (d->l3_rem < payload_len) return -1;  /* Not enough bytes */
      payload_len -= ip->ip_hl*4;
      int j;  
      if (d->proto == 1) {  /* ICMP (Checksum only covers ICMP message) */
         libtrace_icmp_t *icmp_header = (libtrace_icmp_t *)d->dp;
         check = &icmp_header->checksum;
         }
      else {
         sum += checksum(&ip->ip_src.s_addr, 4);  /* 12-byte pseudo-header */
         sum += checksum(&ip->ip_dst.s_addr, 4);
         temp = htons(ip->ip_p);  sum += checksum(&temp, 2);
         temp = htons(payload_len);  sum += checksum(&temp, 2);
         if (d->proto == 6) {  /* TCP */
 	    libtrace_tcp_t *tcp_header = (libtrace_tcp_t *)d->dp;
            check = &tcp_header->check;
           }
         else if (d->proto == 17 ) {  /* UDP */
 	    libtrace_udp_t *udp_header = (libtrace_udp_t *)d->dp;
            check = &udp_header->check;
            }
	 else return -1;
         }
      }
   else {  /* v6 */
      libtrace_ip6_t *ip6 = (libtrace_ip6_t *)d->l3p;
      payload_len = d->rem;
      sum += checksum(&ip6->ip_src, 16);  /* 40-byte pseudo-header */
      sum += checksum(&ip6->ip_dst, 16);
      tmp32 = htonl(payload_len);  sum += checksum(&tmp32, 4);
      tmp32 = htons(d->proto);  sum += checksum(&tmp32, 4);
      if (d->proto == 6) {  /* TCP */
         libtrace_tcp_t *tcp_header = (libtrace_tcp_t *)d->dp;
         check = &tcp_header->check;
         }
      else if (d->proto == 17) {  /* UDP */
         libtrace_udp_t *udp_header =  (libtrace_udp_t *)d->dp;
         check = &udp_header->check;
         }
      else if (d->proto == 58) {  /* ICMPv6 */
         libtrace_icmp6_t *icmp_header =  (libtrace_icmp6_t *)d->dp;
         check = &icmp_header->checksum;
         }
      else return -1;
      }
   save_cks = *check;  *check = 0;
   libtrace_udp_t *ludp = (libtrace_udp_t *)d->dp;
   sum += checksum(d->dp, payload_len);
   while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
   sum = (uint16_t)~sum;
   if (reset_cks) { *check = sum;  return 1; }
   else { *check = save_cks;  return sum == save_cks; }
   }

int NO_COMPRESSION = TRACE_OPTION_COMPRESSTYPE_NONE;
int ZLIB_COMPRESSION = TRACE_OPTION_COMPRESSTYPE_ZLIB;
int ZIP2_COMPRESSION = TRACE_OPTION_COMPRESSTYPE_BZ2;
int LZO_COMPRESSION  = TRACE_OPTION_COMPRESSTYPE_LZO;

int OUT_FILE_FLAGS = TRACE_OPTION_OUTPUT_FILEFLAGS;
int OUT_COMP_TYPE = TRACE_OPTION_OUTPUT_COMPRESSTYPE;
int OUT_COMP_LEVEL = TRACE_OPTION_OUTPUT_COMPRESS;

int OPT_SNAPLEN = TRACE_OPTION_SNAPLEN;
int OPT_PROMISC = TRACE_OPTION_PROMISC;

int TRACE_ETH = TRACE_TYPE_ETH;
int TRACE_ATM = TRACE_TYPE_ATM;
int TRACE_80211 = TRACE_TYPE_80211;
int TRACE_NONE = TRACE_TYPE_NONE;
int TRACE_LINUX_SLL = TRACE_TYPE_LINUX_SLL;
int TRACE_PFLOG = TRACE_TYPE_PFLOG;
int TRACE_DUCK = TRACE_TYPE_DUCK;
int TRACE_80211_RADIO = TRACE_TYPE_80211_RADIO;
int TRACE_LLCSNAP = TRACE_TYPE_LLCSNAP;
int TRACE_PPP = TRACE_TYPE_PPP;
int TRACE_METADATA = TRACE_TYPE_METADATA;

int DIR_OUTGOING = TRACE_DIR_OUTGOING;
int DIR_INCOMING = TRACE_DIR_INCOMING;
int DIR_OTHER = TRACE_DIR_OTHER;
int DIR_UNKNOWN = TRACE_DIR_UNKNOWN;

int event_read_packet(struct libtrace_t *lt_trace, libtrace_packet_t *packet,
      long user_timeout) {  /* Seconds */
   libtrace_eventobj_t ev;
   fd_set fdset;  int maxfd = 0;
   struct timeval timeout, *tp = NULL;

   time_t idle_timeout, time_now = time(NULL);
   do {
      FD_ZERO(&fdset);
      ev = trace_event(lt_trace, packet);
      if (ev.type == TRACE_EVENT_PACKET) { // 2
	 if (ev.size > 0) return 1;  /* Read OK (or EOF) */
	 return -1;  /* Error */
	 }
      else if (ev.type == TRACE_EVENT_TERMINATE) { // 3
	 return 0;  /* Return as EOF */
	 }
      else if (ev.type == TRACE_EVENT_SLEEP) { // 1
	 /* Stop select when libtrace wants us to poll again
	    or when our timeout triggers, whichever comes first.
	    ev.seconds = time libtrace wants us to wait */
	 if (user_timeout == 0) {
	    timeout.tv_sec = (long)ev.seconds;
	    timeout.tv_usec =  (long)((ev.seconds-timeout.tv_sec)*1000000.0);
	    }
	 else {
	    timeout.tv_sec = user_timeout;
 	    if (ev.seconds < user_timeout) timeout.tv_sec = ev.seconds;
	    timeout.tv_usec = 0;
	    }
	 tp = &timeout;
	 }
      else if (ev.type == TRACE_EVENT_IOWAIT) {  // 0
 	 /* Wait for fd to be ready or until user timeout triggers */
	 if (user_timeout == 0)
	    tp = NULL;  /* Select won't time out */
	 else {
	    timeout.tv_sec = user_timeout;  timeout.tv_usec = 0;
	    tp = &timeout;
	    }
	 FD_SET(ev.fd, &fdset);	 maxfd = ev.fd;				
	 }
      idle_timeout = time_now + user_timeout;  /* time_t for timeout */
      select(maxfd+1, &fdset, NULL, NULL, tp);

      time_now = time(NULL);
      } while (time_now < idle_timeout);

   return -2;  /* Timeout */
   }

int set_config(struct libtrace_t *trace, int which, int val) {
    return trace_config(trace, which, &val);
   }
int set_config_filter(struct libtrace_t *trace,
      struct libtrace_filter_t *filter) {
   return trace_config(trace, TRACE_OPTION_FILTER, filter);
   }
int set_config_output(struct libtrace_out_t *trace, int which, int val) {
   return trace_config_output(trace, which, &val);
   }

uint16_t get_short(uint8_t *bp, uint16_t x) {
   uint16_t v = *(uint16_t *)&bp[x];
   return ntohs(v);
   }
void set_short(uint8_t *bp, int bx, uint16_t v) {
   *(uint16_t *)&bp[bx] = htons(v);
   }
uint32_t get_long(uint8_t *bp, uint32_t x) {
   uint32_t v = *(uint32_t *)&bp[x];
   return ntohl(v);
   }
void set_long(uint8_t *bp, int bx, uint32_t v) {
   *(uint32_t *)&bp[bx] = htonl(v);
   }
uint32_t get_bytes(uint8_t *bp, uint16_t x, int nbytes) {
   uint32_t a = 0;  uint8_t *ap = (uint8_t *)&a;
   memmove(&ap[4-nbytes], bp, nbytes);
   return a;
    }
void set_bytes(uint8_t *dstp, uint8_t *srcp, int x) {
   memmove(dstp, srcp, x);
   }
    """,
   libraries=["trace", "c"])  # list of libraries to link with

ffi.cdef(
    """
struct trace_err_t {  /* From libtrace.h */
   int err_num;
    char problem[255];  /* CFFI 'flexible' size */
   };

extern void trace_help(void);  /* External libtrace functions */
extern struct libtrace_t *trace_create(const char *uri);
extern struct libtrace_out_t *trace_create_output(const char *uri);
extern int trace_start(struct libtrace_t *trace);
extern int trace_start_output(struct libtrace_out_t *trace);
extern struct trace_err_t trace_get_err(struct libtrace_t *trace);
extern struct libtrace_packet_t *trace_create_packet(void);
extern struct trace_err_t trace_get_err_output(
   struct libtrace_out_t *trace);
extern int trace_read_packet(struct libtrace_t *trace,
    struct libtrace_packet_t *packet);
extern int trace_write_packet(struct libtrace_out_t *trace,
   struct libtrace_packet_t *packet);
extern int trace_pause(struct libtrace_t *libtrace);
extern void trace_destroy(struct libtrace_t *trace);
extern void trace_destroy_output(struct libtrace_out_t *trace);
extern struct libtrace_filter_t *trace_create_filter(char *filterstring);
extern int trace_apply_filter(struct libtrace_filter_t *filter,
   const struct libtrace_packet_t *packet);
extern uint64_t trace_get_dropped_packets(struct libtrace_t *trace);
extern uint64_t trace_get_accepted_packets(struct libtrace_t *trace);
extern int trace_get_direction(const struct libtrace_packet_t *packet);

struct pi {  /* DataObject info */
   int o_type;      /* Data type, RLT_TYPE values above */
   int o_kind;      /* Object kind, defined abve */
   uint8_t *data;   /* Pointer to the object's libtrace_packet_t or string */
   uint8_t *l2p;    /* Pointer to link-layer header */
   int l2_rem;      /* Bytes remaining at layer 2 */
   int linktype;    /* link type from original pkt, so we
		         can use trace_get_payload_from_layer2() */
   int ethertype;   /* So we can see its network protocol */
   int vlan_tag;    /* 802.1Q tag, NULL if there isn't one */
   uint8_t *l3p;    /* Pointer to the object's Internet struct */
   int l3_rem;      /* Remaining bytes for encapsulating python object */
   int proto;       /* Transport protocol */
   uint8_t *dp;     /* Pointer to an object within a DataObject
                         e.g. to a libtrace_tcp_t in a packet */
   int rem;         /* Number of bytes in the object */
   };
    """)

if __name__ == "__main__":
    ffi.compile()
