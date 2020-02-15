/*********************************************************************
 *
 * File        :  doc/source/readme.sgml
 *
 * Purpose     :  README file to give a short intro.
 *
 * Copyright   :  Written by and Copyright (C) 2001-2018 the
 *                Privoxy team. https://www.privoxy.org/
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
 *                or write to the Free Software Foundation, Inc.,
 *                51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 *                USA
 *
 *********************************************************************/

This README is included with Privoxy 3.0.28. See https://www.privoxy.org/ for
more information. The current code maturity level is "stable".

-------------------------------------------------------------------------------

Privoxy is a non-caching web proxy with advanced filtering capabilities for
enhancing privacy, modifying web page data and HTTP headers, controlling
access, and removing ads and other obnoxious Internet junk. Privoxy has a
flexible configuration and can be customized to suit individual needs and
tastes. It has application for both stand-alone systems and multi-user
networks.

Privoxy is Free Software and licensed under the GNU GPLv2.

Privoxy is an associated project of Software in the Public Interest (SPI).

Helping hands and donations are welcome:

  * https://www.privoxy.org/faq/general.html#PARTICIPATE

  * https://www.privoxy.org/faq/general.html#DONATE

-------------------------------------------------------------------------------

1. CHANGES

For a list of changes in this release, please have a look at the "ChangeLog",
the "What's New" section or the "Upgrader's Notes" in the User Manual.

-------------------------------------------------------------------------------

2. INSTALL

See the INSTALL file in this directory, for installing from raw source, and the
User Manual, for all other installation types.

-------------------------------------------------------------------------------

3. RUN

privoxy [--help] [--version] [--no-daemon] [--pidfile PIDFILE] [--user USER
[.GROUP]] [--chroot] [--pre-chroot-nslookup HOSTNAME ][config_file]

See the man page or User Manual for an explanation of each option, and other
configuration and usage issues.

If no config_file is specified on the command line, Privoxy will look for a
file named 'config' in the current directory (except Win32 which will look for
'config.txt'). If no config_file is found, Privoxy will fail to start.

-------------------------------------------------------------------------------

4. CONFIGURATION

See: 'config', 'default.action', 'user.action', 'default.filter', and
'user.filter'. 'user.action' and 'user.filter' are for personal and local
configuration preferences. These are all well commented. Most of the magic is
in '*.action' files. 'user.action' should be used for any actions
customizations. On Unix-like systems, these files are typically installed in /
etc/privoxy. On Windows, then wherever the executable itself is installed.
There are many significant changes and advances from earlier versions. The User
Manual has an explanation of all configuration options, and examples: https://
www.privoxy.org/user-manual/.

Be sure to set your browser(s) for HTTP/HTTPS Proxy at <IP>:<Port>, or whatever
you specify in the config file under 'listen-address'. DEFAULT is
127.0.0.1:8118. Note that Privoxy ONLY proxies HTTP (and HTTPS) traffic. Do not
try it with FTP or other protocols for the simple reason it does not work.

The actions list can be configured via the web interface accessed via http://
p.p/, as well other options.

-------------------------------------------------------------------------------

5. DOCUMENTATION

There should be documentation in the 'doc' subdirectory. In particular, see the
User Manual there, the FAQ, and those interested in Privoxy development, should
look at developer-manual.

The source and configuration files are all well commented. The main
configuration files are: 'config', 'default.action', and 'default.filter'.

Included documentation may vary according to platform and packager. All
documentation is posted on https://www.privoxy.org, in case you don't have it,
or can't find it.

-------------------------------------------------------------------------------

6. CONTACTING THE DEVELOPERS, BUG REPORTING AND FEATURE REQUESTS

We value your feedback. In fact, we rely on it to improve Privoxy and its
configuration. However, please note the following hints, so we can provide you
with the best support.

-------------------------------------------------------------------------------

6.1. Please provide sufficient information

A lot of support requests don't contain enough information and can't be solved
without a lot of back and forth which causes unnecessary delays. Reading this
section should help to prevent that.

Before contacting us to report a problem, please try to verify that it is a 
Privoxy problem, and not a browser or site problem or documented behaviour that
just happens to be different than what you expected. If unsure, try toggling
off Privoxy, and see if the problem persists.

If you are using your own custom configuration, please try the default
configuration to see if the problem is configuration related. If you're having
problems with a feature that is disabled by default, please ask around on the
mailing list if others can reproduce the problem.

If you aren't using the latest Privoxy version, the problem may have been found
and fixed in the meantime. We would appreciate if you could take the time to
upgrade to the latest version and verify that the problem still exists.

