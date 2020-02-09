#!/usr/bin/perl

##############################################################################################
# uagen (http://www.fabiankeil.de/sourcecode/uagen/)
#
# Generates a pseudo-random Firefox user agent and writes it into a Privoxy action file
# and optionally into a Mozilla prefs file. For documentation see 'perldoc uagen(.pl)'.
#
# Examples (created with v1.0):
#
# Mozilla/5.0 (X11; U; NetBSD i386; en-US; rv:1.8.0.2) Gecko/20060421 Firefox/1.5.0.2
# Mozilla/5.0 (Macintosh; U; Intel Mac OS X; en-CA; rv:1.8.0.2) Gecko/20060425 Firefox/1.5.0.2
# Mozilla/5.0 (X11; U; SunOS i86pc; no-NO; rv:1.8.0.2) Gecko/20060420 Firefox/1.5.0.2
# Mozilla/5.0 (X11; U; Linux x86_64; de-AT; rv:1.8.0.2) Gecko/20060422 Firefox/1.5.0.2
# Mozilla/5.0 (X11; U; NetBSD i386; en-US; rv:1.8.0.2) Gecko/20060415 Firefox/1.5.0.2
# Mozilla/5.0 (X11; U; OpenBSD sparc64; pl-PL; rv:1.8.0.2) Gecko/20060429 Firefox/1.5.0.2
# Mozilla/5.0 (X11; U; Linux i686; en-CA; rv:1.8.0.2) Gecko/20060413 Firefox/1.5.0.2
#
# Copyright (c) 2006-2011 Fabian Keil <fk@fabiankeil.de>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
##############################################################################################

use strict;
use warnings;
use Time::Local;
use Getopt::Long;

use constant {

   UAGEN_VERSION       => 'uagen 1.2.1',

   UAGEN_LOGFILE       => '/var/log/uagen.log',
   ACTION_FILE         => '/etc/privoxy/user-agent.action',
   MOZILLA_PREFS_FILE  => '',
   SILENT              =>  0,
   NO_LOGGING          =>  0,
   NO_ACTION_FILE      =>  0,
   LOOP                =>  0,
   SLEEPING_TIME       =>  5,

   # As of Firefox 4, the "Gecko token" has been frozen
   # http://hacks.mozilla.org/2010/09/final-user-agent-string-for-firefox-4/
   RANDOMIZE_RELEASE_DATE => 0,

   # These variables belong together. If you only change one of them, the generated
   # User-Agent might be invalid. If you're not sure which values make sense,
   # are too lazy to check, but want to change them anyway, take the values you
   # see in the "Help/About Mozilla Firefox" menu.

   BROWSER_VERSION                   => "17.0",
   BROWSER_REVISION                  => '17.0',
   BROWSER_RELEASE_DATE              => '20100101',
};

use constant LANGUAGES => qw(
   en-AU en-GB en-CA en-NZ en-US en-ZW es-ES de-DE de-AT de-CH fr-FR sk-SK nl-NL no-NO pl-PL
);

#######################################################################################

sub generate_creation_time($) {
    my $release_date = shift;

    my ($rel_year, $rel_mon, $rel_day);
    my ($c_day, $c_mon, $c_year);
    my $now = time;
    my (undef, undef, undef, $mday, $mon, $year, undef, undef, undef) = localtime($now);
    $mon  += 1;
    $year += 1900;

    unless ($release_date =~ m/\d{6}/) {
        log_error("Invalid release date format: $release_date. Using "
                  . BROWSER_RELEASE_DATE . " instead.");
        $release_date = BROWSER_RELEASE_DATE;
    }
    $rel_year = substr($release_date, 0, 4);
    $rel_mon  = substr($release_date, 4, 2);
    $rel_day  = substr($release_date, 6, 2);

    #1, 2, 3, Check.
    die "release year in the future" if ($year < $rel_year);
    die "release month in the future"
      if (($year == $rel_year) and ($mon < $rel_mon));
    die "release day in the future"
      if (($year == $rel_year) and ($mon == $rel_mon) and ($mday < $rel_day));

    my @c_time = (0, 0, 0, $rel_day, $rel_mon - 1, $rel_year - 1900, 0, 0, 0);
    my $c_seconds = timelocal(@c_time);

    $c_seconds = $now - (int rand ($now - $c_seconds));
    @c_time = localtime($c_seconds);
    (undef, undef, undef, $c_day, $c_mon, $c_year, undef, undef, undef) = @c_time;
    $c_mon  += 1;
    $c_year += 1900;

    #3, 2, 1, Test.
    die "Compilation year in the future" if ($year < $c_year);
    die "Compilation month in the future"
      if (($year == $c_year) and ($mon < $c_mon));
    die "Compilation day in the future"
      if (($year == $c_year) and ($mon == $c_mon) and ($mday < $c_day));

    return sprintf("%.2i%.2i%.2i", $c_year, $c_mon, $c_day);
}

