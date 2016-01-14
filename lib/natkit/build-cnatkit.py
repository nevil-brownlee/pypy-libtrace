# 1055, Tue 22 Dec 2015 (NZDT)
#
# build-natkit.py:  makes pypy natkit module
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
ffi.cdef("uint32_t seq_add(uint32_t a, uint32_t b);")
ffi.cdef("uint32_t seq_sub(uint32_t a, uint32_t b);")

ffi.cdef("int seq_lt(uint32_t u1, uint32_t u2);")
ffi.cdef("int seq_ge(uint32_t u1, uint32_t u2);")
ffi.cdef("int seq_gt(uint32_t u1, uint32_t u2);")

ffi.set_source("cnatkit",  # .so to be created
    """ // passed to the real C compiler

uint32_t seq_add(uint32_t a, uint32_t b) {
   return (uint32_t)(a + b);
   }
uint32_t seq_sub(uint32_t a, uint32_t b) {
   return (uint32_t)(a - b);
   }

/* Algorithm for sequence-number comparison from
      http://en.academic.ru/dic.nsf/enwiki/1197489
   (an improvement over that given in RFC 1982) */
int seq_lt(uint32_t u1, uint32_t u2) {
   int32_t i1 = (int32_t)u1, i2 = (int32_t)u2;
   int32_t d = i1-i2;
   return d < 0;
   }
int seq_ge(uint32_t u1, uint32_t u2) {
   int32_t i1 = (int32_t)u1, i2 = (int32_t)u2;
   int32_t d = i1-i2;
   return d >= 0;
   }
int seq_gt(uint32_t u1, uint32_t u2) {
   int32_t i1 = (int32_t)u1, i2 = (int32_t)u2;
   int32_t d = i1-i2;
   return d > 0;
   }


struct fkey4 {
   uint8_t version, proto;
   uint8_t sport[2], dport[2];
   uint8_t saddr[4], daddr[4];
   };
struct fkey6 {
   uint8_t version, proto;
   uint8_t sport[2], dport[2];
   uint8_t saddr[16], daddr[16];
   };
    """,
    libraries=["ldns", "c"])   # list of libraries to link with

ffi.cdef(
    """
// our structs and externs go here
struct fkey4 {
   uint8_t version, proto;
   uint8_t sport[2], dport[2];
   uint8_t saddr[4], daddr[4];
   };
struct fkey6 {
   uint8_t version, proto;
   uint8_t sport[2], dport[2];
   uint8_t saddr[16], daddr[16];
   };
    """)

if __name__ == "__main__":
    ffi.compile()
    
