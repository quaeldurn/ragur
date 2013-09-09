#!/usr/bin/perl

# Analyze a cmi file and return ragur readable code

use strict;
use warnings;

our $is_stdin = 0;
our $fh;
our $errors = 0;
our %macro;
our @file;

# if there is an argument to the program
if (defined $ARGV[0]) {
	open ($fh, "<", $ARGV[0])
		or die("Can not open file");
} else {
	print "Reading from STDIN\n";
	$fh = *STDIN;
	$is_stdin = 1;
}

# load $fh to @file
while (<$fh>) {
	push(@file, $_);
}

# first read, save all macro definitions and remove them
for (my $i = 0; $i <= $#file; $i++) {
	if ($file[$i] =~ m/^(\w+):\s*/) {
		$macro{$1} = $i;
		$file[$i] =~ s/^\w+:\s*//;
	} else {
		$file[$i] =~ s/^\s*//;
	}
	
	if ($file[$i] =~ m/^;/) {
		splice(@file, $i, 1);
	}
}

# second read, checking if operations are correctly written and replacing
# macros the respective number.
for (my $i = 0; $i <= $#file; $i++) {
	if ($file[$i] =~ m/^HALT/i) {
		;
	} elsif ($file[$i] =~ m/^\s*CLEAR/i) {
		;
	} elsif ($file[$i] =~ m/^\s*INC\s+(A|B|C|[A])/i) {
		;
	} elsif ($file[$i] =~ m/^\s*JIE\s+(A|B|C|[A])\s+(A|B|C|[A])\s+(\d+|\w+)/i) {
		# if macro is defined
		my $word = $3;
		if ($word =~ m/\w+/ && defined $macro{$word}) {
			if ($macro{$word} =~ m/\d+/) {
				$file[$i] =~ s/\w+$/$macro{$word}/;
			}
		} elsif ($word =~ m/\d+/) {
			;
		} else {
			print "error on line $., maybe the $word macro hasn't been defined?\n";
			$errors++;
		}
	} else {
		print "error in line $.\n";
		$errors++;
	}
}

if ($is_stdin == 1) {
	;
} else {
	close($fh);
}

if ($errors == 0) {
	print @file;
} else {
	print "$errors error/s found in file\n";
}
