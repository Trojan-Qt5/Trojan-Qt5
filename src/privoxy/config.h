/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/acconfig.h,v $
 *
 * Purpose     :  This file should be the first thing included in every
 *                .c file.  (Before even system headers).  It contains
 *                #define statements for various features.  It was
 *                introduced because the compile command line started
 *                getting ludicrously long with feature defines.
 *
 * Copyright   :  Written by and Copyright (C) 2001-2014 the
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


/*
 * Version number - Major (X._._)
 */
#define VERSION_MAJOR 3

/*
 * Version number - Minor (_.X._)
 */
#define VERSION_MINOR 0

/*
 * Version number - Point (_._.X)
 */
#define VERSION_POINT 28

/*
 * Version number, as a string
 */
#define VERSION "3.0.28"

/*
 * Status of the code: "alpha", "beta" or "stable".
 */
#define CODE_STATUS "stable"

/*
 * Should pcre be statically built in instead of linkling with libpcre?
 * (This is determined by configure depending on the availiability of
 * libpcre and user preferences).
 * Don't bother to change this here! Use configure instead.
 */
#define FEATURE_DYNAMIC_PCRE 1

/*
 * Should pcrs be statically built in instead of linkling with libpcrs?
 * (This is determined by configure depending on the availiability of
 * libpcrs and user preferences).
 * Don't bother to change this here! Use configure instead.
 */
#define STATIC_PCRS 1

/*
 * Allows the use of an ACL to control access to the proxy by IP address.
 */
#define FEATURE_ACL 1

/*
 * Allow Privoxy to use accf_http(9) if supported.
 */
/* #undef FEATURE_ACCEPT_FILTER */

/*
 * Enables the web-based configuration (actionsfile) editor.  If you
 * have a shared proxy, you might want to turn this off.
 */
#define FEATURE_CGI_EDIT_ACTIONS 1

/*
 * Locally redirect remote script-redirect URLs
 */
#define FEATURE_FAST_REDIRECTS 1

/*
 * Bypass filtering for 1 page only
 */
#define FEATURE_FORCE_LOAD 1

/*
 * Allow blocking using images as well as HTML.
 * If you do not define this then everything is blocked as HTML.
 */
#define FEATURE_IMAGE_BLOCKING 1

/*
 * Use PNG instead of GIF for built-in images
 */
/* #undef FEATURE_NO_GIFS */

/*
 * Allow to shutdown Privoxy through the webinterface.
 */
/* #undef FEATURE_GRACEFUL_TERMINATION */

/*
 * Allow PCRE syntax in host patterns.
 */
/* #undef FEATURE_EXTENDED_HOST_PATTERNS */

/*
 * Allow filtering with scripts and programs.
 */
/* #undef FEATURE_EXTERNAL_FILTERS */

/*
 * Keep connections alive if possible.
 */
#define FEATURE_CONNECTION_KEEP_ALIVE 1

/*
 * Allow to share outgoing connections between incoming connections.
 */
#define FEATURE_CONNECTION_SHARING 1

/*
 * Use POSIX threads instead of native threads.
 */
#define FEATURE_PTHREAD 1

/*
 * Enables statistics function.
 */
#define FEATURE_STATISTICS 1

/*
 * Enable strptime() sanity checks.
 */
/* #undef FEATURE_STRPTIME_SANITY_CHECKS */

/*
 * Allow Privoxy to be "disabled" so it is just a normal non-blocking
 * non-anonymizing proxy.  This is useful if you're trying to access a
 * blocked or broken site - just change the setting in the config file,
 * or use the handy "Disable" menu option in the Windows GUI.
 */
#define FEATURE_TOGGLE 1

/*
 * Allows the use of trust files.
 */
#define FEATURE_TRUST 1

/*
 * Defined on Solaris only.  Makes the system libraries thread safe.
 */
/* #undef _REENTRANT */

/*
 * Defined on Solaris only.  Without this, many important functions are not
 * defined in the system headers.
 */
/* #undef __EXTENSIONS__ */

/*
 * Defined always.
 * FIXME: Don't know what it does or why we need it.
 * (presumably something to do with MultiThreading?)
 */
#define __MT__ 1

/* If the (nonstandard and thread-safe) function gethostbyname_r
 * is available, select which signature to use
 */
/* #undef HAVE_GETHOSTBYNAME_R_6_ARGS */
/* #undef HAVE_GETHOSTBYNAME_R_5_ARGS */
/* #undef HAVE_GETHOSTBYNAME_R_3_ARGS */

/* If the (nonstandard and thread-safe) function gethostbyaddr_r
 * is available, select which signature to use
 */
/* #undef HAVE_GETHOSTBYADDR_R_8_ARGS */
/* #undef HAVE_GETHOSTBYADDR_R_7_ARGS */
/* #undef HAVE_GETHOSTBYADDR_R_5_ARGS */

/* Defined if you have gmtime_r and localtime_r with a signature
 * of (struct time *, struct tm *)
 */
#define HAVE_GMTIME_R 1
#define HAVE_LOCALTIME_R 1

/* Define to 'int' if <sys/socket.h> doesn't have it.
 */
/* #undef socklen_t */

