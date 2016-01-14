import os

from cffi import FFI

ffi = FFI()

ffi.cdef("int get_pkt_info(struct pi *spi, struct libtrace_packet_t *packet);")

ffi.set_source("plt",  # .so to be created
    """ // passed to the real C compiler
#include "libtrace.h"

struct pi {  /* DataObject info */
   int type;       /* Data type, RLT_TYPE values above */
   int kind;       /* Object kind, defined abve */
   uint8_t *data;  /* Pointer to the object's libtrace_packet_t or string */
   /* PyObject *mom;   Reference to original python object, used,
		        e.g., when we clone a libtrace pkt */
   uint8_t *l2p;   /* Pointer to link-layer header */
   int l2_rem;     /* Bytes remaining at layer 2 */
   int linktype;   /* link type from original pkt, so we
		        can use trace_get_payload_from_layer2() */
   int ethertype;  /* So we can see its network protocol */
   int vlan_tag;   /* 802.1Q tag, NULL if there isn't one */
   uint8_t *l3p;   /* Pointer to the object's Internet struct */
   int l3_rem;     /* Remaining bytes for encapsulating python object */
   int proto;      /* Transport protocol */
   uint8_t *dp;    /* Pointer to an object within a DataObject
                        e.g. to a libtrace_tcp_t in a packet */
   int rem;        /* Number of bytes in the object */
   };

int get_pkt_info(struct pi *spi, struct libtrace_packet_t *packet) {
   libtrace_linktype_t linktype;  uint32_t l2_rem; int vlan = 0;
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
      if (!vlp) return -4;
      ethertype = vlan_et;
      l2_rem -= vlp-l3p;  l3p = vlp;  l3_rem = vlan_rem;
      }
   spi->l2_rem = l2_rem;  spi->l2p = l2p;
   spi->linktype = linktype;
   spi->ethertype = ethertype;  spi->vlan_tag = vlan;
   spi->l3_rem = l3_rem;  spi->l3p = l3p;
   return l3_rem > 0;
   }

struct libtrace_t *cr;
    """,
   libraries=["trace", "c"])  # list of libraries to link with

# ffi.new("struct pi *")

ffi.cdef(
    """
extern void trace_help(void);  /* External libtrace functions */
extern struct libtrace_t *trace_create(const char *uri);
extern int trace_start(struct libtrace_t *trace);
/* extern libtrace_err_t trace_get_err(libtrace_t *trace);
  Don't know how to handle function returning a struct! */
extern struct libtrace_packet_t *trace_create_packet(void);
extern int trace_read_packet(struct libtrace_t *trace,
    struct libtrace_packet_t *packet);

struct pi {  /* DataObject info */
   int type;       /* Data type, RLT_TYPE values above */
   int kind;       /* Object kind, defined abve */
   uint8_t *data;  /* Pointer to the object's libtrace_packet_t or string */
   /* PyObject *mom;   Reference to original python object, used,
		        e.g., when we clone a libtrace pkt */
   uint8_t *l2p;   /* Pointer to link-layer header */
   int l2_rem;     /* Bytes remaining at layer 2 */
   int linktype;   /* link type from original pkt, so we
		        can use trace_get_payload_from_layer2() */
   int ethertype;  /* So we can see its network protocol */
   int vlan_tag;   /* 802.1Q tag, NULL if there isn't one */
   uint8_t *l3p;   /* Pointer to the object's Internet struct */
   int l3_rem;     /* Remaining bytes for encapsulating python object */
   int proto;      /* Transport protocol */
   uint8_t *dp;    /* Pointer to an object within a DataObject
                        e.g. to a libtrace_tcp_t in a packet */
   int rem;        /* Number of bytes in the object */
   };
    """)

PLTversion    = "2.0"  # 23 Nov 15 (NZDT)

KIND_PKT      =  1  # data points to a libtrace_packet_t
KIND_STR      =  2  # data points to a C byte array
KIND_CPY      =  3  # data points to another (MOM) rlt_obj
def bad_kind(v):
    return v < KIND_PKT or v > KIND_COPY

kind_strings = {KIND_PKT:"Packet", KIND_STR:"String", KIND_CPY:"copy"}

def plt_kind_string(k):
    v = kind_strings(t)
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
    TYPE_L4:"Layer4", TYPE_TCP:"TCP", TYPE_UDP:"UDP",
    TYPE_L5:"Layer5", TYPE_ICMP:"ICMP", TYPE_ICMP6:"ICMP6"}

def plt_type(t):
    v = type_strings(t)
    if v:
        return v
    return "unknown"

def pltData_dump(obj, msg):
   print("%s:  %s, %s, data=%p, mom=%p\n" % (
      msg, plt_type_string(obj.type), plt_kind_string(obj.kind),
	  obj.data, obj.mom))
   print("    l2p=%p, %d, %d, %04x, %04x\n" % (
      obj.l2p, obj.l2_rem, obj.linktype,
      obj.ethertype, obj.vlan_tag))
   print("    l3p=%p, %d, %d,  dp=%p, %d\n" % (
      obj.l3p, obj.l3_rem, obj.proto, obj.dp, obj.rem))

if __name__ == "__main__":
    ffi.compile()