Please be sure to provide the following information when reporting problems or
requesting support:

  * The exact Privoxy version you are using.

  * The operating system and versions you run Privoxy on, e.g. Windows XP SP2.

  * The name, platform, and version of the browser you were using (e.g. 
    Internet Explorer v5.5 for Mac).

  * The URL where the problem occurred, or some way for us to duplicate the
    problem (e.g. http://somesite.example.com/?somethingelse=123).

  * Whether your version of Privoxy is one supplied by the Privoxy developers
    via SourceForge, or if you got your copy somewhere else.

  * Whether you are using Privoxy together with another proxy such as Tor. If
    so, please temporary disable the other proxy to see if the symptoms change.

  * Whether you are using a personal firewall product. If so, does Privoxy work
    without it?

  * Any other pertinent information to help identify the problem such as config
    or log file excerpts (yes, you should have log file entries for each action
    taken). To get a meaningful logfile, please make sure that the logfile
    directive is being used and the following debug options are enabled (all of
    them):

    debug     1 # Log the destination for each request Privoxy let through.
                #   See also debug 1024.
    debug     2 # show each connection status
    debug     4 # show I/O status
    debug     8 # show header parsing
    debug   128 # debug redirects
    debug   256 # debug GIF de-animation
    debug   512 # Common Log Format
    debug  1024 # Log the destination for requests Privoxy didn't let through,
                #   and the reason why.
    debug  4096 # Startup banner and warnings.
    debug  8192 # Non-fatal errors
    debug 65536 # Log applying actions

    If you are having trouble with a filter, please additionally enable

    debug    64 # debug regular expression filters

    If you suspect that Privoxy interprets the request or the response
    incorrectly, please enable

    debug 32768 # log all data read from the network

    It's easy for us to ignore log messages that aren't relevant but missing
    log messages may make it impossible to investigate a problem. If you aren't
    sure which of the debug directives are relevant, please just enable all of
    them and let us worry about it.

    Note that Privoxy log files may contain sensitive information so please
    don't submit any logfiles you didn't read first. You can mask sensitive
    information as long as it's clear that you removed something.

You don't have to tell us your actual name when filing a problem report, but if
you don't, please use a nickname so we can differentiate between your messages
and the ones entered by other "anonymous" users that may respond to your
request if they have the same problem or already found a solution. Note that
due to spam the trackers may not always allow to post without being logged into
SourceForge. If that's the case, you are still free to create a login that
isn't directly linked to your name, though.

Please also check the status of your request a few days after submitting it, as
we may request additional information. If you use a SF id, you should
automatically get a mail when someone responds to your request. Please don't
bother to add an email address when using the tracker. If you prefer to
communicate through email, just use one of the mailing lists directly.

If you are new to reporting problems, you might be interested in How to Report
Bugs Effectively.

The appendix of the Privoxy User Manual also has helpful information on
understanding actions, and action debugging.

-------------------------------------------------------------------------------

6.2. Get Support

All users are welcome to discuss their issues on the users mailing list, where
the developers also hang around.

Please don't send private support requests to individual Privoxy developers,
either use the mailing lists or the support trackers.

If you have to contact a Privoxy developer directly for other reasons, please
send a real mail and do not bother with SourceForge's messaging system. Answers
to SourceForge messages are usually bounced by SourceForge's mail server in
which case the developer wasted time writing a response you don't get. From
your point of view it will look like your message has been completely ignored,
so this is frustrating for all parties involved.

Note that the Privoxy mailing lists are moderated. Posts from unsubscribed
addresses have to be accepted manually by a moderator. This may cause a delay
of several days and if you use a subject that doesn't clearly mention Privoxy
or one of its features, your message may be accidentally discarded as spam.

If you aren't subscribed, you should therefore spend a few seconds to come up
with a proper subject. Additionally you should make it clear that you want to
get CC'd. Otherwise some responses will be directed to the mailing list only,
and you won't see them.

-------------------------------------------------------------------------------

6.3. Reporting Problems

"Problems" for our purposes, come in two forms:

  * Configuration issues, such as ads that slip through, or sites that don't
    function properly due to one Privoxy "action" or another being turned "on".

  * "Bugs" in the programming code that makes up Privoxy, such as that might
    cause a crash. Documentation issues, for example spelling errors and
    unclear descriptions, are bugs, too.

-------------------------------------------------------------------------------

6.3.1. Reporting Ads or Other Configuration Problems

Please send feedback on ads that slipped through, innocent images that were
blocked, sites that don't work properly, and other configuration related
problem of default.action file, to https://sourceforge.net/tracker/?group_id=
11118&atid=460288, the Actions File Tracker.

-------------------------------------------------------------------------------

6.3.2. Reporting Bugs

Before reporting bugs, please make sure that the bug has not already been
submitted and observe the additional hints at the top of the submit form. If
already submitted, please feel free to add any info to the original report that
might help to solve the issue.

-------------------------------------------------------------------------------

6.4. Reporting security problems

If you discovered a security problem or merely suspect that a bug might be a
security issue, please mail Fabian Keil <fk@fabiankeil.de> (OpenPGP
fingerprint: 4F36 C17F 3816 9136 54A1 E850 6918 2291 8BA2 371C).

Usually you should get a response within a day, otherwise it's likely that
either your mail or the response didn't make it. If that happens, please mail
to the developer list to request a status update.

-------------------------------------------------------------------------------

6.5. Mailing Lists

If you prefer to communicate through email, instead of using a web interface,
feel free to use one of the mailing lists. To discuss issues that haven't been
completely diagnosed yet, please use the Privoxy users list. Technically
interested users and people who wish to contribute to the project are always
welcome on the developers list. You can find an overview of all Privoxy-related
mailing lists, including list archives, at: https://lists.privoxy.org/mailman/
listinfo. The lists hosted on privoxy.org have been created in 2016, the
previously-used lists hosted at SourceForge are deprecated but the archives may
still be useful: https://sourceforge.net/mail/?group_id=11118.

-------------------------------------------------------------------------------

6.6. SourceForge support trackers

The SourceForge support trackers may be used as well, but have various
technical problems that are unlikely to be fixed anytime soon. If you don't get
a timely response, please try the mailing list as well.

