#ifndef CGISIMPLE_H_INCLUDED
#define CGISIMPLE_H_INCLUDED
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/cgisimple.h,v $
 *
 * Purpose     :  Declares functions to intercept request, generate
 *                html or gif answers, and to compose HTTP resonses.
 *
 *                Functions declared include:
 *
 *
 * Copyright   :  Written by and Copyright (C) 2001-2017 members of the
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
 **********************************************************************/


#include "project.h"

/*
 * CGI functions
 */
extern jb_err cgi_default      (struct client_state *csp,
                                struct http_response *rsp,
                                const struct map *parameters);
extern jb_err cgi_error_404    (struct client_state *csp,
                                struct http_response *rsp,
                                const struct map *parameters);
extern jb_err cgi_robots_txt   (struct client_state *csp,
                                struct http_response *rsp,
                                const struct map *parameters);
extern jb_err cgi_send_banner  (struct client_state *csp,
                                struct http_response *rsp,
                                const struct map *parameters);
extern jb_err cgi_show_status  (struct client_state *csp,
                                struct http_response *rsp,
                                const struct map *parameters);
extern jb_err cgi_show_url_info(struct client_state *csp,
                                struct http_response *rsp,
                                const struct map *parameters);
extern jb_err cgi_show_request (struct client_state *csp,
                                struct http_response *rsp,
                                const struct map *parameters);
#ifdef FEATURE_CLIENT_TAGS
extern jb_err cgi_show_client_tags(struct client_state *csp,
                                   struct http_response *rsp,
                                   const struct map *parameters);
extern jb_err cgi_toggle_client_tag(struct client_state *csp,
                                    struct http_response *rsp,
                                    const struct map *parameters);
#endif
extern jb_err cgi_transparent_image (struct client_state *csp,
                                     struct http_response *rsp,
                                     const struct map *parameters);
extern jb_err cgi_send_error_favicon (struct client_state *csp,
                                      struct http_response *rsp,
                                      const struct map *parameters);
extern jb_err cgi_send_default_favicon (struct client_state *csp,
                                        struct http_response *rsp,
                                        const struct map *parameters);
extern jb_err cgi_send_stylesheet(struct client_state *csp,
                                  struct http_response *rsp,
                                  const struct map *parameters);
extern jb_err cgi_send_url_info_osd(struct client_state *csp,
                                    struct http_response *rsp,
                                    const struct map *parameters);
extern jb_err cgi_send_user_manual(struct client_state *csp,
                                   struct http_response *rsp,
                                   const struct map *parameters);


#ifdef FEATURE_GRACEFUL_TERMINATION
extern jb_err cgi_die (struct client_state *csp,
                       struct http_response *rsp,
                       const struct map *parameters);
#endif

#endif /* ndef CGISIMPLE_H_INCLUDED */

/*
  Local Variables:
  tab-width: 3
  end:
*/
