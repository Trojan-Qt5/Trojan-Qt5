#!/usr/bin/perl

# utils/filter2docs.pl

# Parse the filter names and descriptions from a filter file and
# spit out copy&paste-ready markup for the various places in
# configuration and documentation where all filters are listed.

use strict;
use warnings;

my (%comment_lines, %action_lines, %sgml_source_1, %sgml_source_2);

sub main() {

    die "Usage: $0 filter-file\n" unless (@ARGV == 1) ;
    open(INPUT, "< $ARGV[0]") or die "Couldn't open input file $ARGV[0]: $!\n";

    parse_file();
    print_markup();
}

sub sgml_escape($) {
    my $text = shift;

    $text =~ s@<@&lt;@g;
    $text =~ s@>@&gt;@g;

    return $text;
}

sub parse_file() {
    while (<INPUT>) {
        if (/^((?:(?:SERVER|CLIENT)-HEADER-)?(?:FILTER|TAGGER)): ([-\w]+) (.*)$/) {
            my $type_uc = $1;
            my $name = $2;
            my $description = $3;
            my $type = lc($type_uc);
            my $sgml_description = sgml_escape($description);
            my $white_space = ' ' x (($type eq 'filter' ? 20 : 27) - length($name));

            $comment_lines{$type} .= "#     $name:" . $white_space . "$description\n";
            $action_lines{$type}  .= "+$type" . "{$name} \\\n";
            $sgml_source_1{$type} .= "   <para>\n    <anchor id=\"$type-$name\">\n" .
                "    <screen>+$type" . "{$name}" . $white_space .
                "# $sgml_description</screen>\n   </para>\n";
            $sgml_source_2{$type} .= ' -<link linkend="' . $type_uc . "-" .
                uc($name) . "\">$type" . "{$name}</link> \\\n";
        }
    }
}

sub print_markup() {

    my @filter_types = (
        'filter',
        'server-header-filter',
        'client-header-filter',
        'server-header-tagger',
        'client-header-tagger'
    );

    foreach my $type (@filter_types) {

        next unless defined $action_lines{$type};

        print "=" x 90;
        
        print <<"        DOCMARKUP";

Producing $type markup:

Comment lines for default.action.master:

$comment_lines{$type}
Block of $type actions for default.action.master:

$action_lines{$type}
SGML Source for AF chapter in U-M:

$sgml_source_1{$type}
SGML Source for AF Tutorial chapter in U-M:

$sgml_source_2{$type}
        DOCMARKUP
    }
}

main();
