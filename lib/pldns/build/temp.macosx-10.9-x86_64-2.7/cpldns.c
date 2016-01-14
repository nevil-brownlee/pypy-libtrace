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
    

/************************************************************/

static void *_cffi_types[] = {
/*  0 */ _CFFI_OP(_CFFI_OP_FUNCTION, 58), // _Bool()(struct ldns_struct_pkt *)
/*  1 */ _CFFI_OP(_CFFI_OP_POINTER, 63), // struct ldns_struct_pkt *
/*  2 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  3 */ _CFFI_OP(_CFFI_OP_FUNCTION, 59), // char *()(struct ldns_struct_rdf *)
/*  4 */ _CFFI_OP(_CFFI_OP_POINTER, 64), // struct ldns_struct_rdf *
/*  5 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  6 */ _CFFI_OP(_CFFI_OP_FUNCTION, 59), // char *()(struct ldns_struct_rr *)
/*  7 */ _CFFI_OP(_CFFI_OP_POINTER, 65), // struct ldns_struct_rr *
/*  8 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  9 */ _CFFI_OP(_CFFI_OP_FUNCTION, 60), // char const *()(int)
/* 10 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7), // int
/* 11 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 12 */ _CFFI_OP(_CFFI_OP_FUNCTION, 10), // int()(struct ldns_info *)
/* 13 */ _CFFI_OP(_CFFI_OP_POINTER, 62), // struct ldns_info *
/* 14 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 15 */ _CFFI_OP(_CFFI_OP_FUNCTION, 10), // int()(struct ldns_info *, uint8_t *, int)
/* 16 */ _CFFI_OP(_CFFI_OP_NOOP, 13),
/* 17 */ _CFFI_OP(_CFFI_OP_POINTER, 69), // uint8_t *
/* 18 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 19 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 20 */ _CFFI_OP(_CFFI_OP_FUNCTION, 10), // int()(struct ldns_struct_pkt *)
/* 21 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 22 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 23 */ _CFFI_OP(_CFFI_OP_FUNCTION, 10), // int()(struct ldns_struct_rdf *)
/* 24 */ _CFFI_OP(_CFFI_OP_NOOP, 4),
/* 25 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 26 */ _CFFI_OP(_CFFI_OP_FUNCTION, 10), // int()(struct ldns_struct_rr *)
/* 27 */ _CFFI_OP(_CFFI_OP_NOOP, 7),
/* 28 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 29 */ _CFFI_OP(_CFFI_OP_FUNCTION, 10), // int()(struct ldns_struct_rr_list *)
/* 30 */ _CFFI_OP(_CFFI_OP_POINTER, 66), // struct ldns_struct_rr_list *
/* 31 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 32 */ _CFFI_OP(_CFFI_OP_FUNCTION, 43), // size_t()(struct ldns_struct_rdf *)
/* 33 */ _CFFI_OP(_CFFI_OP_NOOP, 4),
/* 34 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 35 */ _CFFI_OP(_CFFI_OP_FUNCTION, 43), // size_t()(struct ldns_struct_rr *)
/* 36 */ _CFFI_OP(_CFFI_OP_NOOP, 7),
/* 37 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 38 */ _CFFI_OP(_CFFI_OP_FUNCTION, 4), // struct ldns_struct_rdf *()(struct ldns_struct_rr *)
/* 39 */ _CFFI_OP(_CFFI_OP_NOOP, 7),
/* 40 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 41 */ _CFFI_OP(_CFFI_OP_FUNCTION, 4), // struct ldns_struct_rdf *()(struct ldns_struct_rr *, size_t)
/* 42 */ _CFFI_OP(_CFFI_OP_NOOP, 7),
/* 43 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 28), // size_t
/* 44 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 45 */ _CFFI_OP(_CFFI_OP_FUNCTION, 7), // struct ldns_struct_rr *()(struct ldns_struct_rr_list *, size_t)
/* 46 */ _CFFI_OP(_CFFI_OP_NOOP, 30),
/* 47 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 28),
/* 48 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 49 */ _CFFI_OP(_CFFI_OP_FUNCTION, 30), // struct ldns_struct_rr_list *()(struct ldns_struct_pkt *)
/* 50 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 51 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 52 */ _CFFI_OP(_CFFI_OP_FUNCTION, 67), // uint16_t()(struct ldns_struct_pkt *)
/* 53 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/* 54 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 55 */ _CFFI_OP(_CFFI_OP_FUNCTION, 68), // uint32_t()(struct ldns_struct_rr *)
/* 56 */ _CFFI_OP(_CFFI_OP_NOOP, 7),
/* 57 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 58 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 1), // _Bool
/* 59 */ _CFFI_OP(_CFFI_OP_POINTER, 61), // char *
/* 60 */ _CFFI_OP(_CFFI_OP_POINTER, 61), // char const *
/* 61 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 2), // char
/* 62 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 0), // struct ldns_info
/* 63 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 1), // struct ldns_struct_pkt
/* 64 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 2), // struct ldns_struct_rdf
/* 65 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 3), // struct ldns_struct_rr
/* 66 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 4), // struct ldns_struct_rr_list
/* 67 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 20), // uint16_t
/* 68 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 22), // uint32_t
/* 69 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 18), // uint8_t
/* 70 */ _CFFI_OP(_CFFI_OP_POINTER, 71), // void *
/* 71 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 0), // void
};

static int _cffi_d_get_ldns_info(struct ldns_info * x0, uint8_t * x1, int x2)
{
  return get_ldns_info(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_get_ldns_info(PyObject *self, PyObject *args)
{
  struct ldns_info * x0;
  uint8_t * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "get_ldns_info");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_info *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(17), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(17), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = get_ldns_info(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_get_ldns_info _cffi_d_get_ldns_info
#endif

static char const * _cffi_d_ldns_get_errorstr_by_id(int x0)
{
  return ldns_get_errorstr_by_id(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_get_errorstr_by_id(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_get_errorstr_by_id(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(60));
}
#else
#  define _cffi_f_ldns_get_errorstr_by_id _cffi_d_ldns_get_errorstr_by_id
#endif

static int _cffi_d_ldns_ok(struct ldns_info * x0)
{
  return ldns_ok(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_ok(PyObject *self, PyObject *arg0)
{
  struct ldns_info * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(13), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_info *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(13), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_ok(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_ldns_ok _cffi_d_ldns_ok
#endif

static struct ldns_struct_rr_list * _cffi_d_ldns_pkt_additional(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_additional(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_additional(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  struct ldns_struct_rr_list * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_additional(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(30));
}
#else
#  define _cffi_f_ldns_pkt_additional _cffi_d_ldns_pkt_additional
#endif

static struct ldns_struct_rr_list * _cffi_d_ldns_pkt_answer(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_answer(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_answer(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  struct ldns_struct_rr_list * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_answer(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(30));
}
#else
#  define _cffi_f_ldns_pkt_answer _cffi_d_ldns_pkt_answer
#endif

static struct ldns_struct_rr_list * _cffi_d_ldns_pkt_authority(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_authority(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_authority(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  struct ldns_struct_rr_list * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_authority(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(30));
}
#else
#  define _cffi_f_ldns_pkt_authority _cffi_d_ldns_pkt_authority
#endif

static int _cffi_d_ldns_pkt_get_opcode(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_get_opcode(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_get_opcode(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_get_opcode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_ldns_pkt_get_opcode _cffi_d_ldns_pkt_get_opcode
#endif

static int _cffi_d_ldns_pkt_get_rcode(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_get_rcode(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_get_rcode(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_get_rcode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_ldns_pkt_get_rcode _cffi_d_ldns_pkt_get_rcode
#endif

static uint16_t _cffi_d_ldns_pkt_id(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_id(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_id(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  uint16_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_id(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint16_t);
}
#else
#  define _cffi_f_ldns_pkt_id _cffi_d_ldns_pkt_id
#endif

static char const * _cffi_d_ldns_pkt_opcode2str(int x0)
{
  return ldns_pkt_opcode2str(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_opcode2str(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_opcode2str(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(60));
}
#else
#  define _cffi_f_ldns_pkt_opcode2str _cffi_d_ldns_pkt_opcode2str
#endif

static uint16_t _cffi_d_ldns_pkt_qdcount(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_qdcount(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_qdcount(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  uint16_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_qdcount(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint16_t);
}
#else
#  define _cffi_f_ldns_pkt_qdcount _cffi_d_ldns_pkt_qdcount
#endif

static _Bool _cffi_d_ldns_pkt_qr(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_qr(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_qr(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  _Bool result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_qr(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, _Bool);
}
#else
#  define _cffi_f_ldns_pkt_qr _cffi_d_ldns_pkt_qr
#endif

static struct ldns_struct_rr_list * _cffi_d_ldns_pkt_question(struct ldns_struct_pkt * x0)
{
  return ldns_pkt_question(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_question(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_pkt * x0;
  Py_ssize_t datasize;
  struct ldns_struct_rr_list * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_pkt *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_question(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(30));
}
#else
#  define _cffi_f_ldns_pkt_question _cffi_d_ldns_pkt_question
#endif

static char const * _cffi_d_ldns_pkt_rcode2str(int x0)
{
  return ldns_pkt_rcode2str(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_pkt_rcode2str(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_pkt_rcode2str(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(60));
}
#else
#  define _cffi_f_ldns_pkt_rcode2str _cffi_d_ldns_pkt_rcode2str
#endif

static char * _cffi_d_ldns_rdf2str(struct ldns_struct_rdf * x0)
{
  return ldns_rdf2str(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rdf2str(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rdf * x0;
  Py_ssize_t datasize;
  char * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rdf *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(4), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rdf2str(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(59));
}
#else
#  define _cffi_f_ldns_rdf2str _cffi_d_ldns_rdf2str
#endif

static int _cffi_d_ldns_rdf_get_type(struct ldns_struct_rdf * x0)
{
  return ldns_rdf_get_type(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rdf_get_type(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rdf * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rdf *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(4), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rdf_get_type(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_ldns_rdf_get_type _cffi_d_ldns_rdf_get_type
#endif

static size_t _cffi_d_ldns_rdf_size(struct ldns_struct_rdf * x0)
{
  return ldns_rdf_size(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rdf_size(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rdf * x0;
  Py_ssize_t datasize;
  size_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rdf *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(4), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rdf_size(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, size_t);
}
#else
#  define _cffi_f_ldns_rdf_size _cffi_d_ldns_rdf_size
#endif

static char * _cffi_d_ldns_rr2str(struct ldns_struct_rr * x0)
{
  return ldns_rr2str(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr2str(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rr * x0;
  Py_ssize_t datasize;
  char * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr2str(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(59));
}
#else
#  define _cffi_f_ldns_rr2str _cffi_d_ldns_rr2str
#endif

static int _cffi_d_ldns_rr_get_type(struct ldns_struct_rr * x0)
{
  return ldns_rr_get_type(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_get_type(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rr * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_get_type(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_ldns_rr_get_type _cffi_d_ldns_rr_get_type
#endif

static struct ldns_struct_rr * _cffi_d_ldns_rr_list_rr(struct ldns_struct_rr_list * x0, size_t x1)
{
  return ldns_rr_list_rr(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_list_rr(PyObject *self, PyObject *args)
{
  struct ldns_struct_rr_list * x0;
  size_t x1;
  Py_ssize_t datasize;
  struct ldns_struct_rr * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "ldns_rr_list_rr");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(30), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr_list *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(30), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, size_t);
  if (x1 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_list_rr(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(7));
}
#else
#  define _cffi_f_ldns_rr_list_rr _cffi_d_ldns_rr_list_rr
#endif

static int _cffi_d_ldns_rr_list_rr_count(struct ldns_struct_rr_list * x0)
{
  return ldns_rr_list_rr_count(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_list_rr_count(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rr_list * x0;
  Py_ssize_t datasize;
  int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(30), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr_list *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(30), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_list_rr_count(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_ldns_rr_list_rr_count _cffi_d_ldns_rr_list_rr_count
#endif

static struct ldns_struct_rdf * _cffi_d_ldns_rr_owner(struct ldns_struct_rr * x0)
{
  return ldns_rr_owner(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_owner(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rr * x0;
  Py_ssize_t datasize;
  struct ldns_struct_rdf * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_owner(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(4));
}
#else
#  define _cffi_f_ldns_rr_owner _cffi_d_ldns_rr_owner
#endif

static size_t _cffi_d_ldns_rr_rd_count(struct ldns_struct_rr * x0)
{
  return ldns_rr_rd_count(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_rd_count(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rr * x0;
  Py_ssize_t datasize;
  size_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_rd_count(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, size_t);
}
#else
#  define _cffi_f_ldns_rr_rd_count _cffi_d_ldns_rr_rd_count
#endif

static struct ldns_struct_rdf * _cffi_d_ldns_rr_rdf(struct ldns_struct_rr * x0, size_t x1)
{
  return ldns_rr_rdf(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_rdf(PyObject *self, PyObject *args)
{
  struct ldns_struct_rr * x0;
  size_t x1;
  Py_ssize_t datasize;
  struct ldns_struct_rdf * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "ldns_rr_rdf");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, size_t);
  if (x1 == (size_t)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_rdf(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(4));
}
#else
#  define _cffi_f_ldns_rr_rdf _cffi_d_ldns_rr_rdf
#endif

static uint32_t _cffi_d_ldns_rr_ttl(struct ldns_struct_rr * x0)
{
  return ldns_rr_ttl(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_ttl(PyObject *self, PyObject *arg0)
{
  struct ldns_struct_rr * x0;
  Py_ssize_t datasize;
  uint32_t result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(7), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (struct ldns_struct_rr *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(7), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_ttl(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, uint32_t);
}
#else
#  define _cffi_f_ldns_rr_ttl _cffi_d_ldns_rr_ttl
#endif

static char const * _cffi_d_ldns_rr_type2str(int x0)
{
  return ldns_rr_type2str(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ldns_rr_type2str(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ldns_rr_type2str(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(60));
}
#else
#  define _cffi_f_ldns_rr_type2str _cffi_d_ldns_rr_type2str
#endif

_CFFI_UNUSED_FN
static void _cffi_checkfld_struct_ldns_info(struct ldns_info *p)
{
  /* only to generate compile-time warnings or errors */
  (void)p;
  { struct ldns_struct_pkt * *tmp = &p->ldpkt; (void)tmp; }
  (void)((p->status) << 1);  /* check that 'struct ldns_info.status' is an integer */
}
struct _cffi_align_struct_ldns_info { char x; struct ldns_info y; };

