#define _CFFI_
#include <Python.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

/* See doc/misc/parse_c_type.rst in the source of CFFI for more information */

typedef void *_cffi_opcode_t;

#define _CFFI_OP(opcode, arg)   (_cffi_opcode_t)(opcode | (((uintptr_t)(arg)) << 8))
#define _CFFI_GETOP(cffi_opcode)    ((unsigned char)(uintptr_t)cffi_opcode)
#define _CFFI_GETARG(cffi_opcode)   (((intptr_t)cffi_opcode) >> 8)

#define _CFFI_OP_PRIMITIVE       1
#define _CFFI_OP_POINTER         3
#define _CFFI_OP_ARRAY           5
#define _CFFI_OP_OPEN_ARRAY      7
#define _CFFI_OP_STRUCT_UNION    9
#define _CFFI_OP_ENUM           11
#define _CFFI_OP_FUNCTION       13
#define _CFFI_OP_FUNCTION_END   15
#define _CFFI_OP_NOOP           17
#define _CFFI_OP_BITFIELD       19
#define _CFFI_OP_TYPENAME       21
#define _CFFI_OP_CPYTHON_BLTN_V 23   // varargs
#define _CFFI_OP_CPYTHON_BLTN_N 25   // noargs
#define _CFFI_OP_CPYTHON_BLTN_O 27   // O  (i.e. a single arg)
#define _CFFI_OP_CONSTANT       29
#define _CFFI_OP_CONSTANT_INT   31
#define _CFFI_OP_GLOBAL_VAR     33
#define _CFFI_OP_DLOPEN_FUNC    35
#define _CFFI_OP_DLOPEN_CONST   37
#define _CFFI_OP_GLOBAL_VAR_F   39

#define _CFFI_PRIM_VOID          0
#define _CFFI_PRIM_BOOL          1
#define _CFFI_PRIM_CHAR          2
#define _CFFI_PRIM_SCHAR         3
#define _CFFI_PRIM_UCHAR         4
#define _CFFI_PRIM_SHORT         5
#define _CFFI_PRIM_USHORT        6
#define _CFFI_PRIM_INT           7
#define _CFFI_PRIM_UINT          8
#define _CFFI_PRIM_LONG          9
#define _CFFI_PRIM_ULONG        10
#define _CFFI_PRIM_LONGLONG     11
#define _CFFI_PRIM_ULONGLONG    12
#define _CFFI_PRIM_FLOAT        13
#define _CFFI_PRIM_DOUBLE       14
#define _CFFI_PRIM_LONGDOUBLE   15

#define _CFFI_PRIM_WCHAR        16
#define _CFFI_PRIM_INT8         17
#define _CFFI_PRIM_UINT8        18
#define _CFFI_PRIM_INT16        19
#define _CFFI_PRIM_UINT16       20
#define _CFFI_PRIM_INT32        21
#define _CFFI_PRIM_UINT32       22
#define _CFFI_PRIM_INT64        23
#define _CFFI_PRIM_UINT64       24
#define _CFFI_PRIM_INTPTR       25
#define _CFFI_PRIM_UINTPTR      26
#define _CFFI_PRIM_PTRDIFF      27
#define _CFFI_PRIM_SIZE         28
#define _CFFI_PRIM_SSIZE        29
#define _CFFI_PRIM_INT_LEAST8   30
#define _CFFI_PRIM_UINT_LEAST8  31
#define _CFFI_PRIM_INT_LEAST16  32
#define _CFFI_PRIM_UINT_LEAST16 33
#define _CFFI_PRIM_INT_LEAST32  34
#define _CFFI_PRIM_UINT_LEAST32 35
#define _CFFI_PRIM_INT_LEAST64  36
#define _CFFI_PRIM_UINT_LEAST64 37
#define _CFFI_PRIM_INT_FAST8    38
#define _CFFI_PRIM_UINT_FAST8   39
#define _CFFI_PRIM_INT_FAST16   40
#define _CFFI_PRIM_UINT_FAST16  41
#define _CFFI_PRIM_INT_FAST32   42
#define _CFFI_PRIM_UINT_FAST32  43
#define _CFFI_PRIM_INT_FAST64   44
#define _CFFI_PRIM_UINT_FAST64  45
#define _CFFI_PRIM_INTMAX       46
#define _CFFI_PRIM_UINTMAX      47

#define _CFFI__NUM_PRIM         48
#define _CFFI__UNKNOWN_PRIM           (-1)
#define _CFFI__UNKNOWN_FLOAT_PRIM     (-2)
#define _CFFI__UNKNOWN_LONG_DOUBLE    (-3)


struct _cffi_global_s {
    const char *name;
    void *address;
    _cffi_opcode_t type_op;
    void *size_or_direct_fn;  // OP_GLOBAL_VAR: size, or 0 if unknown
                              // OP_CPYTHON_BLTN_*: addr of direct function
};

struct _cffi_getconst_s {
    unsigned long long value;
    const struct _cffi_type_context_s *ctx;
    int gindex;
};

struct _cffi_struct_union_s {
    const char *name;
    int type_index;          // -> _cffi_types, on a OP_STRUCT_UNION
    int flags;               // _CFFI_F_* flags below
    size_t size;
    int alignment;
    int first_field_index;   // -> _cffi_fields array
    int num_fields;
};
#define _CFFI_F_UNION         0x01   // is a union, not a struct
#define _CFFI_F_CHECK_FIELDS  0x02   // complain if fields are not in the
                                     // "standard layout" or if some are missing
#define _CFFI_F_PACKED        0x04   // for CHECK_FIELDS, assume a packed struct
#define _CFFI_F_EXTERNAL      0x08   // in some other ffi.include()
#define _CFFI_F_OPAQUE        0x10   // opaque

struct _cffi_field_s {
    const char *name;
    size_t field_offset;
    size_t field_size;
    _cffi_opcode_t field_type_op;
};

struct _cffi_enum_s {
    const char *name;
    int type_index;          // -> _cffi_types, on a OP_ENUM
    int type_prim;           // _CFFI_PRIM_xxx
    const char *enumerators; // comma-delimited string
};

struct _cffi_typename_s {
    const char *name;
    int type_index;   /* if opaque, points to a possibly artificial
                         OP_STRUCT which is itself opaque */
};