sub generate_language_settings() {

    our @languages;

    my $language_i      = int rand (@languages);
    my $accept_language = $languages[$language_i];
    $accept_language =~ tr/[A-Z]/[a-z]/;

    return ($languages[$language_i], $accept_language);
}

sub generate_platform_and_os() {

    my %os_data = (
        FreeBSD => {
            karma             => 1,
            platform          => 'X11',
            architectures     => [ 'i386', 'amd64', 'sparc64' ],
            order_is_inversed => 0,
        },
        OpenBSD => {
            karma             => 1,
            platform          => 'X11',
            architectures     => [ 'i386', 'amd64', 'sparc64', 'alpha' ],
            order_is_inversed => 0,
        },
        NetBSD => {
            karma             => 1,
            platform          => 'X11',
            architectures     => [ 'i386', 'amd64', 'sparc64', 'alpha' ],
            order_is_inversed => 0,
        },
        Linux => {
            karma             => 1,
            platform          => 'X11',
            architectures     => [ 'i586', 'i686', 'x86_64' ],
            order_is_inversed => 0,
        },
        SunOS => {
            karma             => 1,
            platform          => 'X11',
            architectures     => [ 'i86pc', 'sun4u' ],
            order_is_inversed => 0,
        },
        'Mac OS X' => {
            karma             => 1,
            platform          => 'Macintosh',
            architectures     => [ 'PPC', 'Intel' ],
            order_is_inversed => 1,
        },
        Windows => {
            karma             => 0,
            platform          => 'Windows',
            architectures     => [ 'NT 5.1' ],
            order_is_inversed => 0,
        }
    );

    my @os_names;

    foreach my $os_name ( keys %os_data ) {
        push @os_names, ($os_name) x $os_data{$os_name}{'karma'}
          if $os_data{$os_name}{'karma'};
    }

    my $os_i   = int rand(@os_names);
    my $os     = $os_names[$os_i];
    my $arch_i = int rand( @{ $os_data{$os}{'architectures'} } );
    my $arch   = $os_data{$os}{'architectures'}[$arch_i];

    my $platform = $os_data{$os}{'platform'};

    my $os_or_cpu;
    $os_or_cpu = sprintf "%s %s",
      $os_data{$os}{'order_is_inversed'} ? ( $arch, $os ) : ( $os, $arch );

    return $platform, $os_or_cpu;
}

sub generate_firefox_user_agent() {

    our $languages;
    our $browser_version;
    our $browser_revision;
    our $browser_release_date;
    our $randomize_release_date;

    my $mozillaversion  = '5.0';

    my $creation_time = $randomize_release_date ?
        generate_creation_time($browser_release_date) : $browser_release_date;
    my ( $locale,   $accept_language ) = generate_language_settings();
    my ( $platform, $os_or_cpu )       = generate_platform_and_os;

    my $firefox_user_agent =
      sprintf "Mozilla/%s (%s; %s; rv:%s) Gecko/%s Firefox/%s",
      $mozillaversion, $platform, $os_or_cpu, $browser_revision,
      $creation_time, $browser_version;

    return $accept_language, $firefox_user_agent;
}

