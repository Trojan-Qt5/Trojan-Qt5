/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/ssplit.c,v $
 *
 * Purpose     :  A function to split a string at specified delimiters.
 *
 * Copyright   :  Written by and Copyright (C) 2001-2012 the
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


#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "ssplit.h"
#include "miscutil.h"


/*********************************************************************
 *
 * Function    :  ssplit
 *
 * Description :  Split a string using delimiters in `delim'.  Results
 *                go into `vec'.
 *
 * Parameters  :
 *          1  :  str = string to split.  Will be split in place
 *                (i.e. do not free until you've finished with vec,
 *                previous contents will be trashed by the call).
 *          2  :  delim = array of delimiters (if NULL, uses " \t").
 *          3  :  vec[] = results vector (aka. array) [out]
 *          4  :  vec_len = number of usable slots in the vector (aka. array size)
 *
 * Returns     :  -1 => Error: vec_len is too small to hold all the
 *                      data, or str == NULL.
 *                >=0 => the number of fields put in `vec'.
 *                On error, vec and str may still have been overwritten.
 *
 *********************************************************************/
int ssplit(char *str, const char *delim, char *vec[], size_t vec_len)
{
   unsigned char is_delim[256];
   unsigned char char_type;
   int vec_count = 0;
   enum char_type {
      WANTED     = 0,
      SEPARATOR  = 1,
      TERMINATOR = 2,
   };


   if (!str)
   {
      return(-1);
   }


   /* Build is_delim array */

   memset(is_delim, '\0', sizeof(is_delim));

   if (!delim)
   {
      delim = " \t";  /* default field separators */
   }

   while (*delim)
   {
      is_delim[(unsigned)(unsigned char)*delim++] = SEPARATOR;
   }

   is_delim[(unsigned)(unsigned char)'\0'] = TERMINATOR;
   is_delim[(unsigned)(unsigned char)'\n'] = TERMINATOR;


   /* Parse string */

   /* Skip leading separators. XXX: Why do they matter? */
   while (is_delim[(unsigned)(unsigned char)*str] == SEPARATOR)
   {
      str++;
   }

   /* The first pointer is the beginning of string */
   if (is_delim[(unsigned)(unsigned char)*str] == WANTED)
   {
      /*
       * The first character in this field is not a
       * delimiter or the end of string, so save it.
       */
      if (vec_count >= vec_len)
      {
         return(-1); /* overflow */
      }
      vec[vec_count++] = str;
   }

   while ((char_type = is_delim[(unsigned)(unsigned char)*str]) != TERMINATOR)
   {
      if (char_type == SEPARATOR)
      {
         /* the char is a separator */

         /* null terminate the substring */
         *str++ = '\0';

         /* Check if we want to save this field */
         if (is_delim[(unsigned)(unsigned char)*str] == WANTED)
         {
            /*
             * The first character in this field is not a
             * delimiter or the end of string. So save it.
             */
            if (vec_count >= vec_len)
            {
               return(-1); /* overflow */
            }
            vec[vec_count++] = str;
         }
      }
      else
      {
         str++;
      }
   }
   /* null terminate the substring */
   /* XXX: this shouldn't be necessary, so assert that it isn't. */
   assert(*str == '\0');
   *str = '\0';

   return(vec_count);
}


/*
  Local Variables:
  tab-width: 3
  end:
*/