struct _cffi_type_context_s {
    _cffi_opcode_t *types;
    const struct _cffi_global_s *globals;
    const struct _cffi_field_s *fields;
    const struct _cffi_struct_union_s *struct_unions;
    const struct _cffi_enum_s *enums;
    const struct _cffi_typename_s *typenames;
    int num_globals;
    int num_struct_unions;
    int num_enums;
    int num_typenames;
    const char *const *includes;
    int num_types;
    int flags;      /* future extension */
};

struct _cffi_parse_info_s {
    const struct _cffi_type_context_s *ctx;
    _cffi_opcode_t *output;
    unsigned int output_size;
    size_t error_location;
    const char *error_message;
};

#ifdef _CFFI_INTERNAL
static int parse_c_type(struct _cffi_parse_info_s *info, const char *input);
static int search_in_globals(const struct _cffi_type_context_s *ctx,
                             const char *search, size_t search_len);
static int search_in_struct_unions(const struct _cffi_type_context_s *ctx,
                                   const char *search, size_t search_len);
#endif

/* this block of #ifs should be kept exactly identical between
   c/_cffi_backend.c, cffi/vengine_cpy.py, cffi/vengine_gen.py
   and cffi/_cffi_include.h */
#if defined(_MSC_VER)
# include <malloc.h>   /* for alloca() */
# if _MSC_VER < 1600   /* MSVC < 2010 */
   typedef __int8 int8_t;
   typedef __int16 int16_t;
   typedef __int32 int32_t;
   typedef __int64 int64_t;
   typedef unsigned __int8 uint8_t;
   typedef unsigned __int16 uint16_t;
   typedef unsigned __int32 uint32_t;
   typedef unsigned __int64 uint64_t;
   typedef __int8 int_least8_t;
   typedef __int16 int_least16_t;
   typedef __int32 int_least32_t;
   typedef __int64 int_least64_t;
   typedef unsigned __int8 uint_least8_t;
   typedef unsigned __int16 uint_least16_t;
   typedef unsigned __int32 uint_least32_t;
   typedef unsigned __int64 uint_least64_t;
   typedef __int8 int_fast8_t;
   typedef __int16 int_fast16_t;
   typedef __int32 int_fast32_t;
   typedef __int64 int_fast64_t;
   typedef unsigned __int8 uint_fast8_t;
   typedef unsigned __int16 uint_fast16_t;
   typedef unsigned __int32 uint_fast32_t;
   typedef unsigned __int64 uint_fast64_t;
   typedef __int64 intmax_t;
   typedef unsigned __int64 uintmax_t;
# else
#  include <stdint.h>
# endif
# if _MSC_VER < 1800   /* MSVC < 2013 */
   typedef unsigned char _Bool;
# endif
#else
# include <stdint.h>
# if (defined (__SVR4) && defined (__sun)) || defined(_AIX) || defined(__hpux)
#  include <alloca.h>
# endif
#endif

#ifdef __GNUC__
# define _CFFI_UNUSED_FN  __attribute__((unused))
#else
# define _CFFI_UNUSED_FN  /* nothing */
#endif

/**********  CPython-specific section  **********/
#ifndef PYPY_VERSION


#if PY_MAJOR_VERSION >= 3
# define PyInt_FromLong PyLong_FromLong
#endif

#define _cffi_from_c_double PyFloat_FromDouble
#define _cffi_from_c_float PyFloat_FromDouble
#define _cffi_from_c_long PyInt_FromLong
#define _cffi_from_c_ulong PyLong_FromUnsignedLong
#define _cffi_from_c_longlong PyLong_FromLongLong
#define _cffi_from_c_ulonglong PyLong_FromUnsignedLongLong

#define _cffi_to_c_double PyFloat_AsDouble
#define _cffi_to_c_float PyFloat_AsDouble

#define _cffi_from_c_int(x, type)                                        \
    (((type)-1) > 0 ? /* unsigned */                                     \
        (sizeof(type) < sizeof(long) ?                                   \
            PyInt_FromLong((long)x) :                                    \
         sizeof(type) == sizeof(long) ?                                  \
            PyLong_FromUnsignedLong((unsigned long)x) :                  \
            PyLong_FromUnsignedLongLong((unsigned long long)x)) :        \
        (sizeof(type) <= sizeof(long) ?                                  \
            PyInt_FromLong((long)x) :                                    \
            PyLong_FromLongLong((long long)x)))

