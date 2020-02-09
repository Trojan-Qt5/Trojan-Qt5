#ifndef CLIENT_TAGS_H_INCLUDED
#define CLIENT_TAGS_H_INCLUDED
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/client-tags.h,v $
 *
 * Purpose     :  Declares functions for client-specific tags.
 *
 * Copyright   :  Copyright (C) 2016 Fabian Keil <fk@fabiankeil.de>
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

extern int client_tag_match(const struct pattern_spec *pattern,
                            const struct list *tags);
extern void get_tag_list_for_client(struct list *tag_list,
                                    const char *client_address);
extern time_t get_next_tag_timeout_for_client(const char *client_address);
extern jb_err disable_client_specific_tag(struct client_state *csp,
                                          const char *tag_name);
extern jb_err enable_client_specific_tag(struct client_state *csp,
                                         const char *tag_name,
                                         const time_t time_to_live);
extern int client_has_requested_tag(const char *client_address,
                                    const char *tag);
extern void set_client_address(struct client_state *csp,
                               const struct list *headers);

#define CLIENT_TAG_LENGTH_MAX 50
#endif
