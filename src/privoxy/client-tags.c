/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/client-tags.c,v $
 *
 * Purpose     :  Functions related to client-specific tags.
 *
 * Copyright   :  Copyright (C) 2016-2017 Fabian Keil <fk@fabiankeil.de>
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

#include "config.h"

#ifdef FEATURE_CLIENT_TAGS

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "project.h"
#include "list.h"
#include "jcc.h"
#include "miscutil.h"
#include "errlog.h"
#include "parsers.h"

struct client_specific_tag
{
   char *name;

   time_t end_of_life;

   struct client_specific_tag *next;
   struct client_specific_tag *prev;
};

/**
 * This struct represents tags that have been requested by clients
 */
struct requested_tags
{
   char *client; /**< The IP address of the client that requested the tag */

   /**< List of tags the client requested .... */
   struct client_specific_tag *tags;

   struct requested_tags *next;
   struct requested_tags *prev;
};

struct requested_tags *requested_tags;
static void remove_tag_for_client(const char *client_address, const char *tag);

/*********************************************************************
 *
 * Function    :  validate_tag_list
 *
 * Description :  Validates the given tag list
 *
 * Parameters  :
 *          1  :  enabled_tags = The tags to validate
 *
 * Returns     :  void
 *
 *********************************************************************/
static void validate_tag_list(struct client_specific_tag *enabled_tags)
{
   while (enabled_tags != NULL)
   {
      if (enabled_tags->name == NULL)
      {
         assert(enabled_tags->name != NULL);
         log_error(LOG_LEVEL_FATAL, "validate_tag_list(): Tag without name detected");
      }
      if (enabled_tags->next != NULL)
      {
         if (enabled_tags->next->prev != enabled_tags)
         {
            assert(enabled_tags->next->prev == enabled_tags);
            log_error(LOG_LEVEL_FATAL, "validate_tag_list(): Invalid backlink detected");
         }
      }
      enabled_tags = enabled_tags->next;
   }
}

/*********************************************************************
 *
 * Function    :  validate_requested_tags
 *
 * Description :  Validates the requested_tags list
 *
 * Parameters  : N/A
 *
 * Returns     :  void
 *
 *********************************************************************/
static jb_err validate_requested_tags()
{
   struct requested_tags *requested_tag;

   for (requested_tag = requested_tags; requested_tag != NULL;
        requested_tag = requested_tag->next)
   {
      if (requested_tag->client == NULL)
      {
         assert(requested_tag->client != NULL);
         log_error(LOG_LEVEL_FATAL, "validate_tag_list(): Client not registered");
      }
      validate_tag_list(requested_tag->tags);
      if (requested_tag->next != NULL)
      {
         if (requested_tag->next->prev != requested_tag)
         {
            assert(requested_tag->next->prev == requested_tag);
            log_error(LOG_LEVEL_FATAL, "validate_requested_tags(): Invalid backlink detected");
         }
      }
   }

   return TRUE;
}


/*********************************************************************
 *
 * Function    :  get_client_specific_tag
 *
 * Description :  Returns the data for a client-specific-tag specified
 *                by name.
 *
 * Parameters  :
 *          1  :  tag_list = The tag list to check
 *          2  :  name =     The tag name to look up
 *
 * Returns     :  Pointer to tag structure or NULL on error.
 *
 *********************************************************************/
static struct client_tag_spec *get_client_specific_tag(
   struct client_tag_spec *tag_list, const char *name)
{
   struct client_tag_spec *tag;

   for (tag = tag_list; tag != NULL; tag = tag->next)
   {
      if (tag->name != NULL && !strcmp(tag->name, name))
      {
         return tag;
      }
   }

   log_error(LOG_LEVEL_ERROR, "No such tag: '%s'", name);

   return NULL;

}


/*********************************************************************
 *
 * Function    :  get_tags_for_client
 *
 * Description :  Returns the list of tags the client opted-in.
 *
 * Parameters  :
 *          1  :  client_address = Address of the client
 *
 * Returns     :  Pointer to tag structure or NULL on error.
 *
 *********************************************************************/
static struct client_specific_tag *get_tags_for_client(const char *client_address)
{
   struct requested_tags *requested_tag;

   for (requested_tag = requested_tags; requested_tag != NULL;
        requested_tag = requested_tag->next)
   {
      if (!strcmp(requested_tag->client, client_address))
      {
         return requested_tag->tags;
      }
   }

   return NULL;
}