#define _cffi_to_c_int(o, type)                                          \
    ((type)(                                                             \
     sizeof(type) == 1 ? (((type)-1) > 0 ? (type)_cffi_to_c_u8(o)        \
                                         : (type)_cffi_to_c_i8(o)) :     \
     sizeof(type) == 2 ? (((type)-1) > 0 ? (type)_cffi_to_c_u16(o)       \
                                         : (type)_cffi_to_c_i16(o)) :    \
     sizeof(type) == 4 ? (((type)-1) > 0 ? (type)_cffi_to_c_u32(o)       \
                                         : (type)_cffi_to_c_i32(o)) :    \
     sizeof(type) == 8 ? (((type)-1) > 0 ? (type)_cffi_to_c_u64(o)       \
                                         : (type)_cffi_to_c_i64(o)) :    \
     (Py_FatalError("unsupported size for type " #type), (type)0)))

#define _cffi_to_c_i8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[1])
#define _cffi_to_c_u8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[2])
#define _cffi_to_c_i16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[3])
#define _cffi_to_c_u16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[4])
#define _cffi_to_c_i32                                                   \
                 ((int(*)(PyObject *))_cffi_exports[5])
#define _cffi_to_c_u32                                                   \
                 ((unsigned int(*)(PyObject *))_cffi_exports[6])
#define _cffi_to_c_i64                                                   \
                 ((long long(*)(PyObject *))_cffi_exports[7])
#define _cffi_to_c_u64                                                   \
                 ((unsigned long long(*)(PyObject *))_cffi_exports[8])
#define _cffi_to_c_char                                                  \
                 ((int(*)(PyObject *))_cffi_exports[9])
#define _cffi_from_c_pointer                                             \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[10])
#define _cffi_to_c_pointer                                               \
    ((char *(*)(PyObject *, CTypeDescrObject *))_cffi_exports[11])
#define _cffi_get_struct_layout                                          \
    not used any more
#define _cffi_restore_errno                                              \
    ((void(*)(void))_cffi_exports[13])
#define _cffi_save_errno                                                 \
    ((void(*)(void))_cffi_exports[14])
#define _cffi_from_c_char                                                \
    ((PyObject *(*)(char))_cffi_exports[15])
#define _cffi_from_c_deref                                               \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[16])
#define _cffi_to_c                                                       \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[17])
#define _cffi_from_c_struct                                              \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[18])
#define _cffi_to_c_wchar_t                                               \
    ((wchar_t(*)(PyObject *))_cffi_exports[19])
#define _cffi_from_c_wchar_t                                             \
    ((PyObject *(*)(wchar_t))_cffi_exports[20])
#define _cffi_to_c_long_double                                           \
    ((long double(*)(PyObject *))_cffi_exports[21])
#define _cffi_to_c__Bool                                                 \
    ((_Bool(*)(PyObject *))_cffi_exports[22])
#define _cffi_prepare_pointer_call_argument                              \
    ((Py_ssize_t(*)(CTypeDescrObject *, PyObject *, char **))_cffi_exports[23])
#define _cffi_convert_array_from_object                                  \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[24])
#define _CFFI_NUM_EXPORTS 25

typedef struct _ctypedescr CTypeDescrObject;

static void *_cffi_exports[_CFFI_NUM_EXPORTS];

#define _cffi_type(index)   (                           \
    assert((((uintptr_t)_cffi_types[index]) & 1) == 0), \
    (CTypeDescrObject *)_cffi_types[index])

static PyObject *_cffi_init(const char *module_name, Py_ssize_t version,
                            const struct _cffi_type_context_s *ctx)
{
    PyObject *module, *o_arg, *new_module;
    void *raw[] = {
        (void *)module_name,
        (void *)version,
        (void *)_cffi_exports,
        (void *)ctx,
    };

    module = PyImport_ImportModule("_cffi_backend");
    if (module == NULL)
        goto failure;

    o_arg = PyLong_FromVoidPtr((void *)raw);
    if (o_arg == NULL)
        goto failure;

    new_module = PyObject_CallMethod(
        module, (char *)"_init_cffi_1_0_external_module", (char *)"O", o_arg);

    Py_DECREF(o_arg);
    Py_DECREF(module);
    return new_module;

  failure:
    Py_XDECREF(module);
    return NULL;
}

_CFFI_UNUSED_FN
static PyObject **_cffi_unpack_args(PyObject *args_tuple, Py_ssize_t expected,
                                    const char *fnname)
{
    if (PyTuple_GET_SIZE(args_tuple) != expected) {
        PyErr_Format(PyExc_TypeError,
                     "%.150s() takes exactly %zd arguments (%zd given)",
                     fnname, expected, PyTuple_GET_SIZE(args_tuple));
        return NULL;
    }
    return &PyTuple_GET_ITEM(args_tuple, 0);   /* pointer to the first item,
                                                  the others follow */
}

#endif
/**********  end CPython-specific section  **********/


#define _cffi_array_len(array)   (sizeof(array) / sizeof((array)[0]))

#define _cffi_prim_int(size, sign)                                      \
    ((size) == 1 ? ((sign) ? _CFFI_PRIM_INT8  : _CFFI_PRIM_UINT8)  :    \
     (size) == 2 ? ((sign) ? _CFFI_PRIM_INT16 : _CFFI_PRIM_UINT16) :    \
     (size) == 4 ? ((sign) ? _CFFI_PRIM_INT32 : _CFFI_PRIM_UINT32) :    \
     (size) == 8 ? ((sign) ? _CFFI_PRIM_INT64 : _CFFI_PRIM_UINT64) :    \
     _CFFI__UNKNOWN_PRIM)

#define _cffi_prim_float(size)                                          \
    ((size) == sizeof(float) ? _CFFI_PRIM_FLOAT :                       \
     (size) == sizeof(double) ? _CFFI_PRIM_DOUBLE :                     \
     (size) == sizeof(long double) ? _CFFI__UNKNOWN_LONG_DOUBLE :       \
     _CFFI__UNKNOWN_FLOAT_PRIM)

#define _cffi_check_int(got, got_nonpos, expected)      \
    ((got_nonpos) == (expected <= 0) &&                 \
     (got) == (unsigned long long)expected)

#ifdef __cplusplus
}
#endif

/************************************************************/

 // passed to the real C compiler
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
    

/************************************************************/

static void *_cffi_types[] = {
/*  0 */ _CFFI_OP(_CFFI_OP_FUNCTION, 112), // double()(struct pi *)
/*  1 */ _CFFI_OP(_CFFI_OP_POINTER, 117), // struct pi *
/*  2 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  3 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_filter_t *, struct libtrace_packet_t const *)
/*  4 */ _CFFI_OP(_CFFI_OP_POINTER, 113), // struct libtrace_filter_t *
/*  5 */ _CFFI_OP(_CFFI_OP_POINTER, 115), // struct libtrace_packet_t const *
/*  6 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  7 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_out_t *)
/*  8 */ _CFFI_OP(_CFFI_OP_POINTER, 114), // struct libtrace_out_t *
/*  9 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 10 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_out_t *, int, int)
/* 11 */ _CFFI_OP(_CFFI_OP_NOOP, 8),
/* 12 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7), // int
/* 13 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 14 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 15 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_out_t *, struct libtrace_packet_t *)
/* 16 */ _CFFI_OP(_CFFI_OP_NOOP, 8),
/* 17 */ _CFFI_OP(_CFFI_OP_POINTER, 115), // struct libtrace_packet_t *
/* 18 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 19 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_t *)
/* 20 */ _CFFI_OP(_CFFI_OP_POINTER, 116), // struct libtrace_t *
/* 21 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 22 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_t *, int, int)
/* 23 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 24 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 25 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 26 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 27 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_t *, struct libtrace_filter_t *)
/* 28 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 29 */ _CFFI_OP(_CFFI_OP_NOOP, 4),
/* 30 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 31 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct libtrace_t *, struct libtrace_packet_t *)
/* 32 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 33 */ _CFFI_OP(_CFFI_OP_NOOP, 17),
/* 34 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 35 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct pi *)
/* 36 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 37 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 38 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct pi *, int)
/* 39 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 40 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 41 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 42 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct pi *, struct libtrace_packet_t *)
/* 43 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 44 */ _CFFI_OP(_CFFI_OP_NOOP, 17),
/* 45 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 46 */ _CFFI_OP(_CFFI_OP_FUNCTION, 12), // int()(struct pi *, struct pi *)
/* 47 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 48 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 49 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 50 */ _CFFI_OP(_CFFI_OP_FUNCTION, 4), // struct libtrace_filter_t *()(char *)
/* 51 */ _CFFI_OP(_CFFI_OP_POINTER, 109), // char *
/* 52 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 53 */ _CFFI_OP(_CFFI_OP_FUNCTION, 8), // struct libtrace_out_t *()(char const *)
/* 54 */ _CFFI_OP(_CFFI_OP_POINTER, 109), // char const *
/* 55 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 56 */ _CFFI_OP(_CFFI_OP_FUNCTION, 17), // struct libtrace_packet_t *()(void)
/* 57 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 58 */ _CFFI_OP(_CFFI_OP_FUNCTION, 20), // struct libtrace_t *()(char const *)
/* 59 */ _CFFI_OP(_CFFI_OP_NOOP, 54),
/* 60 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 61 */ _CFFI_OP(_CFFI_OP_FUNCTION, 118), // struct trace_err_t()(struct libtrace_out_t *)
/* 62 */ _CFFI_OP(_CFFI_OP_NOOP, 8),
/* 63 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 64 */ _CFFI_OP(_CFFI_OP_FUNCTION, 118), // struct trace_err_t()(struct libtrace_t *)
/* 65 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 66 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 67 */ _CFFI_OP(_CFFI_OP_FUNCTION, 69), // uint16_t()(uint8_t *, uint16_t)
/* 68 */ _CFFI_OP(_CFFI_OP_POINTER, 120), // uint8_t *
/* 69 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 20), // uint16_t
/* 70 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 71 */ _CFFI_OP(_CFFI_OP_FUNCTION, 100), // uint32_t()(uint8_t *, int32_t)
/* 72 */ _CFFI_OP(_CFFI_OP_NOOP, 68),
/* 73 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 21), // int32_t
/* 74 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 75 */ _CFFI_OP(_CFFI_OP_FUNCTION, 100), // uint32_t()(uint8_t *, uint16_t, int)
/* 76 */ _CFFI_OP(_CFFI_OP_NOOP, 68),
/* 77 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 20),
/* 78 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 79 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 80 */ _CFFI_OP(_CFFI_OP_FUNCTION, 119), // uint64_t()(struct libtrace_t *)
/* 81 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 82 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 83 */ _CFFI_OP(_CFFI_OP_FUNCTION, 119), // uint64_t()(struct pi *)
/* 84 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 85 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 86 */ _CFFI_OP(_CFFI_OP_FUNCTION, 121), // void()(struct libtrace_out_t *)
/* 87 */ _CFFI_OP(_CFFI_OP_NOOP, 8),
/* 88 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 89 */ _CFFI_OP(_CFFI_OP_FUNCTION, 121), // void()(struct libtrace_t *)
/* 90 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 91 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 92 */ _CFFI_OP(_CFFI_OP_FUNCTION, 121), // void()(uint8_t *, int, uint16_t)
/* 93 */ _CFFI_OP(_CFFI_OP_NOOP, 68),
/* 94 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 95 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 20),
/* 96 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 97 */ _CFFI_OP(_CFFI_OP_FUNCTION, 121), // void()(uint8_t *, int, uint32_t)
/* 98 */ _CFFI_OP(_CFFI_OP_NOOP, 68),
/* 99 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 100 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 22), // uint32_t
/* 101 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 102 */ _CFFI_OP(_CFFI_OP_FUNCTION, 121), // void()(uint8_t *, uint8_t *, int)
/* 103 */ _CFFI_OP(_CFFI_OP_NOOP, 68),
/* 104 */ _CFFI_OP(_CFFI_OP_NOOP, 68),
/* 105 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 106 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 107 */ _CFFI_OP(_CFFI_OP_FUNCTION, 121), // void()(void)
/* 108 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 109 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 2), // char
/* 110 */ _CFFI_OP(_CFFI_OP_ARRAY, 109), // char[255]
/* 111 */ (_cffi_opcode_t)(255),
/* 112 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 14), // double
/* 113 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 0), // struct libtrace_filter_t
/* 114 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 1), // struct libtrace_out_t
/* 115 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 2), // struct libtrace_packet_t
/* 116 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 3), // struct libtrace_t
/* 117 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 4), // struct pi
/* 118 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 5), // struct trace_err_t
/* 119 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 24), // uint64_t
/* 120 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 18), // uint8_t
/* 121 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 0), // void
};

static uint32_t _cffi_d_get_bytes(uint8_t * x0, uint16_t x1, int x2)
{
  return get_bytes(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_bytes(PyObject *self, PyObject *args)
{
  uint8_t * x0;
  uint16_t x1;
  int x2;
  Py_ssize_t datasize;
  uint32_t result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "get_bytes");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(68), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(68), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, uint16_t);
  if (x1 == (uint16_t)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_bytes(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint32_t);
}
#else
#  define _cffi_f_get_bytes _cffi_d_get_bytes
#endif

static int _cffi_d_get_capture_len(struct pi * x0)
{
  return get_capture_len(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_capture_len(PyObject *self, PyObject *arg0)
{
  struct pi * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_capture_len(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_capture_len _cffi_d_get_capture_len
#endif

static int _cffi_d_get_direction(struct pi * x0)
{
  return get_direction(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_direction(PyObject *self, PyObject *arg0)
{
  struct pi * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_direction(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_direction _cffi_d_get_direction
#endif

static uint64_t _cffi_d_get_erf_time(struct pi * x0)
{
  return get_erf_time(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_erf_time(PyObject *self, PyObject *arg0)
{
  struct pi * x0;
  Py_ssize_t datasize;
  uint64_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_erf_time(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint64_t);
}
#else
#  define _cffi_f_get_erf_time _cffi_d_get_erf_time
#endif

static uint32_t _cffi_d_get_long(uint8_t * x0, int32_t x1)
{
  return get_long(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_long(PyObject *self, PyObject *args)
{
  uint8_t * x0;
  int32_t x1;
  Py_ssize_t datasize;
  uint32_t result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "get_long");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(68), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(68), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int32_t);
  if (x1 == (int32_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_long(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint32_t);
}
#else
#  define _cffi_f_get_long _cffi_d_get_long
#endif

static int _cffi_d_get_pkt_info(struct pi * x0, struct libtrace_packet_t * x1)
{
  return get_pkt_info(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_pkt_info(PyObject *self, PyObject *args)
{
  struct pi * x0;
  struct libtrace_packet_t * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "get_pkt_info");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(17), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (struct libtrace_packet_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(17), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_pkt_info(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_pkt_info _cffi_d_get_pkt_info
#endif

static uint16_t _cffi_d_get_short(uint8_t * x0, uint16_t x1)
{
  return get_short(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_short(PyObject *self, PyObject *args)
{
  uint8_t * x0;
  uint16_t x1;
  Py_ssize_t datasize;
  uint16_t result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "get_short");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(68), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(68), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, uint16_t);
  if (x1 == (uint16_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_short(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint16_t);
}
#else
#  define _cffi_f_get_short _cffi_d_get_short
#endif

static double _cffi_d_get_time(struct pi * x0)
{
  return get_time(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_time(PyObject *self, PyObject *arg0)
{
  struct pi * x0;
  Py_ssize_t datasize;
  double result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_time(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_double(result);
}
#else
#  define _cffi_f_get_time _cffi_d_get_time
#endif

static int _cffi_d_get_trans_payload(struct pi * x0, struct pi * x1)
{
  return get_trans_payload(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_trans_payload(PyObject *self, PyObject *args)
{
  struct pi * x0;
  struct pi * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "get_trans_payload");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_trans_payload(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_trans_payload _cffi_d_get_trans_payload
#endif

static int _cffi_d_get_transport_info(struct pi * x0, struct pi * x1)
{
  return get_transport_info(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_transport_info(PyObject *self, PyObject *args)
{
  struct pi * x0;
  struct pi * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "get_transport_info");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_transport_info(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_transport_info _cffi_d_get_transport_info
#endif

static int _cffi_d_get_ts_sec(struct pi * x0)
{
  return get_ts_sec(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_ts_sec(PyObject *self, PyObject *arg0)
{
  struct pi * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_ts_sec(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_ts_sec _cffi_d_get_ts_sec
#endif

static int _cffi_d_get_wire_len(struct pi * x0)
{
  return get_wire_len(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_wire_len(PyObject *self, PyObject *arg0)
{
  struct pi * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_wire_len(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_wire_len _cffi_d_get_wire_len
#endif

static int _cffi_d_l3_checksum(struct pi * x0, int x1)
{
  return l3_checksum(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_l3_checksum(PyObject *self, PyObject *args)
{
  struct pi * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "l3_checksum");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = l3_checksum(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_l3_checksum _cffi_d_l3_checksum
#endif

static void _cffi_d_set_bytes(uint8_t * x0, uint8_t * x1, int x2)
{
  set_bytes(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_set_bytes(PyObject *self, PyObject *args)
{
  uint8_t * x0;
  uint8_t * x1;
  int x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "set_bytes");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(68), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(68), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(68), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(68), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { set_bytes(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_set_bytes _cffi_d_set_bytes
#endif

static int _cffi_d_set_config(struct libtrace_t * x0, int x1, int x2)
{
  return set_config(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_set_config(PyObject *self, PyObject *args)
{
  struct libtrace_t * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "set_config");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = set_config(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_set_config _cffi_d_set_config
#endif

static int _cffi_d_set_config_filter(struct libtrace_t * x0, struct libtrace_filter_t * x1)
{
  return set_config_filter(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_set_config_filter(PyObject *self, PyObject *args)
{
  struct libtrace_t * x0;
  struct libtrace_filter_t * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "set_config_filter");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (struct libtrace_filter_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(4), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = set_config_filter(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_set_config_filter _cffi_d_set_config_filter
#endif

static int _cffi_d_set_config_output(struct libtrace_out_t * x0, int x1, int x2)
{
  return set_config_output(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_set_config_output(PyObject *self, PyObject *args)
{
  struct libtrace_out_t * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "set_config_output");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_out_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = set_config_output(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_set_config_output _cffi_d_set_config_output
#endif

static void _cffi_d_set_long(uint8_t * x0, int x1, uint32_t x2)
{
  set_long(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_set_long(PyObject *self, PyObject *args)
{
  uint8_t * x0;
  int x1;
  uint32_t x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "set_long");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(68), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(68), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, uint32_t);
  if (x2 == (uint32_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { set_long(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_set_long _cffi_d_set_long
#endif

static void _cffi_d_set_short(uint8_t * x0, int x1, uint16_t x2)
{
  set_short(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_set_short(PyObject *self, PyObject *args)
{
  uint8_t * x0;
  int x1;
  uint16_t x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "set_short");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(68), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(68), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, uint16_t);
  if (x2 == (uint16_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { set_short(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_set_short _cffi_d_set_short
#endif

static int _cffi_d_trace_apply_filter(struct libtrace_filter_t * x0, struct libtrace_packet_t const * x1)
{
  return trace_apply_filter(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_apply_filter(PyObject *self, PyObject *args)
{
  struct libtrace_filter_t * x0;
  struct libtrace_packet_t const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "trace_apply_filter");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_filter_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(4), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(5), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (struct libtrace_packet_t const *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(5), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_apply_filter(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_trace_apply_filter _cffi_d_trace_apply_filter
#endif

static struct libtrace_t * _cffi_d_trace_create(char const * x0)
{
  return trace_create(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_create(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  struct libtrace_t * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(54), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (char const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(54), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_create(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(20));
}
#else
#  define _cffi_f_trace_create _cffi_d_trace_create
#endif

static struct libtrace_filter_t * _cffi_d_trace_create_filter(char * x0)
{
  return trace_create_filter(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_create_filter(PyObject *self, PyObject *arg0)
{
  char * x0;
  Py_ssize_t datasize;
  struct libtrace_filter_t * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(51), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (char *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(51), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_create_filter(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(4));
}
#else
#  define _cffi_f_trace_create_filter _cffi_d_trace_create_filter
#endif

static struct libtrace_out_t * _cffi_d_trace_create_output(char const * x0)
{
  return trace_create_output(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_create_output(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  struct libtrace_out_t * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(54), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (char const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(54), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_create_output(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(8));
}
#else
#  define _cffi_f_trace_create_output _cffi_d_trace_create_output
#endif

static struct libtrace_packet_t * _cffi_d_trace_create_packet(void)
{
  return trace_create_packet();
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_create_packet(PyObject *self, PyObject *noarg)
{
  struct libtrace_packet_t * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_create_packet(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  (void)noarg; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(17));
}
#else
#  define _cffi_f_trace_create_packet _cffi_d_trace_create_packet
#endif

static void _cffi_d_trace_destroy(struct libtrace_t * x0)
{
  trace_destroy(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_destroy(PyObject *self, PyObject *arg0)
{
  struct libtrace_t * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { trace_destroy(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_trace_destroy _cffi_d_trace_destroy
#endif

static void _cffi_d_trace_destroy_output(struct libtrace_out_t * x0)
{
  trace_destroy_output(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_destroy_output(PyObject *self, PyObject *arg0)
{
  struct libtrace_out_t * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_out_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { trace_destroy_output(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_trace_destroy_output _cffi_d_trace_destroy_output
#endif

static uint64_t _cffi_d_trace_get_accepted_packets(struct libtrace_t * x0)
{
  return trace_get_accepted_packets(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_get_accepted_packets(PyObject *self, PyObject *arg0)
{
  struct libtrace_t * x0;
  Py_ssize_t datasize;
  uint64_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_get_accepted_packets(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint64_t);
}
#else
#  define _cffi_f_trace_get_accepted_packets _cffi_d_trace_get_accepted_packets
#endif

static uint64_t _cffi_d_trace_get_dropped_packets(struct libtrace_t * x0)
{
  return trace_get_dropped_packets(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_get_dropped_packets(PyObject *self, PyObject *arg0)
{
  struct libtrace_t * x0;
  Py_ssize_t datasize;
  uint64_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_get_dropped_packets(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint64_t);
}
#else
#  define _cffi_f_trace_get_dropped_packets _cffi_d_trace_get_dropped_packets
#endif

static struct trace_err_t _cffi_d_trace_get_err(struct libtrace_t * x0)
{
  return trace_get_err(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_get_err(PyObject *self, PyObject *arg0)
{
  struct libtrace_t * x0;
  Py_ssize_t datasize;
  struct trace_err_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_get_err(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_struct((char *)&result, _cffi_type(118));
}
#else
static void _cffi_f_trace_get_err(struct trace_err_t *result, struct libtrace_t * x0)
{
  { *result = trace_get_err(x0); }
}
#endif

static struct trace_err_t _cffi_d_trace_get_err_output(struct libtrace_out_t * x0)
{
  return trace_get_err_output(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_get_err_output(PyObject *self, PyObject *arg0)
{
  struct libtrace_out_t * x0;
  Py_ssize_t datasize;
  struct trace_err_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_out_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_get_err_output(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_struct((char *)&result, _cffi_type(118));
}
#else
static void _cffi_f_trace_get_err_output(struct trace_err_t *result, struct libtrace_out_t * x0)
{
  { *result = trace_get_err_output(x0); }
}
#endif

static void _cffi_d_trace_help(void)
{
  trace_help();
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_help(PyObject *self, PyObject *noarg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { trace_help(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  (void)noarg; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_trace_help _cffi_d_trace_help
#endif

static int _cffi_d_trace_pause(struct libtrace_t * x0)
{
  return trace_pause(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_pause(PyObject *self, PyObject *arg0)
{
  struct libtrace_t * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_pause(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_trace_pause _cffi_d_trace_pause
#endif

static int _cffi_d_trace_read_packet(struct libtrace_t * x0, struct libtrace_packet_t * x1)
{
  return trace_read_packet(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_read_packet(PyObject *self, PyObject *args)
{
  struct libtrace_t * x0;
  struct libtrace_packet_t * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "trace_read_packet");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(17), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (struct libtrace_packet_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(17), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_read_packet(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_trace_read_packet _cffi_d_trace_read_packet
#endif

static int _cffi_d_trace_start(struct libtrace_t * x0)
{
  return trace_start(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_start(PyObject *self, PyObject *arg0)
{
  struct libtrace_t * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(20), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(20), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_start(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_trace_start _cffi_d_trace_start
#endif

static int _cffi_d_trace_start_output(struct libtrace_out_t * x0)
{
  return trace_start_output(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_start_output(PyObject *self, PyObject *arg0)
{
  struct libtrace_out_t * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_out_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_start_output(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_trace_start_output _cffi_d_trace_start_output
#endif

static int _cffi_d_trace_write_packet(struct libtrace_out_t * x0, struct libtrace_packet_t * x1)
{
  return trace_write_packet(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_trace_write_packet(PyObject *self, PyObject *args)
{
  struct libtrace_out_t * x0;
  struct libtrace_packet_t * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "trace_write_packet");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(8), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct libtrace_out_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(8), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(17), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (struct libtrace_packet_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(17), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = trace_write_packet(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_trace_write_packet _cffi_d_trace_write_packet
#endif

static int _cffi_d_transport_checksum(struct pi * x0, int x1)
{
  return transport_checksum(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_transport_checksum(PyObject *self, PyObject *args)
{
  struct pi * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "transport_checksum");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct pi *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = transport_checksum(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_transport_checksum _cffi_d_transport_checksum
#endif

_CFFI_UNUSED_FN
static void _cffi_checkfld_struct_pi(struct pi *p)
{
  /* only to generate compile-time warnings or errors */
  (void)p;
  (void)((p->o_type) << 1);  /* check that 'struct pi.o_type' is an integer */
  (void)((p->o_kind) << 1);  /* check that 'struct pi.o_kind' is an integer */
  { uint8_t * *tmp = &p->data; (void)tmp; }
  { uint8_t * *tmp = &p->l2p; (void)tmp; }
  (void)((p->l2_rem) << 1);  /* check that 'struct pi.l2_rem' is an integer */
  (void)((p->linktype) << 1);  /* check that 'struct pi.linktype' is an integer */
  (void)((p->ethertype) << 1);  /* check that 'struct pi.ethertype' is an integer */
  (void)((p->vlan_tag) << 1);  /* check that 'struct pi.vlan_tag' is an integer */
  { uint8_t * *tmp = &p->l3p; (void)tmp; }
  (void)((p->l3_rem) << 1);  /* check that 'struct pi.l3_rem' is an integer */
  (void)((p->proto) << 1);  /* check that 'struct pi.proto' is an integer */
  { uint8_t * *tmp = &p->dp; (void)tmp; }
  (void)((p->rem) << 1);  /* check that 'struct pi.rem' is an integer */
}
struct _cffi_align_struct_pi { char x; struct pi y; };

_CFFI_UNUSED_FN
static void _cffi_checkfld_struct_trace_err_t(struct trace_err_t *p)
{
  /* only to generate compile-time warnings or errors */
  (void)p;
  (void)((p->err_num) << 1);  /* check that 'struct trace_err_t.err_num' is an integer */
  { char(*tmp)[255] = &p->problem; (void)tmp; }
}
struct _cffi_align_struct_trace_err_t { char x; struct trace_err_t y; };

static int *_cffi_var_LZO_COMPRESSION(void)
{
  return &(LZO_COMPRESSION);
}

static int *_cffi_var_NO_COMPRESSION(void)
{
  return &(NO_COMPRESSION);
}

static int *_cffi_var_OPT_PROMISC(void)
{
  return &(OPT_PROMISC);
}

static int *_cffi_var_OPT_SNAPLEN(void)
{
  return &(OPT_SNAPLEN);
}

static int *_cffi_var_OUT_COMP_LEVEL(void)
{
  return &(OUT_COMP_LEVEL);
}

static int *_cffi_var_OUT_COMP_TYPE(void)
{
  return &(OUT_COMP_TYPE);
}

static int *_cffi_var_OUT_FILE_FLAGS(void)
{
  return &(OUT_FILE_FLAGS);
}

static int *_cffi_var_TRACE_80211(void)
{
  return &(TRACE_80211);
}

static int *_cffi_var_TRACE_80211_RADIO(void)
{
  return &(TRACE_80211_RADIO);
}

static int *_cffi_var_TRACE_ATM(void)
{
  return &(TRACE_ATM);
}

static int *_cffi_var_TRACE_DUCK(void)
{
  return &(TRACE_DUCK);
}

static int *_cffi_var_TRACE_ETH(void)
{
  return &(TRACE_ETH);
}

static int *_cffi_var_TRACE_LINUX_SLL(void)
{
  return &(TRACE_LINUX_SLL);
}

static int *_cffi_var_TRACE_LLCSNAP(void)
{
  return &(TRACE_LLCSNAP);
}

static int *_cffi_var_TRACE_METADATA(void)
{
  return &(TRACE_METADATA);
}

static int *_cffi_var_TRACE_NONE(void)
{
  return &(TRACE_NONE);
}

static int *_cffi_var_TRACE_PFLOG(void)
{
  return &(TRACE_PFLOG);
}

static int *_cffi_var_TRACE_PPP(void)
{
  return &(TRACE_PPP);
}

static int *_cffi_var_ZIP2_COMPRESSION(void)
{
  return &(ZIP2_COMPRESSION);
}

static int *_cffi_var_ZLIB_COMPRESSION(void)
{
  return &(ZLIB_COMPRESSION);
}

static const struct _cffi_global_s _cffi_globals[] = {
  { "LZO_COMPRESSION", (void *)_cffi_var_LZO_COMPRESSION, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "NO_COMPRESSION", (void *)_cffi_var_NO_COMPRESSION, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "OPT_PROMISC", (void *)_cffi_var_OPT_PROMISC, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "OPT_SNAPLEN", (void *)_cffi_var_OPT_SNAPLEN, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "OUT_COMP_LEVEL", (void *)_cffi_var_OUT_COMP_LEVEL, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "OUT_COMP_TYPE", (void *)_cffi_var_OUT_COMP_TYPE, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "OUT_FILE_FLAGS", (void *)_cffi_var_OUT_FILE_FLAGS, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_80211", (void *)_cffi_var_TRACE_80211, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_80211_RADIO", (void *)_cffi_var_TRACE_80211_RADIO, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_ATM", (void *)_cffi_var_TRACE_ATM, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_DUCK", (void *)_cffi_var_TRACE_DUCK, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_ETH", (void *)_cffi_var_TRACE_ETH, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_LINUX_SLL", (void *)_cffi_var_TRACE_LINUX_SLL, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_LLCSNAP", (void *)_cffi_var_TRACE_LLCSNAP, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_METADATA", (void *)_cffi_var_TRACE_METADATA, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_NONE", (void *)_cffi_var_TRACE_NONE, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_PFLOG", (void *)_cffi_var_TRACE_PFLOG, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "TRACE_PPP", (void *)_cffi_var_TRACE_PPP, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "ZIP2_COMPRESSION", (void *)_cffi_var_ZIP2_COMPRESSION, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "ZLIB_COMPRESSION", (void *)_cffi_var_ZLIB_COMPRESSION, _CFFI_OP(_CFFI_OP_GLOBAL_VAR_F, 12), (void *)0 },
  { "get_bytes", (void *)_cffi_f_get_bytes, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 75), (void *)_cffi_d_get_bytes },
  { "get_capture_len", (void *)_cffi_f_get_capture_len, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 35), (void *)_cffi_d_get_capture_len },
  { "get_direction", (void *)_cffi_f_get_direction, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 35), (void *)_cffi_d_get_direction },
  { "get_erf_time", (void *)_cffi_f_get_erf_time, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 83), (void *)_cffi_d_get_erf_time },
  { "get_long", (void *)_cffi_f_get_long, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 71), (void *)_cffi_d_get_long },
  { "get_pkt_info", (void *)_cffi_f_get_pkt_info, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 42), (void *)_cffi_d_get_pkt_info },
  { "get_short", (void *)_cffi_f_get_short, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 67), (void *)_cffi_d_get_short },
  { "get_time", (void *)_cffi_f_get_time, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 0), (void *)_cffi_d_get_time },
  { "get_trans_payload", (void *)_cffi_f_get_trans_payload, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 46), (void *)_cffi_d_get_trans_payload },
  { "get_transport_info", (void *)_cffi_f_get_transport_info, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 46), (void *)_cffi_d_get_transport_info },
  { "get_ts_sec", (void *)_cffi_f_get_ts_sec, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 35), (void *)_cffi_d_get_ts_sec },
  { "get_wire_len", (void *)_cffi_f_get_wire_len, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 35), (void *)_cffi_d_get_wire_len },
  { "l3_checksum", (void *)_cffi_f_l3_checksum, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 38), (void *)_cffi_d_l3_checksum },
  { "set_bytes", (void *)_cffi_f_set_bytes, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 102), (void *)_cffi_d_set_bytes },
  { "set_config", (void *)_cffi_f_set_config, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 22), (void *)_cffi_d_set_config },
  { "set_config_filter", (void *)_cffi_f_set_config_filter, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 27), (void *)_cffi_d_set_config_filter },
  { "set_config_output", (void *)_cffi_f_set_config_output, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 10), (void *)_cffi_d_set_config_output },
  { "set_long", (void *)_cffi_f_set_long, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 97), (void *)_cffi_d_set_long },
  { "set_short", (void *)_cffi_f_set_short, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 92), (void *)_cffi_d_set_short },
  { "trace_apply_filter", (void *)_cffi_f_trace_apply_filter, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 3), (void *)_cffi_d_trace_apply_filter },
  { "trace_create", (void *)_cffi_f_trace_create, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 58), (void *)_cffi_d_trace_create },
  { "trace_create_filter", (void *)_cffi_f_trace_create_filter, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 50), (void *)_cffi_d_trace_create_filter },
  { "trace_create_output", (void *)_cffi_f_trace_create_output, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 53), (void *)_cffi_d_trace_create_output },
  { "trace_create_packet", (void *)_cffi_f_trace_create_packet, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_N, 56), (void *)_cffi_d_trace_create_packet },
  { "trace_destroy", (void *)_cffi_f_trace_destroy, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 89), (void *)_cffi_d_trace_destroy },
  { "trace_destroy_output", (void *)_cffi_f_trace_destroy_output, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 86), (void *)_cffi_d_trace_destroy_output },
  { "trace_get_accepted_packets", (void *)_cffi_f_trace_get_accepted_packets, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 80), (void *)_cffi_d_trace_get_accepted_packets },
  { "trace_get_dropped_packets", (void *)_cffi_f_trace_get_dropped_packets, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 80), (void *)_cffi_d_trace_get_dropped_packets },
  { "trace_get_err", (void *)_cffi_f_trace_get_err, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 64), (void *)_cffi_d_trace_get_err },
  { "trace_get_err_output", (void *)_cffi_f_trace_get_err_output, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 61), (void *)_cffi_d_trace_get_err_output },
  { "trace_help", (void *)_cffi_f_trace_help, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_N, 107), (void *)_cffi_d_trace_help },
  { "trace_pause", (void *)_cffi_f_trace_pause, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 19), (void *)_cffi_d_trace_pause },
  { "trace_read_packet", (void *)_cffi_f_trace_read_packet, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 31), (void *)_cffi_d_trace_read_packet },
  { "trace_start", (void *)_cffi_f_trace_start, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 19), (void *)_cffi_d_trace_start },
  { "trace_start_output", (void *)_cffi_f_trace_start_output, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 7), (void *)_cffi_d_trace_start_output },
  { "trace_write_packet", (void *)_cffi_f_trace_write_packet, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 15), (void *)_cffi_d_trace_write_packet },
  { "transport_checksum", (void *)_cffi_f_transport_checksum, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 38), (void *)_cffi_d_transport_checksum },
};

