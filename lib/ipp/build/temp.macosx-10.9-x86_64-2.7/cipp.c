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
#include <arpa/inet.h>
#include <string.h>
char ipaddr2str_buf[60];  // Result buffer for ipp2str()
char *ipaddr2str(int version, uint8_t *addr) {
   if (version == 4) inet_ntop(AF_INET, addr, ipaddr2str_buf, INET_ADDRSTRLEN);
   else {
      inet_ntop(AF_INET6, addr, ipaddr2str_buf, INET6_ADDRSTRLEN);
      /* RFC 5952 says you can't use :: in place of a single :0:
         libc on OSX 10.9 gets this wrong, so we check for it!
	 Nevil and Se-Young, Wed 10 Jun 15 (NZST) */
      char *cp = strstr(ipaddr2str_buf, "::");
      if (cp) {
	 int j, nc;
	 for (j = nc = 0; ipaddr2str_buf[j] != '\0'; j += 1)
	    if (ipaddr2str_buf[j] == ':') nc += 1;
	 if (nc == 7) {
	    memmove(&cp[2], &cp[1], &ipaddr2str_buf[strlen(ipaddr2str_buf)]-cp);
	    cp[1] = '0';
	    }
	 }
      }
   return ipaddr2str_buf;
   }

int str2ipaddr(uint8_t *ba, int ver, char *s) {
   if (ver == 4) return inet_pton(AF_INET, s, ba);
   else return inet_pton(AF_INET6, s, ba);
   }

int caddrcmp(int version, uint8_t *a, uint8_t *b) {
    return memcmp(a, b, version = 4 ? 4 : 16);  /* Returns -128, 0, 128 ! */
   }

int c_fbd(int version, uint8_t *a, uint8_t *b) {
   int nbytes = version == 4 ? 4 : 16;
   int j, r;  uint8_t xor;
   for (j = 0; j != nbytes; ++j)
      if (a[j] != b[j]) break;
   r = j*8; 
   if (j == nbytes)  /* Addresses match */
      return r;
   xor = a[j] ^ b[j];
   while ((xor & 0x80) == 0) {
      r += 1;  xor <<= 1;
      }
   return r;
   }

uint8_t b_mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

int c_bit_set(int version, uint8_t *a, int bn) {
   int last_bit;
   last_bit = version == 4 ? 31 : 127;
   if (bn < 0)  /* Special case: <root> patricia tree node has bit_index -1 */
        /* Always returns true, so <root> stays at top of the tree */
        return 1;
   else if (bn > last_bit)  /* Past last byte of key (string), always false */
      return 0;
   return (a[bn/8] & b_mask[bn%8]) != 0;
   }

void c_complement(int version, uint8_t *a, uint8_t *b) {
   int j, nbytes = version == 4 ? 4 : 16;
   for (j = 0; j != nbytes; ++j) a[j] = ~b[j];
   }
    

/************************************************************/