/*********************************************************************
 *
 * Function    :  get_tag_list_for_client
 *
 * Description :  Provides a list of tag names the client opted-in.
 *                Other tag attributes are not part of the list.
 *
 * Parameters  :
 *          1  :  tag_list = The list to fill in.
 *          2  :  client_address = Address of the client
 *
 * Returns     :  Pointer to tag list.
 *
 *********************************************************************/
void get_tag_list_for_client(struct list *tag_list,
                             const char *client_address)
{
   struct client_specific_tag *enabled_tags;
   const time_t now = time(NULL);

   privoxy_mutex_lock(&client_tags_mutex);

   enabled_tags = get_tags_for_client(client_address);
   while (enabled_tags != NULL)
   {
      if (enabled_tags->end_of_life && (enabled_tags->end_of_life < now))
      {
         struct client_specific_tag *next_tag = enabled_tags->next;
         log_error(LOG_LEVEL_INFO,
            "Tag '%s' for client %s expired %u seconds ago. Deleting it.",
            enabled_tags->name, client_address,
            (now - enabled_tags->end_of_life));
         remove_tag_for_client(client_address, enabled_tags->name);
         enabled_tags = next_tag;
         continue;
      }
      else
      {
         enlist(tag_list, enabled_tags->name);
      }
      enabled_tags = enabled_tags->next;
   }

   privoxy_mutex_unlock(&client_tags_mutex);
}


/*********************************************************************
 *
 * Function    :  get_next_tag_timeout_for_client
 *
 * Description :  Figures out when the next temporarily enabled tag
 *                for the client will have timed out.
 *
 * Parameters  :
 *          1  :  client_address = Address of the client
 *
 * Returns     :  Lowest timeout in seconds
 *
 *********************************************************************/
time_t get_next_tag_timeout_for_client(const char *client_address)
{
   struct client_specific_tag *enabled_tags;
   time_t next_timeout = 0;
   const time_t now = time(NULL);

   privoxy_mutex_lock(&client_tags_mutex);

   enabled_tags = get_tags_for_client(client_address);
   while (enabled_tags != NULL)
   {
      log_error(LOG_LEVEL_CGI, "Evaluating tag '%s' for client %s. End of life %d",
         enabled_tags->name, client_address, enabled_tags->end_of_life);
      if (enabled_tags->end_of_life)
      {
          time_t time_left = enabled_tags->end_of_life - now;
          /* Add a second to make sure the tag will have expired */
          time_left++;
          log_error(LOG_LEVEL_CGI, "%d > %d?", next_timeout, time_left);
          if (next_timeout == 0 || next_timeout > time_left)
          {
             next_timeout = time_left;
          }
       }
       enabled_tags = enabled_tags->next;
   }

   privoxy_mutex_unlock(&client_tags_mutex);

   log_error(LOG_LEVEL_CGI, "Next timeout in %d seconds", next_timeout);

   return next_timeout;

}


/*********************************************************************
 *
 * Function    :  create_client_specific_tag
 *
 * Description :  Allocates memory for a client specific tag
 *                and populates it.
 *
 * Parameters  :
 *          1  :  name = The name of the tag to create.
 *          2  :  time_to_live = 0, or the number of seconds
 *                               the tag remains activated.
 *
 * Returns     :  Pointer to populated tag
 *
 *********************************************************************/
static struct client_specific_tag *create_client_specific_tag(const char *name,
   const time_t time_to_live)
{
   struct client_specific_tag *tag;

   tag = zalloc_or_die(sizeof(struct client_specific_tag));
   tag->name = strdup_or_die(name);
   tag->end_of_life = time_to_live ? (time(NULL) + time_to_live) : 0;

   return tag;

}

/*********************************************************************
 *
 * Function    :  add_tag_for_client
 *
 * Description :  Adds the tag for the client.
 *
 * Parameters  :
 *          1  :  client_address = Address of the client
 *          2  :  tag = The tag to add.
 *          3  :  time_to_live = 0, or the number of seconds
 *                               the tag remains activated.
 *
 * Returns     :  void
 *
 *********************************************************************/