sub log_to_file($) {

    my $message = shift;

    our $logfile;
    our $no_logging;

    my ( $sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst ) =
      localtime time;
    $year += 1900;
    $mon  += 1;
    my $logtime = sprintf "%i/%.2i/%.2i %.2i:%.2i", $year, $mon, $mday, $hour,
      $min;

    return if $no_logging;

    open(my $log_fd, ">>", $logfile) || die "Writing " . $logfile . " failed";
    printf $log_fd UAGEN_VERSION . " ($logtime) $message\n";
    close($log_fd);
}

sub log_error($) {

    my $message = shift;

    $message = "Error: $message";
    log_to_file($message);
    print "$message\n";

    exit(1);
}

sub write_action_file() {

    our $action_file;
    our $user_agent;
    our $accept_language;
    our $no_hide_accept_language;
    our $action_injection;

    my $action_file_content = '';

    if ($action_injection){
        open(my $actionfile_fd, "<", $action_file)
            or log_error "Reading action file $action_file failed!";
        while (<$actionfile_fd>) {
            s@(hide-accept-language\{).*?(\})@$1$accept_language$2@;
            s@(hide-user-agent\{).*?(\})@$1$user_agent$2@;
	    $action_file_content .= $_;
        }
        close($actionfile_fd);
    } else {
	$action_file_content = "{";
	$action_file_content .= sprintf "+hide-accept-language{%s} \\\n",
            $accept_language unless $no_hide_accept_language;
        $action_file_content .= sprintf " +hide-user-agent{%s} \\\n}\n/\n",
            $user_agent;
    }
    open(my $actionfile_fd, ">", $action_file)
      or log_error "Writing action file $action_file failed!";
    print $actionfile_fd $action_file_content;
    close($actionfile_fd);

    return 0;
}