_CFFI_UNUSED_FN
static void _cffi_checkfld_struct_ldns_struct_rdf(struct ldns_struct_rdf *p)
{
  /* only to generate compile-time warnings or errors */
  (void)p;
  (void)((p->_size) << 1);  /* check that 'struct ldns_struct_rdf._size' is an integer */
  (void)((p->_type) << 1);  /* check that 'struct ldns_struct_rdf._type' is an integer */
  { void * *tmp = &p->_data; (void)tmp; }
}
struct _cffi_align_struct_ldns_struct_rdf { char x; struct ldns_struct_rdf y; };

static const struct _cffi_global_s _cffi_globals[] = {
  { "get_ldns_info", (void *)_cffi_f_get_ldns_info, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 15), (void *)_cffi_d_get_ldns_info },
  { "ldns_get_errorstr_by_id", (void *)_cffi_f_ldns_get_errorstr_by_id, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 9), (void *)_cffi_d_ldns_get_errorstr_by_id },
  { "ldns_ok", (void *)_cffi_f_ldns_ok, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 12), (void *)_cffi_d_ldns_ok },
  { "ldns_pkt_additional", (void *)_cffi_f_ldns_pkt_additional, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 49), (void *)_cffi_d_ldns_pkt_additional },
  { "ldns_pkt_answer", (void *)_cffi_f_ldns_pkt_answer, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 49), (void *)_cffi_d_ldns_pkt_answer },
  { "ldns_pkt_authority", (void *)_cffi_f_ldns_pkt_authority, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 49), (void *)_cffi_d_ldns_pkt_authority },
  { "ldns_pkt_get_opcode", (void *)_cffi_f_ldns_pkt_get_opcode, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 20), (void *)_cffi_d_ldns_pkt_get_opcode },
  { "ldns_pkt_get_rcode", (void *)_cffi_f_ldns_pkt_get_rcode, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 20), (void *)_cffi_d_ldns_pkt_get_rcode },
  { "ldns_pkt_id", (void *)_cffi_f_ldns_pkt_id, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 52), (void *)_cffi_d_ldns_pkt_id },
  { "ldns_pkt_opcode2str", (void *)_cffi_f_ldns_pkt_opcode2str, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 9), (void *)_cffi_d_ldns_pkt_opcode2str },
  { "ldns_pkt_qdcount", (void *)_cffi_f_ldns_pkt_qdcount, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 52), (void *)_cffi_d_ldns_pkt_qdcount },
  { "ldns_pkt_qr", (void *)_cffi_f_ldns_pkt_qr, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 0), (void *)_cffi_d_ldns_pkt_qr },
  { "ldns_pkt_question", (void *)_cffi_f_ldns_pkt_question, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 49), (void *)_cffi_d_ldns_pkt_question },
  { "ldns_pkt_rcode2str", (void *)_cffi_f_ldns_pkt_rcode2str, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 9), (void *)_cffi_d_ldns_pkt_rcode2str },
  { "ldns_rdf2str", (void *)_cffi_f_ldns_rdf2str, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 3), (void *)_cffi_d_ldns_rdf2str },
  { "ldns_rdf_get_type", (void *)_cffi_f_ldns_rdf_get_type, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 23), (void *)_cffi_d_ldns_rdf_get_type },
  { "ldns_rdf_size", (void *)_cffi_f_ldns_rdf_size, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 32), (void *)_cffi_d_ldns_rdf_size },
  { "ldns_rr2str", (void *)_cffi_f_ldns_rr2str, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 6), (void *)_cffi_d_ldns_rr2str },
  { "ldns_rr_get_type", (void *)_cffi_f_ldns_rr_get_type, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 26), (void *)_cffi_d_ldns_rr_get_type },
  { "ldns_rr_list_rr", (void *)_cffi_f_ldns_rr_list_rr, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 45), (void *)_cffi_d_ldns_rr_list_rr },
  { "ldns_rr_list_rr_count", (void *)_cffi_f_ldns_rr_list_rr_count, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 29), (void *)_cffi_d_ldns_rr_list_rr_count },
  { "ldns_rr_owner", (void *)_cffi_f_ldns_rr_owner, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 38), (void *)_cffi_d_ldns_rr_owner },
  { "ldns_rr_rd_count", (void *)_cffi_f_ldns_rr_rd_count, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 35), (void *)_cffi_d_ldns_rr_rd_count },
  { "ldns_rr_rdf", (void *)_cffi_f_ldns_rr_rdf, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 41), (void *)_cffi_d_ldns_rr_rdf },
  { "ldns_rr_ttl", (void *)_cffi_f_ldns_rr_ttl, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 55), (void *)_cffi_d_ldns_rr_ttl },
  { "ldns_rr_type2str", (void *)_cffi_f_ldns_rr_type2str, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 9), (void *)_cffi_d_ldns_rr_type2str },
};

