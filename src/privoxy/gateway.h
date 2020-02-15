#ifndef GATEWAY_H_INCLUDED
#define GATEWAY_H_INCLUDED
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/gateway.h,v $
 *
 * Purpose     :  Contains functions to connect to a server, possibly
 *                using a "gateway" (i.e. HTTP proxy and/or SOCKS4
 *                proxy).  Also contains the list of gateway types.
 *
 * Copyright   :  Written by and Copyright (C) 2001-2009 the
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


struct forward_spec;
struct http_request;
struct client_state;

extern jb_socket forwarded_connect(const struct forward_spec * fwd,
                                   struct http_request *http,
                                   struct client_state *csp);

#ifdef FEATURE_CONNECTION_SHARING
extern void initialize_reusable_connections(void);
extern void forget_connection(jb_socket sfd);
extern void remember_connection(const struct reusable_connection *connection);
extern int close_unusable_connections(void);
#endif /* FEATURE_CONNECTION_SHARING */

extern void mark_connection_closed(struct reusable_connection *closed_connection);
#ifdef FEATURE_CONNECTION_KEEP_ALIVE
extern int connection_destination_matches(const struct reusable_connection *connection,
                                          const struct http_request *http,
                                          const struct forward_spec *fwd);
#endif /* def FEATURE_CONNECTION_KEEP_ALIVE */

#ifdef FUZZ
extern jb_err socks_fuzz(struct client_state *csp);
#endif

#endif /* ndef GATEWAY_H_INCLUDED */

/*
  Local Variables:
  tab-width: 3
  end:
*/