static void add_tag_for_client(const char *client_address,
   const char *tag, const time_t time_to_live)
{
   struct requested_tags *clients_with_tags;
   struct client_specific_tag *enabled_tags;

   validate_requested_tags();

   if (requested_tags == NULL)
   {
      /* XXX: Code duplication. */
      requested_tags = zalloc_or_die(sizeof(struct requested_tags));
      requested_tags->client = strdup_or_die(client_address);
      requested_tags->tags = create_client_specific_tag(tag, time_to_live);

      validate_requested_tags();
      return;
   }
   else
   {
      clients_with_tags = requested_tags;
      while (clients_with_tags->next != NULL)
      {
         if (!strcmp(clients_with_tags->client, client_address))
         {
            break;
         }
         clients_with_tags = clients_with_tags->next;
      }
      if (strcmp(clients_with_tags->client, client_address))
      {
         /* Client does not have tags yet, add new structure */
         clients_with_tags->next = zalloc_or_die(sizeof(struct requested_tags));
         clients_with_tags->next->prev = clients_with_tags;
         clients_with_tags = clients_with_tags->next;
         clients_with_tags->client = strdup_or_die(client_address);
         clients_with_tags->tags = create_client_specific_tag(tag, time_to_live);

         validate_requested_tags();

         return;
      }
   }

   enabled_tags = clients_with_tags->tags;
   while (enabled_tags != NULL)
   {
      if (enabled_tags->next == NULL)
      {
         enabled_tags->next = create_client_specific_tag(tag, time_to_live);
         enabled_tags->next->prev = enabled_tags;
         break;
      }
      enabled_tags = enabled_tags->next;
   }

   validate_requested_tags();
}


/*********************************************************************
 *
 * Function    :  remove_tag_for_client
 *
 * Description :  Removes the tag for the client.
 *
 * Parameters  :
 *          1  :  client_address = Address of the client
 *          2  :  tag = The tag to remove.
 *
 * Returns     :  void
 *
 *********************************************************************/
static void remove_tag_for_client(const char *client_address, const char *tag)
{
   struct requested_tags *clients_with_tags;
   struct client_specific_tag *enabled_tags;

   validate_requested_tags();

   clients_with_tags = requested_tags;
   while (clients_with_tags != NULL && clients_with_tags->client != NULL)
   {
      if (!strcmp(clients_with_tags->client, client_address))
      {
         break;
      }
      clients_with_tags = clients_with_tags->next;
   }

   assert(clients_with_tags != NULL);
   if (clients_with_tags == NULL)
   {
      log_error(LOG_LEVEL_ERROR,
         "Tried to remove tag %s for tag-less client %s",
         tag, client_address);
   }
   enabled_tags = clients_with_tags->tags;
   while (enabled_tags != NULL)
   {
      if (!strcmp(enabled_tags->name, tag))
      {
         if (enabled_tags->next != NULL)
         {
            enabled_tags->next->prev = enabled_tags->prev;
            if (enabled_tags == clients_with_tags->tags)
            {
               /* Tag is first in line */
               clients_with_tags->tags = enabled_tags->next;
            }
         }
         if (enabled_tags->prev != NULL)
         {
            /* Tag has preceding tag */
            enabled_tags->prev->next = enabled_tags->next;
         }
         if (enabled_tags->prev == NULL && enabled_tags->next == NULL)
         {
            /* Tag is the only one */
            if (clients_with_tags->next != NULL)
            {
               /* Client has following client */
               clients_with_tags->next->prev = clients_with_tags->prev;
            }
            if (clients_with_tags->prev != NULL)
            {
               /* Client has preceding client */
               clients_with_tags->prev->next = clients_with_tags->next;
            }
            if (clients_with_tags == requested_tags)
            {
               /*
                * We're in the process of removing the last tag,
                * mark the global list as empty.
                */
               requested_tags = NULL;
            }
            freez(clients_with_tags->client);
            freez(clients_with_tags);
         }
         freez(enabled_tags->name);
         freez(enabled_tags);
         break;
      }

      enabled_tags = enabled_tags->next;
   }

   validate_requested_tags();

}


/*********************************************************************
 *
 * Function    :  client_has_requested_tag
 *
 * Description :  Checks whether or not the given client requested
 *                the tag.
 *
 * Parameters  :
 *          1  :  client_address = Address of the client
 *          2  :  tag = Tag to check.
 *
 * Returns     :  TRUE or FALSE.
 *
 *********************************************************************/
int client_has_requested_tag(const char *client_address, const char *tag)
{
   struct client_specific_tag *enabled_tags;

   enabled_tags = get_tags_for_client(client_address);

   while (enabled_tags != NULL)
   {
      if (!strcmp(enabled_tags->name, tag))
      {
         return TRUE;
      }
      enabled_tags = enabled_tags->next;
   }

   return FALSE;

}

/*********************************************************************
 *
 * Function    :  enable_client_specific_tag
 *
 * Description :  Enables a client-specific-tag for the client
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  tag_name = The name of the tag to enable
 *          3  :  time_to_live = If not 0, the number of seconds the
 *                               tag should stay enabled.
 *
 * Returns     :  JB_ERR_OK on success, JB_ERR_MEMORY or JB_ERR_PARSE.
 *
 *********************************************************************/