static const struct _cffi_field_s _cffi_fields[] = {
  { "o_type", offsetof(struct pi, o_type),
              sizeof(((struct pi *)0)->o_type),
              _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "o_kind", offsetof(struct pi, o_kind),
              sizeof(((struct pi *)0)->o_kind),
              _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "data", offsetof(struct pi, data),
            sizeof(((struct pi *)0)->data),
            _CFFI_OP(_CFFI_OP_NOOP, 68) },
  { "l2p", offsetof(struct pi, l2p),
           sizeof(((struct pi *)0)->l2p),
           _CFFI_OP(_CFFI_OP_NOOP, 68) },
  { "l2_rem", offsetof(struct pi, l2_rem),
              sizeof(((struct pi *)0)->l2_rem),
              _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "linktype", offsetof(struct pi, linktype),
                sizeof(((struct pi *)0)->linktype),
                _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "ethertype", offsetof(struct pi, ethertype),
                 sizeof(((struct pi *)0)->ethertype),
                 _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "vlan_tag", offsetof(struct pi, vlan_tag),
                sizeof(((struct pi *)0)->vlan_tag),
                _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "l3p", offsetof(struct pi, l3p),
           sizeof(((struct pi *)0)->l3p),
           _CFFI_OP(_CFFI_OP_NOOP, 68) },
  { "l3_rem", offsetof(struct pi, l3_rem),
              sizeof(((struct pi *)0)->l3_rem),
              _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "proto", offsetof(struct pi, proto),
             sizeof(((struct pi *)0)->proto),
             _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "dp", offsetof(struct pi, dp),
          sizeof(((struct pi *)0)->dp),
          _CFFI_OP(_CFFI_OP_NOOP, 68) },
  { "rem", offsetof(struct pi, rem),
           sizeof(((struct pi *)0)->rem),
           _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "err_num", offsetof(struct trace_err_t, err_num),
               sizeof(((struct trace_err_t *)0)->err_num),
               _CFFI_OP(_CFFI_OP_NOOP, 12) },
  { "problem", offsetof(struct trace_err_t, problem),
               sizeof(((struct trace_err_t *)0)->problem),
               _CFFI_OP(_CFFI_OP_NOOP, 110) },
};

