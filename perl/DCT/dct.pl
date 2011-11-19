#! /usr/bin/perl -w

use 5.008003;
use strict qw ( vars );
use warnings;

package main;
use Getopt::Simple;
use IO::File;
use Region;
use DCT;
use Pod::Usage;

my($options) =
  {
   'bbox' =>
   {
	'type'          => '=s',
	'default'       => '0,0,1,1',
	'verbose'       => 'Bounding box',
	'order'         => 1,
   },
   'input' =>
   {
	'type'          => '=s',
	'default'       => '-',
	'verbose'       => 'Filename for BOXES file',
	'order'         => 1,
   },
   'stats' =>
   {
	'type'          => '=s',
	'default'       => undef,
	'verbose'       => 'Filename for STATS file',
	'order'         => 2,
   },
   'output' =>
   {
	'type'          => '=s',
	'default'       => '-',
	'verbose'       => 'Filename for OUTPUT file',
	'order'         => 2,
   },
   'inserts' =>
   {
	'type'          => '=s',
	'default'       => undef,
	'verbose'       => 'Filename for INSERTS file',
	'order'         => 2,
   },
  };

my($option) = new Getopt::Simple;
if (! $option -> getOptions($options, "Usage: dct.pl [options]") )
  {
	exit(-1);       # Failure.
  }

my $dct=new DCT bbox=>new Region(bbox=>[split(/,/,$option->{switch}{bbox})],
								 id=>0);

my %out;
for my $o (qw /output stats inserts /) {
  if ($option->{switch}{$o}) {
	my $oh = new IO::File;
	$oh->open("> $option->{switch}{$o}") or 
	  pod2usage(-message=>"$o file not found",
				-exitval=>2,
				-verbose=>1);
	$out{$o}=$oh;
  }
}


my $bh = new IO::File;
$bh->open("< $option->{switch}{input}") or 
  pod2usage(-message=>'input file not found',
			-exitval=>2,
			-verbose=>1);

# Print headers:
$out{stats}->print("#id key_cmp Ysize Yinsert Ydelete Asize Ainsert Adelete\n") 
  if defined($out{stats});

my @region;
while ($bh->getline) {
  chomp;
  my ($type,$id,@box) = split / /;
  if ($type==1) {				# Insert
	$region[$id]=new Region bbox=>\@box,id=>$id,priv=>{A=>0,X=>0,Y=>0};
	$dct->insert($region[$id]);
  } elsif ($type==2) {			# Query
	my $stab = new Region bbox=>\@box,id=>$id;
	$DEBUG::key_cmp=0;
	$DEBUG::insert=0;
	$DEBUG::delete=0;
	$DEBUG::a_insert=0;
	$DEBUG::a_delete=0;
	my @out = $dct->find($stab);
	my $y_size = $dct->{cascade}->{min_endp}->size;
	my $a_size = $dct->{cascade}->{cascade}->size;
	$out{output}->print("$id ",join(" ",@out),"\n") if defined($out{output});
	$out{stats}->print("$id $DEBUG::key_cmp $y_size $DEBUG::insert $DEBUG::delete $a_size $DEBUG::a_insert $DEBUG::a_delete\n") if defined($out{stats});
  } elsif ($type==0) {			# DELETE
  }
}
$bh->close;

if ($out{inserts}) {
  foreach my $r (@region) {
	$out{inserts}->print("$r->{id} $r->{priv}->{X} $r->{priv}->{Y} $r->{priv}->{A}\n");
  }
}

grep((defined($out{$_}) and $out{$_}->close()),qw / output stats inserts/);

		

__END__

=head1 NAME

dct.pl - Dynamic Cascading Trees

=head1 SYNOPSIS

dct.pl [--boxes file] [--queries file] 

 Options:
  -help            brief help message
  -man             full documentation

=head1 OPTIONS

=over 8

=item B<-help>

Print a brief help message and exits.

=item B<-man>

Prints the manual page and exits.

=back

=head1 DESCRIPTION

B<dct.pl> will exercise a DCT.

=cut