static void *_cffi_types[] = {
/*  0 */ _CFFI_OP(_CFFI_OP_FUNCTION, 17), // char *()(int, uint8_t *)
/*  1 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7), // int
/*  2 */ _CFFI_OP(_CFFI_OP_POINTER, 25), // uint8_t *
/*  3 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  4 */ _CFFI_OP(_CFFI_OP_FUNCTION, 1), // int()(int, uint8_t *, int)
/*  5 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/*  6 */ _CFFI_OP(_CFFI_OP_NOOP, 2),
/*  7 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/*  8 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  9 */ _CFFI_OP(_CFFI_OP_FUNCTION, 1), // int()(int, uint8_t *, uint8_t *)
/* 10 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 11 */ _CFFI_OP(_CFFI_OP_NOOP, 2),
/* 12 */ _CFFI_OP(_CFFI_OP_NOOP, 2),
/* 13 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 14 */ _CFFI_OP(_CFFI_OP_FUNCTION, 1), // int()(uint8_t *, int, char *)
/* 15 */ _CFFI_OP(_CFFI_OP_NOOP, 2),
/* 16 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 17 */ _CFFI_OP(_CFFI_OP_POINTER, 24), // char *
/* 18 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 19 */ _CFFI_OP(_CFFI_OP_FUNCTION, 26), // void()(int, uint8_t *, uint8_t *)
/* 20 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 21 */ _CFFI_OP(_CFFI_OP_NOOP, 2),
/* 22 */ _CFFI_OP(_CFFI_OP_NOOP, 2),
/* 23 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 24 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 2), // char
/* 25 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 18), // uint8_t
/* 26 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 0), // void
};

static int _cffi_d_c_bit_set(int x0, uint8_t * x1, int x2)
{
  return c_bit_set(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_c_bit_set(PyObject *self, PyObject *args)
{
  int x0;
  uint8_t * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "c_bit_set");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = c_bit_set(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_c_bit_set _cffi_d_c_bit_set
#endif

static void _cffi_d_c_complement(int x0, uint8_t * x1, uint8_t * x2)
{
  c_complement(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_c_complement(PyObject *self, PyObject *args)
{
  int x0;
  uint8_t * x1;
  uint8_t * x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "c_complement");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { c_complement(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_c_complement _cffi_d_c_complement
#endif

static int _cffi_d_c_fbd(int x0, uint8_t * x1, uint8_t * x2)
{
  return c_fbd(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_c_fbd(PyObject *self, PyObject *args)
{
  int x0;
  uint8_t * x1;
  uint8_t * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "c_fbd");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = c_fbd(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_c_fbd _cffi_d_c_fbd
#endif

static int _cffi_d_caddrcmp(int x0, uint8_t * x1, uint8_t * x2)
{
  return caddrcmp(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_caddrcmp(PyObject *self, PyObject *args)
{
  int x0;
  uint8_t * x1;
  uint8_t * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "caddrcmp");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = caddrcmp(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_caddrcmp _cffi_d_caddrcmp
#endif

static char * _cffi_d_ipaddr2str(int x0, uint8_t * x1)
{
  return ipaddr2str(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_ipaddr2str(PyObject *self, PyObject *args)
{
  int x0;
  uint8_t * x1;
  Py_ssize_t datasize;
  char * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "ipaddr2str");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  x0 = _cffi_to_c_int(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = ipaddr2str(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(17));
}
#else
#  define _cffi_f_ipaddr2str _cffi_d_ipaddr2str
#endif

static int _cffi_d_str2ipaddr(uint8_t * x0, int x1, char * x2)
{
  return str2ipaddr(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_str2ipaddr(PyObject *self, PyObject *args)
{
  uint8_t * x0;
  int x1;
  char * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "str2ipaddr");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (uint8_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(2), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(17), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(17), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = str2ipaddr(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_str2ipaddr _cffi_d_str2ipaddr
#endif

static const struct _cffi_global_s _cffi_globals[] = {
  { "c_bit_set", (void *)_cffi_f_c_bit_set, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 4), (void *)_cffi_d_c_bit_set },
  { "c_complement", (void *)_cffi_f_c_complement, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 19), (void *)_cffi_d_c_complement },
  { "c_fbd", (void *)_cffi_f_c_fbd, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 9), (void *)_cffi_d_c_fbd },
  { "caddrcmp", (void *)_cffi_f_caddrcmp, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 9), (void *)_cffi_d_caddrcmp },
  { "ipaddr2str", (void *)_cffi_f_ipaddr2str, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 0), (void *)_cffi_d_ipaddr2str },
  { "str2ipaddr", (void *)_cffi_f_str2ipaddr, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 14), (void *)_cffi_d_str2ipaddr },
};

static const struct _cffi_type_context_s _cffi_type_context = {
  _cffi_types,
  _cffi_globals,
  NULL,  /* no fields */
  NULL,  /* no struct_unions */
  NULL,  /* no enums */
  NULL,  /* no typenames */
  6,  /* num_globals */
  0,  /* num_struct_unions */
  0,  /* num_enums */
  0,  /* num_typenames */
  NULL,  /* no includes */
  27,  /* num_types */
  0,  /* flags */
};

#ifdef PYPY_VERSION
PyMODINIT_FUNC
_cffi_pypyinit_cipp(const void *p[])
{
    p[0] = (const void *)0x2601;
    p[1] = &_cffi_type_context;
}
#  ifdef _MSC_VER
     PyMODINIT_FUNC
#  if PY_MAJOR_VERSION >= 3
     PyInit_cipp(void) { return NULL; }
#  else
     initcipp(void) { }
#  endif
#  endif
#elif PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_cipp(void)
{
  return _cffi_init("cipp", 0x2601, &_cffi_type_context);
}
#else
PyMODINIT_FUNC
initcipp(void)
{
  _cffi_init("cipp", 0x2601, &_cffi_type_context);
}
#endif
