#!/bin/sh

# 1019, Fri 29 Jan 2016 (NZDT)
#
# Make pypy-libtrace distribution tarball
#
# pypy-libtrace: a pypy module to make it easy to use libtrace
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


VER=`./pylt-version.py`
echo "Making tarball for $VER  ..."

OLD=`echo ${PWD##*/}`  # Current directory name
N="python-libtrace-${VER}"  # New directory name

cd ..
if [ $OLD != $N ]
then
  mv $OLD $N
fi

echo "Current directory is .."
echo `pwd`

tar zcf ${N}.tgz \
   $N/LICENSE.txt $N/README.txt $N/INSTALL.txt $N/version-history.txt \
   $N/Makefile $N/*.py \
   $N/lib/ipp/*.py $N/lib/plt/*.py $N/lib/pldns/*.py $N/lib/natkit/*.py \
   $N/doc/*.html $N/doc/*.css $N/doc/*.svg \
   $N/doc/examples/*.py $N/doc/examples/*.pcap\
   $N/test/run_test.py \
   $N/test/pypy-test-cases/*.py $N/test/pypy-test-cases/*.res \

if [ $OLD != $N ]
then
mv $N $OLD
fi
cd $OLD