/* Define if pcre.h must be included as <pcre/pcre.h>
 */
/* #undef PCRE_H_IN_SUBDIR */

/* Define if pcreposix.h must be included as <pcre/pcreposix.h>
 */
/* #undef PCREPOSIX_H_IN_SUBDIR */


/* Relevant for select(). Not honoured by all OS. */
/* #undef FD_SETSIZE */

/* Define to enable support for client-specific tags. */
#define FEATURE_CLIENT_TAGS 1

/* Define to 1 to use compression through the zlib library. */
/* #undef FEATURE_COMPRESSION */

/* Define to dynamically link to pcre. */
#define FEATURE_DYNAMIC_PCRE 1

/* Define to 1 to allow to filter content with scripts and programs. */
/* #undef FEATURE_EXTERNAL_FILTERS */

/* Define to 1 to use zlib to decompress data before filtering. */
#define FEATURE_ZLIB 1

/* Define to make fuzzing more convenient. */
/* #undef FUZZ */

/* Define to 1 if you have the `access' function. */
#define HAVE_ACCESS 1

/* Define to 1 if you have the `arc4random' function. */
#define HAVE_ARC4RANDOM 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Define to 1 if you have the `bcopy' function. */
#define HAVE_BCOPY 1

/* Define to 1 if you have the `calloc' function. */
#define HAVE_CALLOC 1

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `gethostbyaddr' function. */
#define HAVE_GETHOSTBYADDR 1

/* Define to 1 if you have the `gethostbyaddr_r' function. */
/* #undef HAVE_GETHOSTBYADDR_R */

/* Define to 1 if you have the `gethostbyname' function. */
#define HAVE_GETHOSTBYNAME 1

/* Define to 1 if you have the `gethostbyname_r' function. */
/* #undef HAVE_GETHOSTBYNAME_R */

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `inet_ntoa' function. */
#define HAVE_INET_NTOA 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `ws2_32' library (-lws2_32). */
/* #undef HAVE_LIBWS2_32 */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the `memchr' function. */
#define HAVE_MEMCHR 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `nanosleep' function. */
#define HAVE_NANOSLEEP 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <OS.h> header file. */
/* #undef HAVE_OS_H */

/* Define to 1 if you have the `poll' function. */
#define HAVE_POLL 1

/* Define to 1 if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define to 1 if you have the `random' function. */
#define HAVE_RANDOM 1

/* Define to 1 if you have the `regcomp' function. */
#define HAVE_REGCOMP 1

/* Define if RFC 2553 resolver functions like getaddrinfo(3) and
   getnameinfo(3) present */
#define HAVE_RFC2553 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the `shutdown' function. */
#define HAVE_SHUTDOWN 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strlcat' function. */
#define HAVE_STRLCAT 1

/* Define to 1 if you have the `strlcpy' function. */
#define HAVE_STRLCPY 1

/* Define to 1 if you have the `strptime' function. */
#define HAVE_STRPTIME 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the `timegm' function. */
#define HAVE_TIMEGM 1

/* Define to 1 if you have the `tzset' function. */
#define HAVE_TZSET 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* The size of `char *', as computed by sizeof. */
#define SIZEOF_CHAR_P 8

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 8

/* The size of `time_t', as computed by sizeof. */
#define SIZEOF_TIME_T 8

/* Define to statically link to internal outdated pcre on Windows. */
/* #undef STATIC_PCRE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to 'int' if <sys/socket.h> doesn't have it. */
/* #undef socklen_t */

/*
 * Defined always.
 * FIXME: Don't know what it does or why we need it.
 * (presumably something to do with ANSI Standard C?)
 */
#ifndef __STDC__
#define __STDC__ 1
#endif /* ndef __STDC__ */

/*
 * Need to set up this define only for the Pthreads library for
 * Win32, available from http://sources.redhat.com/pthreads-win32/
 */
#if defined(FEATURE_PTHREAD) && defined(_WIN32)
#define __CLEANUP_C
#endif /* defined(FEATURE_PTHREAD) && defined(_WIN32) */

/*
 * BEOS does not currently support POSIX threads.
 * This *should* be detected by ./configure, but let's be sure.
 */
#if defined(FEATURE_PTHREAD) && defined(__BEOS__)
#error BEOS does not support pthread - please run ./configure again with "--disable-pthread"

#endif /* defined(FEATURE_PTHREAD) && defined(__BEOS__) */

/*
 * On OpenBSD and maybe also FreeBSD, gcc doesn't define the cpp
 * symbol unix; it defines __unix__ and sometimes not even that:
 */
#if ( defined(__unix__) || defined(__NetBSD__) ) && !defined(unix)
#define unix 1aaa
#endif

/*
 * It's too easy to accidentally use a Cygwin or MinGW32 version of config.h
 * under VC++, and it usually gives many weird error messages.  Let's make
 * the error messages understandable, by bailing out now.
 */
#ifdef _MSC_VER
#error For MS VC++, please use vc_config_winthreads.h or vc_config_pthreads.h.  You can usually do this by selecting the "Build", "Clean" menu option.
#endif /* def _MSC_VER */

#endif /* CONFIG_H_INCLUDED */
