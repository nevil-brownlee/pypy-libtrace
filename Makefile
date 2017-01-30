# 1447, Thu 14 Jan 16 (NZDT)
#
# Main Makefile for the pypy-libtrace distribution.
#
# pypy-libtrace: a python module to make it easy to use libtrace
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


SHELL=/bin/sh
RM=rm -rf # Don't complain if files don't exist
T_MAKE=	make
BKFILES=build *~ \#* .*~
VERSION=

.PHONY:	all clean distclean install

install:
	@make ipp
	@make plt
	@make pldns
	@make natkit
##	@./verify_install.sh $(VERSION)


ipp:
	cd lib/ipp; pypy$(VERSION) build-cipp.py
	cd lib/ipp; pypy$(VERSION) cipp-setup.py install
	cd lib/ipp; pypy$(VERSION) ipp-setup.py install

plt:
	cd lib/plt; pypy$(VERSION) build-cplt.py
	cd lib/plt; pypy$(VERSION) cplt-setup.py install
	cd lib/plt; pypy$(VERSION) plt-setup.py install

pldns:
	cd lib/pldns; pypy$(VERSION) build-cpldns.py
	cd lib/pldns; pypy$(VERSION) cpldns-setup.py install
	cd lib/pldns; pypy$(VERSION) pldns-setup.py install

natkit:
	cd lib/natkit; pypy$(VERSION) build-cnatkit.py
	cd lib/natkit; pypy$(VERSION) cnatkit-setup.py install
	cd lib/natkit; pypy$(VERSION) natkit-setup.py install

clean:
	cd lib/ipp; $(RM) $(BKFILES)
	cd lib/plt; $(RM) $(BKFILES)
	cd lib/natkit; $(RM) $(BKFILES)
	cd lib/pldns; $(RM) $(BKFILES)

distclean: clean
	$(RM)  $(BKFILES)


install-pypy2:
	@make VERSION=2 install  # Needs ln -s pypy2

install-pypy3: 
	@make VERSION=3 install
