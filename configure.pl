#!/usr/bin/perl

use strict;

# init
# ---------
my @configureScript = (
#	{
#		filename     => 'relay_hosts.txt',
#		instructions => "\n\tPlease enter relay hosts one per line.  Press 'return' on a blank line when finished.\n\n",
#		script       => [ 
#					{ prompt => '%n] ',
#					  type   => 'string',
#					  repeat => '' },
#		],
#	},
	{
		filename     => 'users/default',
		instructions => "\n\n\tPlease configure the default user.  Press 'return' for the default value.\n\n",
		script       => [ 
					{ prompt  => 'protocol plugin [%d]: ',
					  type    => 'string',
					  default => 'packet-blasting' },
					{ prompt  => 'encoding plugin [%d]: ',
					  type    => 'string',
					  default => 'blowfish' },
					{ prompt  => 'random seed [%d]: ',
					  type    => 'int',
					  default => '1234567890' },
					{ prompt  => 'random number count [%d]: ',
					  type    => 'int',
					  default => '1000' },
					{ instructions => "\n\tPlease enter relay hosts one per line.  Press 'return' on a blank line when finished.\n\n",
					  prompt       => '%n] ',
					  type         => 'string',
					  repeat       => '' },
		],
	}
);


# main
# ---------
foreach (@configureScript) {
	open FILE, ">$_->{'filename'}" || die "Unable to open filename $_->{'filename'}: $!\n";
	print $_->{'instructions'};

	# parse script
	my($ndx, $inputBuffer, $breakFlag, $repititionFlag) = (1, undef, 0, 0);
	foreach (@{$_->{'script'}}) {
		my $aScript = $_;
		do {
			# prompt
			$repititionFlag = 1 && $ndx = 1 && print $aScript->{'instructions'}
				if defined $aScript->{'instructions'} && !$repititionFlag;
			my $prompt = $aScript->{'prompt'};
			$prompt =~ s,%n,$ndx,g;  
			$prompt =~ s,%d,$aScript->{'default'},g;
			print $prompt;

			# parse input
			chomp($inputBuffer = <STDIN>);
			printf FILE "# %s\n%s\n\n",
				$prompt, ($inputBuffer)
						? $inputBuffer
						: $aScript->{'default'};
			
			# repeat if necessary
			++$ndx;
			$breakFlag = (exists $aScript->{'repeat'})
				? ($inputBuffer ne $aScript->{'repeat'})
					? 1
					: 0
				: 0;
		} while ($breakFlag);
	}
	close FILE;
}