static const struct _cffi_field_s _cffi_fields[] = {
  { "ldpkt", offsetof(struct ldns_info, ldpkt),
             sizeof(((struct ldns_info *)0)->ldpkt),
             _CFFI_OP(_CFFI_OP_NOOP, 1) },
  { "status", offsetof(struct ldns_info, status),
              sizeof(((struct ldns_info *)0)->status),
              _CFFI_OP(_CFFI_OP_NOOP, 10) },
  { "_size", offsetof(struct ldns_struct_rdf, _size),
             sizeof(((struct ldns_struct_rdf *)0)->_size),
             _CFFI_OP(_CFFI_OP_NOOP, 43) },
  { "_type", offsetof(struct ldns_struct_rdf, _type),
             sizeof(((struct ldns_struct_rdf *)0)->_type),
             _CFFI_OP(_CFFI_OP_NOOP, 10) },
  { "_data", offsetof(struct ldns_struct_rdf, _data),
             sizeof(((struct ldns_struct_rdf *)0)->_data),
             _CFFI_OP(_CFFI_OP_NOOP, 70) },
};

static const struct _cffi_struct_union_s _cffi_struct_unions[] = {
  { "ldns_info", 62, _CFFI_F_CHECK_FIELDS,
    sizeof(struct ldns_info), offsetof(struct _cffi_align_struct_ldns_info, y), 0, 2 },
  { "ldns_struct_pkt", 63, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "ldns_struct_rdf", 64, _CFFI_F_CHECK_FIELDS,
    sizeof(struct ldns_struct_rdf), offsetof(struct _cffi_align_struct_ldns_struct_rdf, y), 2, 3 },
  { "ldns_struct_rr", 65, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "ldns_struct_rr_list", 66, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
};

static const struct _cffi_type_context_s _cffi_type_context = {
  _cffi_types,
  _cffi_globals,
  _cffi_fields,
  _cffi_struct_unions,
  NULL,  /* no enums */
  NULL,  /* no typenames */
  26,  /* num_globals */
  5,  /* num_struct_unions */
  0,  /* num_enums */
  0,  /* num_typenames */
  NULL,  /* no includes */
  72,  /* num_types */
  0,  /* flags */
};

#ifdef PYPY_VERSION
PyMODINIT_FUNC
_cffi_pypyinit_cpldns(const void *p[])
{
    p[0] = (const void *)0x2601;
    p[1] = &_cffi_type_context;
}
#  ifdef _MSC_VER
     PyMODINIT_FUNC
#  if PY_MAJOR_VERSION >= 3
     PyInit_cpldns(void) { return NULL; }
#  else
     initcpldns(void) { }
#  endif
#  endif
#elif PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_cpldns(void)
{
  return _cffi_init("cpldns", 0x2601, &_cffi_type_context);
}
#else
PyMODINIT_FUNC
initcpldns(void)
{
  _cffi_init("cpldns", 0x2601, &_cffi_type_context);
}
#endif
