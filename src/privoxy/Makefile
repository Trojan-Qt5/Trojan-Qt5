# Written by and Copyright (C) 2001 the SourceForge
# Privoxy team. http://www.privoxy.org/
#
# Based on the Internet Junkbuster originally written
# by and Copyright (C) 1997 Anonymous Coders and 
# Junkbusters Corporation.  http://www.junkbusters.com
#
# This program is free software; you can redistribute it 
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will
# be useful, but WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# The GNU General Public License should be included with
# this file.  If not, you can view it at
# http://www.gnu.org/copyleft/gpl.html
# or write to the Free Software Foundation, Inc., 59
# Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
#############################################################################

GNU_MAKE_CMD = gmake
MAKE_CMD     = make

error:
	@if [ -f GNUmakefile ]; then \
	    echo "***"; \
	    echo "*** You are not using the GNU version of Make - maybe it's called gmake"; \
	    echo "*** or it's in a different PATH? Please read INSTALL." ; \
	    echo "***"; \
	    exit 1; \
	 elif test -n "$(HOST_ARCH)"  && test -z "$(MAKE_VERSION)" ; then \
	    echo "***"; \
	    echo "*** You are not using GNU Make on Solaris, please make sure you do" ; \
	    echo "*** and re-run 'make' "; \
	    echo "***"; \
	    exit 1 ; \
	 elif test -n "$(MACHINE_ARCH)"  && test -z "$(MAKE_VERSION)" ; then \
	    echo "***"; \
	    echo "*** You are not using GNU Make on FreeBSD, please make sure you do" ; \
	    echo "*** and re-run 'make' "; \
	    echo "***"; \
	    exit 1 ; \
	 else \
	    echo "***"; \
	    echo "*** To build this program, you must run"; \
	    echo "*** autoheader && autoconf && ./configure and then run GNU make."; \
	    echo "***"; \
	    echo -n "*** Shall I do this for you now? (y/n) "; \
	    read answer; \
	    if [ "$$answer" = "y" ]; then \
		autoheader && autoconf && ./configure || exit 1; \
	  	if $(GNU_MAKE_CMD) -v |grep GNU >/dev/null 2>/dev/null; then \
		   $(GNU_MAKE_CMD) ;\
		elif $(MAKE_CMD) -v |grep GNU >/dev/null 2>/dev/null; then \
		   $(MAKE_CMD) ;\
		else \
		   echo "Neither 'make' nor 'gmake' are GNU compatible!" ; \
		   echo "Please read INSTALL." ; \
		   exit 1 ; \
		fi ;\
	    fi; \
	 fi

.PHONY: error

#############################################################################

## Local Variables:
## tab-width: 3
## end:
