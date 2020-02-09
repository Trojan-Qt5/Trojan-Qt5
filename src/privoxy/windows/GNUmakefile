##########################################################################
#
# Written by and Copyright (C) 2002-2018 the Privoxy team.
# http://www.privoxy.org/
#
# This script originally written by and Copyright (C) 2002
# Jonathan Foster
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
##########################################################################


##########################################################################
# External programs
##########################################################################
SED        = sed
PERL       = perl
AWK        = awk
WGET       = wget
STRIP      = strip
AUTOCONF   = autoconf
AUTOHEADER = autoheader

# Program to copy a directory from a web site to the local disk.
WEBMIRROR = $(WGET) -nv -nH -nd -N -r -np

# Program to do LF->CRLF
#
# The sed version should be the most portable, but it doesn't work for me,
# the other two do.  FIXME.
#   - Jon
#
DOSFILTER = $(SED) -e $$'s,$$,\r,'
#DOSFILTER = $(AWK) -v ORS='\r\n' '{print $0;}'
#DOSFILTER = $(PERL) -p -e 's/\n/\r\n/'

# Path to NSIS
# MAKENSIS = ./nsis/nsis-2.30.exe
MAKENSIS = ./nsis-3.03/makensis.exe


##########################################################################
# More configuration
##########################################################################

# Directory containing the source code
SOURCE   = ..

# Directory to build an image of the installed file system in.
DEST     = ./build

# The name of the EXE installer (must match the name specified in
# the .NSI script)
TARGET=privoxy_setup.exe

# The NSIS script that generates the installer.
NSIFILE=privoxy_winthreads.nsi


##########################################################################
# Rules
##########################################################################

winrelease: $(TARGET)

winbuild: $(DEST)/privoxy.exe templates inifiles doc webdoc

clean:
	rm -rf $(DEST) $(TARGET)

clobber: clean
	rm -f $(TARGET) $(SOURCE)/privoxy.exe

.PHONY: winrelease winbuild clean


$(TARGET): $(NSIFILE) winbuild
	$(MAKENSIS) $<

$(DEST):
	mkdir $@

$(DEST)/templates:
	mkdir $@

$(DEST)/doc:
	mkdir $@

$(DEST)/%.txt: $(SOURCE)/% $(DEST)
	$(DOSFILTER) < $< > $@ 


inifiles: $(DEST)/config.txt $(DEST)/match-all.action $(DEST)/default.action $(DEST)/user.action \
          $(DEST)/default.filter $(DEST)/trust.txt $(DEST)/user.filter


$(DEST)/config.txt: $(SOURCE)/config $(DEST)
	$(SED) -e 's!trustfile trust!trustfile trust.txt!' \
	       -e 's!logfile logfile!logfile privoxy.log!' \
	       -e 's!#user-manual http://www.privoxy.org/user-manual/!user-manual ./doc/user-manual/!' \
	       -e 's!#Win32-only: !!' \
	       < $< | \
	       $(DOSFILTER) > $@

$(DEST)/match-all.action: $(SOURCE)/match-all.action $(DEST)
	$(DOSFILTER) < $< > $@

$(DEST)/default.action: $(SOURCE)/default.action $(DEST)
	$(DOSFILTER) < $< > $@ 

$(DEST)/user.action: $(SOURCE)/user.action $(DEST)
	$(DOSFILTER) < $< > $@ 

$(DEST)/default.filter: $(SOURCE)/default.filter $(DEST)
	$(DOSFILTER) < $< > $@ 

$(DEST)/user.filter: $(SOURCE)/user.filter $(DEST)
	$(DOSFILTER) < $< > $@ 

$(DEST)/privoxy.exe: $(SOURCE)/privoxy.exe $(DEST)
	$(STRIP) -o $@ $<

$(SOURCE)/configure: $(SOURCE)/configure.in
	cd $(SOURCE) && $(AUTOCONF)

$(SOURCE)/config.h.in: $(SOURCE)/configure $(SOURCE)/acconfig.h
	cd $(SOURCE) && $(AUTOHEADER)

$(SOURCE)/config.h: $(SOURCE)/configure $(SOURCE)/config.h.in
	./MYconfigure

$(SOURCE)/privoxy.exe: $(SOURCE)/configure $(SOURCE)/config.h
	cd $(SOURCE) && $(MAKE) clean
	cd $(SOURCE) && $(MAKE)


templates: $(patsubst $(SOURCE)/%,$(DEST)/%,$(wildcard $(SOURCE)/templates/*))

$(DEST)/templates/%: $(SOURCE)/templates/% $(DEST)/templates
	if test -f $< ; then $(DOSFILTER) < $< > $@ ; fi


doc: $(DEST)/AUTHORS.txt $(DEST)/README.txt $(DEST)/LICENSE.txt

webdoc: $(DEST)/doc/developer-manual $(DEST)/doc/faq $(DEST)/doc/images $(DEST)/doc/user-manual
	cp $(SOURCE)/doc/webserver/p_doc.css $(DEST)/doc
	cp $(SOURCE)/doc/webserver/p_doc.css $(DEST)/doc/user-manual/


$(DEST)/doc/gpl.html: $(SOURCE)/doc/gpl.html $(DEST)/doc
	$(DOSFILTER) < $< > $@ 

$(DEST)/doc/%.txt : $(SOURCE)/doc/text/%.txt $(DEST)/doc
	$(DOSFILTER) < $< > $@ 

$(DEST)/doc/developer-manual: $(DEST)/doc $(SOURCE)/doc/webserver/developer-manual/index.html
	test -d $(DEST)/doc/developer-manual || mkdir $(DEST)/doc/developer-manual
	cp    $(SOURCE)/doc/webserver/developer-manual/*.html $(DEST)/doc/developer-manual/

$(DEST)/doc/user-manual: $(DEST)/doc $(SOURCE)/doc/webserver/user-manual/index.html
	test -d $(DEST)/doc/user-manual      || mkdir $(DEST)/doc/user-manual
	cp    $(SOURCE)/doc/webserver/user-manual/*.html $(DEST)/doc/user-manual/

$(DEST)/doc/faq: $(DEST)/doc $(SOURCE)/doc/webserver/faq/index.html
	test -d $(DEST)/doc/faq              || mkdir $(DEST)/doc/faq
	cp    $(SOURCE)/doc/webserver/faq/*.html $(DEST)/doc/faq/

$(DEST)/doc/images: $(DEST)/doc
	cp -r $(SOURCE)/doc/webserver/images/ $(DEST)/doc

##########################################################################
# That's all, folks!
##########################################################################

