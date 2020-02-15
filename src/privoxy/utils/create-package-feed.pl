#!/usr/local/bin/perl

############################################################################
# create-package-feed.pl
#
# Generates an RSS feed for the released files.
#
# Usage:
# create-package-feed.pl "path/to/release/files" "path/to/rss/outputfile"
#
# Copyright (c) 2016 "Unknown", Fabian Keil <fk@fabiankeil.de>
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
############################################################################

use warnings;
use strict;
use Digest::SHA;
my @months = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
my @days   = qw(Sun Mon Tue Wed Thu Fri Sat Sun);

my $base_dlurl = 'https://www.privoxy.org/sf-download-mirror/';
my $max_advertised_files = 100;

sub generate_rss_item($$$$) {
    my ($target, $target_uri, $target_time, $target_sha256) = @_;

    my $rss_item;
    my $escaped_target_uri = $target_uri;
    $escaped_target_uri =~ s@ @%20@g;

    # RSS line
    $rss_item =
        '<item><title><![CDATA[' . $target_uri . ']]></title>';
    $rss_item .=
        '<description><![CDATA['
        . $target_uri
        . ' (SHA-256: '
        . $target_sha256
        . ')]]></description>';
    $rss_item .=
        '<link>'
        . $base_dlurl
        . $escaped_target_uri
        . '</link><guid>'
        . $base_dlurl
        . $escaped_target_uri
        . '</guid>';
    $rss_item .= '<pubDate>';
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) =
        gmtime($target_time);
    $rss_item .= sprintf("%s, %s %s %d %.2d:%.2d:%.2d GMT",
                         $days[$wday], $mday, $months[$mon], ($year + 1900),
                         $hour, $min, $sec);
    $rss_item .= '</pubDate></item>';
    $rss_item .= "\n";

    return $rss_item;
}

sub get_sha256_sum($) {
    my $file = shift;

    open(my $fd, "<", $file)
        or die "Can't open '$file' to generate checksum $!";
    my $sha256 = Digest::SHA->new("SHA-256");
    $sha256->addfile($fd);
    close($fd);

    return $sha256->hexdigest;
}

sub get_released_files($) {
    my $scan_dir = shift;

    my @Array = ();
    my $i     = 0;
    my $target;
    my $target_sha256;
    my $target_uri;
    my $target_time;
    my $target_line;

    opendir(my $D1, $scan_dir) or die "Can't open 1st directory! /";
    while (my $fi1 = readdir($D1)) {
        next if ($fi1 =~ m/^\./);
        next if ($fi1 eq 'OldFiles' or $fi1 eq 'pkgsrc');

        opendir(my $D2, $scan_dir . $fi1 . '/')
            or die "Can't open 2nd directory! /$fi1";
        while (my $fi2 = readdir($D2)) {
            next if ($fi2 =~ m/^\./);

            # Start listing /OS/Version/FILE
            opendir(my $D3, $scan_dir . $fi1 . '/' . $fi2 . '/')
                or die "Can't open 3rd directory! /$fi1/$fi2";
            while (my $fi3 = readdir($D3)) {
                next if ($fi3 =~ m/^\./);
                $target = $scan_dir . $fi1 . '/' . $fi2 . '/' . $fi3;
                next if (!-e $target);    # skip if file is not exist

                $target_uri  = $fi1 . '/' . $fi2 . '/' . $fi3;
                $target_time = (stat $target)[9];

                $Array[$i] = ([$target_time, $target, $target_uri]);

                $i++;
            }
            closedir($D3);
        }
        closedir($D2);
    }
    closedir($D1);

    return sort { @$a[0] <=> @$b[0] } @Array;
}

sub generate_feed($) {
    my $scan_dir = shift;

    # Result = Full XML Codes
    my $result = '<?xml version="1.0" encoding="utf-8"?>
 <rss xmlns:content="http://purl.org/rss/1.0/modules/content/" version="2.0">
  <channel>
   <title>Privoxy Releases</title>
   <link>https://www.privoxy.org/announce.txt</link>
   <description><![CDATA[Privoxy Releases RSS feed]]></description>
   <pubDate>';
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = gmtime();
    $result .=
        "$days[$wday], $mday $months[$mon] "
        . ($year + 1900)
        . " $hour:$min:$sec GMT";
    $result .= '</pubDate>';
    $result .= "\n";

    my @resArray = get_released_files($scan_dir);
    my $i = @resArray - 1;
    while ($max_advertised_files-- > 0 && $i >= 0) {
        my $target_time =  $resArray[$i][0];
        my $target = $resArray[$i][1];
        my $target_uri =  $resArray[$i][2];

        my $target_sha256 = get_sha256_sum($target);

        my $rss_item = generate_rss_item($target, $target_uri, $target_time, $target_sha256);

        $result .= $rss_item;
        $i--;
    }
    $result .= '  </channel>
   </rss>';

    return $result;
}

sub main() {
    my $scan_dir = shift(@ARGV)
        or die "Local package directory not specified (first argument)\n";
    my $save_rss_file = shift(@ARGV)
        or die "RSS output file path not specified (second argument)\n";

    my $result = generate_feed($scan_dir);

    open(my $XMLF, ">", $save_rss_file) or die "Failed to write XML file";
    print $XMLF $result;
    close($XMLF);
}

main();
