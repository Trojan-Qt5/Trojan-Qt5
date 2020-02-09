/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/fuzz.c,v $
 *
 * Purpose     :  Fuzz-related functions for Privoxy.
 *
 * Copyright   :  Written by and Copyright (C) 2014-16 by
 *                Fabian Keil <fk@fabiankeil.de>
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


#include "config.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "project.h"
#include "filters.h"
#include "loaders.h"
#include "parsers.h"
#include "miscutil.h"
#include "errlog.h"
#include "actions.h"
#include "cgi.h"
#include "loadcfg.h"
#include "urlmatch.h"
#include "filters.h"
#include "jbsockets.h"
#include "gateway.h"
#include "jcc.h"
#include "list.h"


#ifdef FUZZ
static int fuzz_action(struct client_state *csp, char *fuzz_input_file);
static int fuzz_client_header(struct client_state *csp, char *fuzz_input_file);
static int fuzz_deflate(struct client_state *csp, char *fuzz_input_file);
static int fuzz_filter(struct client_state *csp, char *fuzz_input_file);
static int fuzz_gif(struct client_state *csp, char *fuzz_input_file);
static int fuzz_gzip(struct client_state *csp, char *fuzz_input_file);
static int fuzz_socks(struct client_state *csp, char *fuzz_input_file);
static int fuzz_pcrs_substitute(struct client_state *csp, char *fuzz_input_file);
static int fuzz_server_header(struct client_state *csp, char *fuzz_input_file);

struct fuzz_mode
{
   const char *name;
   const char *expected_input;
   const int stdin_support;
   int (* const handler)(struct client_state *csp, char *input_file);
};

static const struct fuzz_mode fuzz_modes[] = {
   { "action", "Text to parse as action file.", 0, fuzz_action },
   { "client-request", "Client request to parse. Currently incomplete", 1, fuzz_client_request },
   { "client-header", "Client header to parse.", 1, fuzz_client_header },
   { "chunked-transfer-encoding", "Chunk-encoded data to dechunk.", 1, fuzz_chunked_transfer_encoding },
   { "deflate", "deflate-compressed data to decompress.", 1, fuzz_deflate },
   { "filter", "Text to parse as filter file.", 0, fuzz_filter },
   { "gif", "gif to deanimate.", 1, fuzz_gif },
   { "gzip", "gzip-compressed data to decompress.", 1, fuzz_gzip },
   { "pcrs-substitute", "A pcrs-substitute to compile. Not a whole pcrs job! Example: Bla $1 bla \x43 $3 blah.", 1, fuzz_pcrs_substitute },
   { "server-header", "Server header to parse.", 1, fuzz_server_header },
   { "server-response", "Server response to parse.", 1, fuzz_server_response },
   { "socks", "A socks server response. Only reads from stdin!", 1, fuzz_socks },
};

/*********************************************************************
 *
 * Function    :  load_fuzz_input_from_stdin
 *
 * Description :  Loads stdin into a buffer.
 *
 * Parameters  :
 *          1  :  csp = Used to store the data.
 *
 * Returns     :  JB_ERR_OK in case of success,
 *
 *********************************************************************/
static jb_err load_fuzz_input_from_stdin(struct client_state *csp)
{
   static char buf[BUFFER_SIZE];
   int ret;

   while (0 < (ret = read_socket(0, buf, sizeof(buf))))
   {
      log_error(LOG_LEVEL_INFO,
         "Got %d bytes from stdin: %E. They look like this: %N",
         ret, ret, buf);

      if (add_to_iob(csp->iob, csp->config->buffer_limit, buf, ret))
      {
         log_error(LOG_LEVEL_FATAL, "Failed to buffer them.");
      }
   }

   log_error(LOG_LEVEL_INFO, "Read %d bytes from stdin",
      csp->iob->eod -csp->iob->cur);

   return JB_ERR_OK;
}

/*********************************************************************
 *
 * Function    :  load_fuzz_input_from_file
 *
 * Description :  Loads file content into a buffer.
 *
 * Parameters  :
 *          1  :  csp = Used to store the data.
 *          2  :  filename = Name of the file to be loaded.
 *
 * Returns     :  JB_ERR_OK in case of success,
 *
 *********************************************************************/
