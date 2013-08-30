use strict;

my %macro;

open (my $fh, "<", $ARGV[0])
	or die("Can not open file");

my $file;

while (my $line = <$fh>) {
	if ($line =~ s/^(w+)://) {
		$macro{$1} = $.;
	}

	if ($line =~ m/^\s*HALT/i) {
		;
	} elsif ($line =~ m/^\s*CLEAR/i) {
		;
	} elsif ($line =~ m/^\s*INC\s+(A|B|C|[A])/i) {
		;
	} elsif ($line =~ m/^\s*JIE\s+(A|B|C|[A])\s+(A|B|C|[A])\s+(\d+|\w+)/i) {
		# if macro is defined
		if ($3 =~ m/\w+/i && $macro{$3} =~ m/\d+/) {
			; # something
		} elsif ($3 =~ m/\d+/) {
			;
		}
	} else {
		print "ERROR\n";
	}
}
