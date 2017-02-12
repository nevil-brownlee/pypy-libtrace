# 1340, Mon  7 Dec 2015 (NZDT)
#
# build-cipp.py:  makes pypy cipp module
#                 build-cipp.py is the actual module code,
#                    from cipp import ffi, lib # couldn't find lib !!
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

ffi.cdef("char *ipaddr2str(int version, uint8_t *addr);")
ffi.cdef("int str2ipaddr(uint8_t *ba, int ver, char *s);")
ffi.cdef("int caddrcmp(int version, uint8_t *a, uint8_t *b);")
ffi.cdef("long ipp_hash(int ver, uint8_t *sa, int len);")
ffi.cdef("void c_mask_network(uint8_t *a, int sz, uint8_t *b, int len);")
ffi.cdef("int c_bit_set(int version, uint8_t *a, int bn);")
ffi.cdef("int c_fbd(int version, uint8_t *a, uint8_t *b);")
ffi.cdef("void c_complement(int version, uint8_t *a, uint8_t *b);")

ffi.set_source("cipp",  # .so to be created
    """ // passed to the real C compiler
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
	 for (j = nc = 0; ipaddr2str_buf[j] != '\\0'; j += 1)
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
    return memcmp(a, b, version == 4 ? 4 : 16);  /* Returns -128, 0, 128 ! */
   }

long ipp_hash(int ver, uint8_t *sa, int len) {
   int nb, j;  uint32_t *sa32;  long hash;
   hash = (len*63997 << 8) + (uint8_t)ver*41;
   sa32 = (uint32_t *)sa;
   nb = (ver == 4 ? 1 : 4);  /* 32-bit groups */
   for (j = 0; j != nb; j += 1) hash ^= sa32[j]*99991;
   return (long)(hash & 0x7FFFFFFF);  /* Don't return -1 */
   }

uint8_t fb_mask[8] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

void c_mask_network(uint8_t *a, int sz, uint8_t *b, int len) {
   int bytes = len/8, bits = len%8;
   memmove(a, b, bytes);
   a[bytes] = b[bytes] & fb_mask[bits];
   int zlen = sz - bytes - 1;
   if (zlen != 0) memset(&b[bytes+1], 0, zlen);
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
    """,
    libraries=["c"])   # list of libraries to link with

ffi.cdef(
    """
    """)


if __name__ == "__main__":
    ffi.compile()
    