static jb_err load_fuzz_input_from_file(struct client_state *csp, const char *filename)
{
   FILE *fp;
   size_t length;
   long ret;

   fp = fopen(filename, "rb");
   if (NULL == fp)
   {
      log_error(LOG_LEVEL_FATAL, "Failed to open %s: %E", filename);
   }

   /* Get file length */
   if (fseek(fp, 0, SEEK_END))
   {
      log_error(LOG_LEVEL_FATAL,
         "Unexpected error while fseek()ing to the end of %s: %E",
         filename);
   }
   ret = ftell(fp);
   if (-1 == ret)
   {
      log_error(LOG_LEVEL_FATAL,
         "Unexpected ftell() error while loading %s: %E",
         filename);
   }
   length = (size_t)ret;

   /* Go back to the beginning. */
   if (fseek(fp, 0, SEEK_SET))
   {
      log_error(LOG_LEVEL_FATAL,
         "Unexpected error while fseek()ing to the beginning of %s: %E",
         filename);
   }

   csp->iob->size = length + 1;

   csp->iob->buf = malloc_or_die(csp->iob->size);
   csp->iob->cur = csp->iob->buf;
   csp->iob->eod = csp->iob->buf + length;

   if (1 != fread(csp->iob->cur, length, 1, fp))
   {
      /*
       * May theoretically happen if the file size changes between
       * fseek() and fread() because it's edited in-place. Privoxy
       * and common text editors don't do that, thus we just fail.
       */
      log_error(LOG_LEVEL_FATAL,
         "Couldn't completely read file %s.", filename);
   }
   *csp->iob->eod = '\0';

   fclose(fp);

   return JB_ERR_OK;

}

/*********************************************************************
 *
 * Function    :  load_fuzz_input
 *
 * Description :  Loads a file into a buffer. XXX: Reverse argument order
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  filename = Name of the file to be loaded.
 *
 * Returns     :  JB_ERR_OK in case of success,
 *
 *********************************************************************/
jb_err load_fuzz_input(struct client_state *csp, const char *filename)
{
   if (strcmp(filename, "-") == 0)
   {
      return load_fuzz_input_from_stdin(csp);
   }

   return load_fuzz_input_from_file(csp, filename);
}


/*********************************************************************
 *
 * Function    :  remove_forbidden_bytes
 *
 * Description :  Sanitizes fuzzed data to decrease the likelihood of
 *                premature parse abortions.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *
 * Returns     :  N/A
 *
 *********************************************************************/
static void remove_forbidden_bytes(struct client_state *csp)
{
   char *p = csp->iob->cur;
   char first_valid_byte = ' ';

   while (p < csp->iob->eod)
   {
      if (*p != '\0')
      {
         first_valid_byte = *p;
         break;
      }
      p++;
   }

   p = csp->iob->cur;
   while (p < csp->iob->eod)
   {
      if (*p == '\0')
      {
         *p = first_valid_byte;
      }
      p++;
   }
}


/*********************************************************************
 *
 * Function    :  fuzz_action
 *
 * Description :  Treat the fuzzed input as action file.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
int fuzz_action(struct client_state *csp, char *fuzz_input_file)
{
   csp->config->actions_file[0] = fuzz_input_file;

   return(load_action_files(csp));
}


/*********************************************************************
 *
 * Function    :  fuzz_client_header
 *
 * Description :  Treat the fuzzed input as a client header.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
int fuzz_client_header(struct client_state *csp, char *fuzz_input_file)
{
   char *header;

   header = get_header(csp->iob);

   if (NULL == header)
   {
      return 1;
   }
   if (JB_ERR_OK != enlist(csp->headers, header))
   {
      return 1;
   }

   /*
    * Silence an insightful client_host_adder() warning
    * about ignored weirdness.
    */
   csp->flags |= CSP_FLAG_HOST_HEADER_IS_SET;
   /* Adding headers doesn't depend on the fuzzed input */
   csp->flags |= CSP_FLAG_CLIENT_CONNECTION_HEADER_SET;

   /* +hide-if-modified-since{+60} */
   csp->action->flags |= ACTION_HIDE_IF_MODIFIED_SINCE;
   csp->action->string[ACTION_STRING_IF_MODIFIED_SINCE] = "+60";

   /* XXX: Enable more actions. */

   return(sed(csp, FILTER_CLIENT_HEADERS));
}


