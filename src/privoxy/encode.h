#ifndef ENCODE_H_INCLUDED
#define ENCODE_H_INCLUDED
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/encode.h,v $
 *
 * Purpose     :  Functions to encode and decode URLs, and also to
 *                encode cookies and HTML text.
 *
 * Copyright   :  Written by and Copyright (C) 2001 members of the
 *                Privoxy team. http://www.privoxy.org/
 *
 *                Based on the Internet Junkbuster originally written
 *                by and Copyright (C) 1997 Anonymous Coders and
 *                Junkbusters Corporation.  http://www.junkbusters.com
 *
 *                This program is free software; you can redistribute it
 *                and/or modify it under the terms of the GNU General
 *                Public License as published by the Free Software
 *                Foundation; either version 2 of the License, or (at
 *                your option) any later version.
 *
 *                This program is distributed in the hope that it will
 *                be useful, but WITHOUT ANY WARRANTY; without even the
 *                implied warranty of MERCHANTABILITY or FITNESS FOR A
 *                PARTICULAR PURPOSE.  See the GNU General Public
 *                License for more details.
 *
 *                The GNU General Public License should be included with
 *                this file.  If not, you can view it at
 *                http://www.gnu.org/copyleft/gpl.html
 *                or write to the Free Software Foundation, Inc., 59
 *                Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *********************************************************************/


extern char * html_encode(const char *s);
extern char * url_encode(const char *s);
extern char * url_decode(const char *str);
extern int    xtoi(const char *s);
extern char * html_encode_and_free_original(char *s);
extern char * percent_encode_url(const char *s);

#endif /* ndef ENCODE_H_INCLUDED */

/*
  Local Variables:
  tab-width: 3
  end:
*/