jb_err enable_client_specific_tag(struct client_state *csp,
   const char *tag_name, const time_t time_to_live)
{
   struct client_tag_spec *tag;

   privoxy_mutex_lock(&client_tags_mutex);

   tag = get_client_specific_tag(csp->config->client_tags, tag_name);
   if (tag == NULL)
   {
      privoxy_mutex_unlock(&client_tags_mutex);
      return JB_ERR_PARSE;
   }

   if (client_has_requested_tag(csp->client_address, tag_name))
   {
      log_error(LOG_LEVEL_ERROR,
         "Tag '%s' already enabled for client '%s'", tag->name, csp->client_address);
   }
   else
   {
      add_tag_for_client(csp->client_address, tag_name, time_to_live);
      log_error(LOG_LEVEL_INFO,
         "Tag '%s' enabled for client '%s'. TTL: %d.",
         tag->name, csp->client_address, time_to_live);
   }

   privoxy_mutex_unlock(&client_tags_mutex);

   return JB_ERR_OK;

}

/*********************************************************************
 *
 * Function    :  disable_client_specific_tag
 *
 * Description :  Disables a client-specific-tag for the client
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  tag_name = The name of the tag to disable
 *
 * Returns     :  JB_ERR_OK on success, JB_ERR_MEMORY or JB_ERR_PARSE.
 *
 *********************************************************************/
jb_err disable_client_specific_tag(struct client_state *csp, const char *tag_name)
{
   struct client_tag_spec *tag;

   privoxy_mutex_lock(&client_tags_mutex);

   tag = get_client_specific_tag(csp->config->client_tags, tag_name);
   if (tag == NULL)
   {
      privoxy_mutex_unlock(&client_tags_mutex);
      return JB_ERR_PARSE;
   }

   if (client_has_requested_tag(csp->client_address, tag_name))
   {
      remove_tag_for_client(csp->client_address, tag_name);
      log_error(LOG_LEVEL_INFO,
         "Tag '%s' disabled for client '%s'", tag->name, csp->client_address);
   }
   else
   {
      log_error(LOG_LEVEL_ERROR,
         "Tag '%s' currently not set for client '%s'",
         tag->name, csp->client_address);
   }

   privoxy_mutex_unlock(&client_tags_mutex);
   return JB_ERR_OK;

}


/*********************************************************************
 *
 * Function    :  client_tag_match
 *
 * Description :  Compare a client tag against a client tag pattern.
 *
 * Parameters  :
 *          1  :  pattern = a TAG pattern
 *          2  :  tag = Client tag to match
 *
 * Returns     :  Nonzero if the tag matches the pattern, else 0.
 *
 *********************************************************************/
int client_tag_match(const struct pattern_spec *pattern,
                     const struct list *tags)
{
   struct list_entry *tag;

   if (!(pattern->flags & PATTERN_SPEC_CLIENT_TAG_PATTERN))
   {
      /*
       * It's not a client pattern and thus shouldn't
       * be matched against client tags.
       */
      return 0;
   }

   assert(tags);

   for (tag = tags->first; tag != NULL; tag = tag->next)
   {
      if (0 == regexec(pattern->pattern.tag_regex, tag->str, 0, NULL, 0))
      {
         return 1;
      }
   }

   return 0;

}


/*********************************************************************
 *
 * Function    :  set_client_address
 *
 * Description :  Sets the client address that will be used to enable,
 *                disable, or apply client tags.
 *
 * Parameters  :
 *          1  :  csp = Current client state (buffers, headers, etc...)
 *          2  :  headers = Client headers
 *
 * Returns     :  void.
 *
 *********************************************************************/
void set_client_address(struct client_state *csp, const struct list *headers)
{
   if (csp->config->trust_x_forwarded_for)
   {
      const char *client_address;

      client_address = get_header_value(headers, "X-Forwarded-For:");
      if (client_address != NULL)
      {
         csp->client_address = strdup_or_die(client_address);
         log_error(LOG_LEVEL_HEADER,
            "Got client address %s from X-Forwarded-For header",
            csp->client_address);
      }
   }

   if (csp->client_address == NULL)
   {
      csp->client_address = strdup_or_die(csp->ip_addr_str);
   }
}

#else
#error Compiling client-tags.c without FEATURE_CLIENT_TAGS
#endif /* def FEATURE_CLIENT_TAGS */