/*********************************************************************
 *
 * Function    :  fuzz_filter
 *
 * Description :  Treat the fuzzed input as filter file.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
int fuzz_filter(struct client_state *csp, char *fuzz_input_file)
{
   csp->config->re_filterfile[0] = fuzz_input_file;
   return (load_one_re_filterfile(csp, 0));
}


/*********************************************************************
 *
 * Function    :  fuzz_deflate
 *
 * Description :  Treat the fuzzed input as data to deflate.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
static int fuzz_deflate(struct client_state *csp, char *fuzz_input_file)
{
   csp->content_type = CT_DEFLATE;
   return(JB_ERR_OK == decompress_iob(csp));
}


/*********************************************************************
 *
 * Function    :  fuzz_gif
 *
 * Description :  Treat the fuzzed input as a gif to deanimate.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
static int fuzz_gif(struct client_state *csp, char *fuzz_input_file)
{
   char *deanimated_gif;

   if (6 < csp->iob->size)
   {
      /* Why yes of course, officer, this is a gif. */
      memcpy(csp->iob->cur, "GIF87a", 6);
   }

   /* Using the last image requires parsing of all images */
   csp->action->string[ACTION_STRING_DEANIMATE] = "last";
   deanimated_gif = gif_deanimate_response(csp);
   if (NULL != deanimated_gif)
   {
      free(deanimated_gif);
      return 0;
   }

   return 1;

}


/*********************************************************************
 *
 * Function    :  fuzz_gzip
 *
 * Description :  Treat the fuzzed input as data to unzip
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
static int fuzz_gzip(struct client_state *csp, char *fuzz_input_file)
{
   csp->content_type = CT_GZIP;

   return(JB_ERR_OK == decompress_iob(csp));

}


/*********************************************************************
 *
 * Function    :  fuzz_socks
 *
 * Description :  Treat the fuzzed input as a socks response.
 *                XXX: This is pretty useless as parsing socks repsonse
 *                     is trivial.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
static int fuzz_socks(struct client_state *csp, char *fuzz_input_file)
{
   return(JB_ERR_OK == socks_fuzz(csp));
}


/*********************************************************************
 *
 * Function    :  fuzz_pcrs_substitute
 *
 * Description :  Treat the fuzzed input as a pcrs substitute.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
static int fuzz_pcrs_substitute(struct client_state *csp, char *fuzz_input_file)
{
   static pcrs_substitute *result;
   int err;

   remove_forbidden_bytes(csp);
   result = pcrs_compile_fuzzed_replacement(csp->iob->cur, &err);
   if (NULL == result)
   {
      log_error(LOG_LEVEL_ERROR,
         "Failed to compile pcrs replacement. Error: %s", pcrs_strerror(err));
      return 1;
   }
   log_error(LOG_LEVEL_INFO, "%s", pcrs_strerror(err));
   free(result->text);
   freez(result);
   return 0;
}


/*********************************************************************
 *
 * Function    :  fuzz_server_header
 *
 * Description :  Treat the fuzzed input as a server header.
 *
 * Parameters  :
 *          1  :  csp      = Used to store the data.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Result of fuzzed function
 *
 *********************************************************************/
