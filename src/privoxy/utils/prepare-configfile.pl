#!/usr/local/bin/perl

# This script is used by the config-file target in GNUMakefile.
#
# It removes garbage in the w3m output and separates comments
# and active directives.

use strict;
use warnings;

sub main() {
    my $hit_header = 0;
    my $hit_option = 0;
    my $header_len;
    my $unfold_mode = 0;
    my $unfolding_enabled = 0;
    my $windows_section_reached = 0;

    while (<>) {

        if (!$unfolding_enabled and m/=========/) {
            # We passed the table of contents
            # and can try to unfold unintentional
            # line breaks;
            $unfolding_enabled = 1;
        }
        if (m/specific to the Windows GUI/) {
            # The Windows section is formatted differently.
            $windows_section_reached = 1;
        }

        s/^1\. \@\@TITLE\@\@/     /i;

        if ($hit_header) {
            $header_len += length($_);
            $_ = " " . $_;
        } elsif (m/^(\d*\.){1,3}\s/) {
            # Remove the first digit as it's the
            # config file section in the User Manual.
            s/^(\d\.)//;

            # If it's a section header, uppercase it.
            $_ = uc() if (/^\d\.\s+/);

            # Remember to underline it.
            $hit_header = 1;
            $header_len = length($_);
        }

        if ($unfold_mode) {
            s/^\s+/ /;
            $unfold_mode = 0;
        } else {
            if ( $hit_option ) {
               # processing a continuation of a @@ line
               if ( /^\s*$/ ) { # blank line
                  $hit_option = 0;
                  next;
               }
            }
            s/^/#  /;
        }
        if ($unfolding_enabled and
            (m/(\s+#)\s*$/ or m/forward-socks5 and$/)) {
            $unfold_mode = 1;
            chomp;
        }

        # XXX: someone should figure out what this stuff
        # is supposed to do (and if it really does that).
        s/^#  #/####/ if /^#  #{12,}/;
        s/^\n//;
        s/^#\s*-{20,}//;
        s/ *$//;
        $hit_option = 1 if s/^#\s+@@//;

        if ($windows_section_reached) {
            # Do not drop empty lines in the Windows section
            s/^\s*$/#\n/;
        }

        print unless (/^\s*$/);

        if ($hit_header and !$unfold_mode) {
            # The previous line was a section
            # header so we better underline it.
            die "Invalid header length" unless defined $header_len;
            print "#  " . "=" x $header_len . "\n";
            $hit_header = 0;
            $header_len = 0;
        };
    }
}
main();