sub write_prefs_file() {

    our $mozilla_prefs_file;
    our $user_agent;
    our $accept_language;
    our $clean_prefs;

    my $prefs_file_content = '';
    my $prefsfile_fd;

    if (open($prefsfile_fd, "<", $mozilla_prefs_file)) {

        while (<$prefsfile_fd>) {
            s@user_pref\(\"general.useragent.override\",.*\);\n?@@;
            s@user_pref\(\"intl.accept_languages\",.*\);\n?@@;
	    $prefs_file_content .= $_;
        }
        close($prefsfile_fd);
    } else {
        log_error "Reading prefs file $mozilla_prefs_file failed. Creating a new file!";
    }

    $prefs_file_content .=
        sprintf("user_pref(\"general.useragent.override\", \"%s\");\n", $user_agent) .
        sprintf("user_pref(\"intl.accept_languages\", \"%s\");\n", $accept_language)
        unless $clean_prefs;

    open($prefsfile_fd, ">", $mozilla_prefs_file)
      or log_error "Writing prefs file $mozilla_prefs_file failed!";
    print $prefsfile_fd $prefs_file_content;
    close($prefsfile_fd);

}

sub VersionMessage() {
    printf UAGEN_VERSION . "\n" . 'Copyright (C) 2006-2011 Fabian Keil <fk@fabiankeil.de> ' .
        "\nhttp://www.fabiankeil.de/sourcecode/uagen/\n";
}

sub help() {

    our $logfile;
    our $action_file;
    our $browser_version;
    our $browser_revision;
    our $browser_release_date;
    our $sleeping_time;
    our $loop;
    our $mozilla_prefs_file;

    my $comma_separated_languages;

    $loop = $loop ? ' ' . $loop : '';
    $mozilla_prefs_file = $mozilla_prefs_file ? ' ' . $mozilla_prefs_file : '';
    foreach (LANGUAGES){
	$comma_separated_languages .= $_ . ",";
    }
    chop $comma_separated_languages;

    VersionMessage;

    print << "    EOF"

Options and their default values if there are any:
    [--action-file $action_file]
    [--action-injection]
    [--browser-release-date $browser_release_date]
    [--browser-revision $browser_revision]
    [--browser-version $browser_version]
    [--clean-prefs-file]
    [--help]
    [--language-overwrite $comma_separated_languages]
    [--logfile $logfile]
    [--loop$loop]
    [--no-action-file]
    [--no-hide-accept-language]
    [--no-logfile]
    [--prefs-file$mozilla_prefs_file]
    [--randomize-release-date]
    [--quiet]
    [--silent]
    [--sleeping-time $sleeping_time]
    [--version]
see "perldoc $0" for more information
    EOF
    ;
    exit(0);
}

sub main() {

    my $error_message;
    my  $no_action_file          = NO_ACTION_FILE;

    our $silent                  = SILENT;
    our $no_logging              = NO_LOGGING;
    our $logfile                 = UAGEN_LOGFILE;
    our $action_file             = ACTION_FILE;
    our $randomize_release_date  = RANDOMIZE_RELEASE_DATE;
    our $browser_version         = BROWSER_VERSION;
    our $browser_revision        = BROWSER_REVISION;
    our $browser_release_date    = BROWSER_RELEASE_DATE;
    our $sleeping_time           = SLEEPING_TIME;
    our $loop                    = LOOP;
    our $no_hide_accept_language = 0;
    our $action_injection        = 0;

    our @languages;
    our ( $accept_language, $user_agent );
    our $mozilla_prefs_file = MOZILLA_PREFS_FILE;
    our $clean_prefs = 0;

    GetOptions('logfile=s' => \$logfile,
               'action-file=s' => \$action_file,
               'language-overwrite=s@' => \@languages,
               'silent|quiet' => \$silent,
               'no-hide-accept-language' => \$no_hide_accept_language,
               'no-logfile' => \$no_logging,
               'no-action-file' => \$no_action_file,
               'randomize-release-date' => \$randomize_release_date,
               'browser-version=s' => \$browser_version,
               'browser-revision=s' => \$browser_revision,
               'browser-release-date=s' => \$browser_release_date,
               'action-injection' => \$action_injection,
               'loop' => \$loop,
               'sleeping-time' => \$sleeping_time,
               'prefs-file=s' => \$mozilla_prefs_file,
               'clean-prefs-file' => \$clean_prefs,
               'help' => \&help,
               'version' => sub {VersionMessage() && exit(0)}
    ) or exit(0);

    if (@languages) {
        @languages = split(/,/,join(',',@languages));
    } else {
	@languages = LANGUAGES;
    }

    srand( time ^ ( $$ + ( $$ << 15 ) ) );

    do {
        $error_message='';
        ( $accept_language, $user_agent ) = generate_firefox_user_agent();

        print "$user_agent\n" unless $silent;

        write_action_file() unless $no_action_file;
        write_prefs_file() if $mozilla_prefs_file;

        log_to_file "Generated User-Agent: $user_agent";

    } while ($loop && sleep($sleeping_time * 60));
}

main();

=head1 NAME

B<uagen> - A Firefox User-Agent generator for Privoxy and Mozilla browsers

=head1 SYNOPSIS

B<uagen> [B<--action-file> I<action_file>] [B<--action-injection>]
[B<--browser-release-date> I<browser_release_date>]
[B<--browser-revision> I<browser_revision>]
[B<--browser-version> I<browser_version>]
[B<--clean-prefs-file>]
[B<--help>] [B<--language-overwrite> I<language(s)>]
[B<--logfile> I<logfile>] [B<--loop>] [B<--no-action-file>] [B<--no-logfile>]
[B<--prefs-file> I<prefs_file>] [B<--randomize-release-date>]
[B<--quiet>] [B<--sleeping-time> I<minutes>] [B<--silent>] [B<--version>]

=head1 DESCRIPTION

B<uagen> generates a fake Firefox User-Agent and writes it into a Privoxy action file
as parameter for Privoxy's B<hide-user-agent> action. Operating system, architecture,
platform, language and, optionally, the build date are randomized.

The generated language is also used as parameter for the
B<hide-accept-language> action which is understood by Privoxy since
version 3.0.5 beta.

Additionally the User-Agent can be written into prefs.js files which are
used by many Mozilla browsers.

=head1 OPTIONS

B<--action-file> I<action_file> Privoxy action file to write the
generated actions into. Default is /etc/privoxy/user-agent.action.

B<--action-injection> Don't generate a new action file from scratch,
but read an old one and just replace the action values. Useful
to keep custom URL patterns. For this to work, the action file
has to be already present. B<uagen> neither checks the syntax
nor cares if all actions are present. Garbage in, garbage out.

B<--browser-release-date> I<browser_release_date> Date to use.
The format is YYYYMMDD. Some sanity checks are done, but you
shouldn't rely on them.

B<--browser-revision> I<browser_revision> Use a custom revision.
B<uagen> will use it without any sanity checks.

B<--browser-version> I<browser_version> Use a custom browser version.
B<uagen> will use it without any sanity checks.

B<--clean-prefs-file> The I<prefs_file> is read and the variables
B<general.useragent.override> and B<intl.accept_languages> are removed.
Only effective if I<prefs_file> is set, and only useful if you want
to use the browser's defaults again.

B<--help> List command line options and exit.

B<--language-overwrite> I<language(s)> Comma separated list of language codes
to overwrite the default values. B<uagen> chooses one of them for the generated
User-Agent, by default the chosen language in lower cases is also used as
B<hide-accept-language> parameter.

B<--logfile> I<logfile> Logfile to save error messages and the generated
User-Agents. Default is /var/log/uagen.log.

B<--loop> Don't exit after the generation of the action file. Sleep for
a while and generate a new one instead. Useful if you don't have cron(8).

B<--no-logfile> Don't log anything.

B<--no-action-file> Don't write the action file.

B<--no-hide-accept-language> Stay compatible with Privoxy 3.0.3
and don't generate the B<hide-accept-language> action line. You should
really update your Privoxy version instead.

B<--prefs-file> I<prefs_file> Use the generated User-Agent to set the
B<general.useragent.override> variable in the Mozilla preference file
I<prefs_file>, The B<intl.accept_languages> variable will be set as well.

Firefox's preference file is usually located in
~/.mozilla/firefox/*.default/prefs.js. Note that Firefox doesn't reread
the file once it is running.

B<--randomize-release-date> Randomly pick a date between the configured
release date and the actual date. Note that Firefox versions after 4.0
no longer provide the build date in the User-Agent header, so if you
randomize the date anyway, it will be obvious that the generated User-Agent
is fake.

B<--quiet> Don't print the generated User-Agent to the console.

B<--sleeping-time> I<minutes> Time to sleep. Only effective if used with B<--loop>.

B<--silent> Don't print the generated User-Agent to the console.

B<--version> Print version and exit.

The second dash is optional, options can be shortened, as long as there are
no ambiguities.

=head1 PRIVOXY CONFIGURATION

In Privoxy's configuration file the line:

    actionsfile user-agent.action

should be added after:

    actionfile default.action

and before:

    actionfile user.action

This way the user can still use custom User-Agents
in I<user.action>. I<user-agent> has to be the name
of the generated action file.

If you are using Privoxy 3.0.6 or earlier, don't add the ".action" extension.

=head1 EXAMPLES

Without any options, B<uagen> creates an action file like:

 {+hide-accept-language{en-ca} \
  +hide-user-agent{Mozilla/5.0 (X11; U; OpenBSD i386; en-CA; rv:1.8.0.4) Gecko/20060628 Firefox/1.5.0.4} \
 }
 /

with the --no-accept-language option the generated file
could look like this one:

 {+hide-user-agent{Mozilla/5.0 (X11; U; FreeBSD i386; de-DE; rv:1.8.0.4) Gecko/20060720 Firefox/1.5.0.4} \
 }
 /

=head1 CAVEATS

If the browser opens an encrypted connection, Privoxy can't inspect
the content and the browser's headers reach the server unmodified.
It is the user's job to use Privoxy's limit-connect action to make sure
there are no encrypted connections to untrusted sites.

Mozilla users can alter the browser's User-Agent with the
B<--prefs-file> option. But note that the preference file is only read
on startup. If the browser is already running, B<uagen's> changes will be ignored.

Hiding the User-Agent is pointless if the browser accepts all
cookies or even is configured for remote maintenance through Flash,
JavaScript, Java or similar security problems.

=head1 BUGS

Some parameters can't be specified at the command line.

=head1 SEE ALSO

privoxy(1)

=head1 AUTHOR

Fabian Keil <fk@fabiankeil.de>

http://www.fabiankeil.de/sourcecode/uagen/

http://www.fabiankeil.de/blog-surrogat/2006/01/26/firefox-user-agent-generator.html (German)

=cut