int fuzz_server_header(struct client_state *csp, char *fuzz_input_file)
{
   char *header;

   header = get_header(csp->iob);

   if (NULL == header)
   {
      return 1;
   }
   if (JB_ERR_OK != enlist(csp->headers, header))
   {
      return 1;
   }

   /* Adding headers doesn't depend on the fuzzed input */
   csp->flags |= CSP_FLAG_CLIENT_HEADER_PARSING_DONE;
   csp->flags |= CSP_FLAG_SERVER_CONNECTION_HEADER_SET;

   /* +overwrite-last-modified{randomize} */
   csp->action->flags |= ACTION_OVERWRITE_LAST_MODIFIED;
   csp->action->string[ACTION_STRING_LAST_MODIFIED] = "randomize";

   /* +limit-cookie-lifetime{60} */
   csp->action->flags |= ACTION_LIMIT_COOKIE_LIFETIME;
   csp->action->string[ACTION_STRING_LIMIT_COOKIE_LIFETIME] = "60";

   /* XXX: Enable more actions. */

   return(sed(csp, FILTER_SERVER_HEADERS));
}

/*********************************************************************
 *
 * Function    :  process_fuzzed_input
 *
 * Description :  Process the fuzzed input in a specified file treating
 *                it like the input type specified.
 *
 * Parameters  :
 *          1  :  fuzz_input_type = Type of input.
 *          2  :  fuzz_input_file = File to read the input from.
 *
 * Returns     : Return value of the fuzzed function
 *
 *********************************************************************/
int process_fuzzed_input(char *fuzz_input_type, char *fuzz_input_file)
{
   static struct client_state csp_stack_storage;
   static struct configuration_spec config_stack_storage;
   struct client_state *csp;
   int i;

   csp = &csp_stack_storage;
   csp->config = &config_stack_storage;
   csp->config->buffer_limit = 4096 * 1024;
   csp->config->receive_buffer_size = 4096;

   /* In --stfu mode, these will be ignored ... */
   set_debug_level(LOG_LEVEL_ACTIONS|LOG_LEVEL_CONNECT|LOG_LEVEL_DEANIMATE|LOG_LEVEL_INFO|LOG_LEVEL_ERROR|LOG_LEVEL_RE_FILTER|LOG_LEVEL_HEADER|LOG_LEVEL_WRITING|LOG_LEVEL_RECEIVED);

   csp->flags |= CSP_FLAG_FUZZED_INPUT;
   csp->config->feature_flags |= RUNTIME_FEATURE_ACCEPT_INTERCEPTED_REQUESTS;

#ifdef FEATURE_CLIENT_TAGS
   csp->config->trust_x_forwarded_for = 1;
#endif

   for (i = 0; i < SZ(fuzz_modes); i++)
   {
      if (strcmp(fuzz_modes[i].name, fuzz_input_type) == 0)
      {
         if (fuzz_modes[i].stdin_support &&
            (strcmp(fuzz_input_type, "client-request") != 0) &&
            (strcmp(fuzz_input_type, "server-response") != 0) &&
            (strcmp(fuzz_input_type, "socks") != 0))
         {
            load_fuzz_input(csp, fuzz_input_file);
         }
         return (fuzz_modes[i].handler(csp, fuzz_input_file));
      }
   }

   log_error(LOG_LEVEL_FATAL,
      "Unrecognized fuzz type %s for input file %s. You may need --help.",
      fuzz_input_type, fuzz_input_file);

   /* Not reached. */
   return 1;

}


/*********************************************************************
 *
 * Function    :  show_fuzz_usage
 *
 * Description :  Shows the --fuzz usage. D'oh.
 *
 * Parameters  :  Pointer to argv[0] for identifying ourselves
 *
 * Returns     :  void
 *
 *********************************************************************/
void show_fuzz_usage(const char *name)
{
   int i;

   printf("%s%s --fuzz fuzz-mode ./path/to/fuzzed/input [--stfu]\n\n",
      "       ", name);

   printf("Supported fuzz modes and the expected input:\n");
   for (i = 0; i < SZ(fuzz_modes); i++)
   {
      printf(" %s: %s\n", fuzz_modes[i].name, fuzz_modes[i].expected_input);
   }
   printf("\n");

   printf("The following fuzz modes read data from stdin if the 'file' is '-'\n");
   for (i = 0; i < SZ(fuzz_modes); i++)
   {
      if (fuzz_modes[i].stdin_support)
      {
         printf(" %s\n", fuzz_modes[i].name);
      }
   }
   printf("\n");
}
#endif