static const struct _cffi_struct_union_s _cffi_struct_unions[] = {
  { "libtrace_filter_t", 113, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "libtrace_out_t", 114, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "libtrace_packet_t", 115, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "libtrace_t", 116, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "pi", 117, _CFFI_F_CHECK_FIELDS,
    sizeof(struct pi), offsetof(struct _cffi_align_struct_pi, y), 0, 13 },
  { "trace_err_t", 118, _CFFI_F_CHECK_FIELDS,
    sizeof(struct trace_err_t), offsetof(struct _cffi_align_struct_trace_err_t, y), 13, 2 },
};

static const struct _cffi_type_context_s _cffi_type_context = {
  _cffi_types,
  _cffi_globals,
  _cffi_fields,
  _cffi_struct_unions,
  NULL,  /* no enums */
  NULL,  /* no typenames */
  57,  /* num_globals */
  6,  /* num_struct_unions */
  0,  /* num_enums */
  0,  /* num_typenames */
  NULL,  /* no includes */
  122,  /* num_types */
  0,  /* flags */
};

#ifdef PYPY_VERSION
PyMODINIT_FUNC
_cffi_pypyinit_cplt(const void *p[])
{
    p[0] = (const void *)0x2601;
    p[1] = &_cffi_type_context;
}
#  ifdef _MSC_VER
     PyMODINIT_FUNC
#  if PY_MAJOR_VERSION >= 3
     PyInit_cplt(void) { return NULL; }
#  else
     initcplt(void) { }
#  endif
#  endif
#elif PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_cplt(void)
{
  return _cffi_init("cplt", 0x2601, &_cffi_type_context);
}
#else
PyMODINIT_FUNC
initcplt(void)
{
  _cffi_init("cplt", 0x2601, &_cffi_type_context);
}
#endif
