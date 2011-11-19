=pod
=head1 NAME

Gvar::Scan - Single Scan for GVAR data.

=head1 SYNOPSIS

  use Gvar::Scan;

=head1 DESCRIPTION

A Scan is a set of blocks associated with a Documnetation Block

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut

package Gvar::Scan;
use Gvar::Block::Doc;
use PDL;
use strict 'vars';

use fields qw(blocks);

#
# CONSTRUCTOR
#
sub new {
  my Gvar::Scan $scan = shift;
  my %p=@_;
  unless (ref $scan) {
    $scan = fields::new($scan);
  }
  $scan->{blocks}=$p{blocks} if $p{blocks};
  $scan;
}

{
  my @CB = (
	    undef,
	    [ (3..10) ],
	    [ 2 ],
	    [ 2 ],
	    [ 1 ],
	    [ 1 ]
	    );
  sub channel_blocks {
    @{ $CB[shift] };
  }
}  

=pod 
=item block_ids

returns an Array of block ids.  This is just (0..10), for now, but
will change when we start reading the block 11s.

=cut

sub block_ids { (0..10); }

=pod
=item block

returns the requested block.

=cut

sub block { $ { shift->{blocks} }[shift]; }

=pod
=item doc

returns the documentation block.

=cut
sub doc { shift->block(0); }

sub bbox { shift->doc->bbox(@_);}

=pod
=item data

returns a PDL with either the radiance or the Temperature for all the
pertinent data records associated with a GVAR channel.  Hides which
block it comes from.  Call region to get the region within the frame.

=cut

sub data ($$) {
  my ($scan,$ch) = @_;
  my $data;
  my $doc=$scan->doc;

  if ($ch == 1) {
      # Sometimes scans shift #s of pixels I dont' know what to do
      my $pix=eval {cat(map($ { $_->{records} }[0]->{data},@{ $scan->{blocks}}[3..10]))->xchg(0,1);};
      if (defined($pix)) {
	  my $ivcrb=float @{$doc->{IVCRB}};
	  my $ivcr1=float @{$doc->{IVCR1}};
	  my $ivcr2=float @{$doc->{IVCR2}};
	  $data = ($pix*$pix*$ivcr2+$pix*$ivcr1+$ivcrb)->xchg(0,1);
      }
  }
  $data;
}

=pod
=item raw_data

returns a PDL with all the pertinent data records associated with a
GVAR channel.  Hides which block it comes from.  Call region to get
the region within the frame.

=cut

sub raw_data ($$) {
  my $scan = shift; 
  my @ch = @_;
  push @ch,1,2,3,4,5 unless (@ch);

  my @data;
  my @b = @{ $scan->{blocks} };
  foreach my $ch (@ch) {
    my @r;    
    ($ch == 1) and $b[3]->{records} and push @r,map($ { $_->{records} }[0],@b[3..10]);
    ($ch == 2) and $b[2]->{records} and push @r,@{ $b[2]->{records} }[0,1];
    ($ch == 3) and $b[2]->{records} and push @r,@{ $b[2]->{records} }[2];
    ($ch == 4) and $b[1]->{records} and push @r,@{ $b[1]->{records} }[0,1];
    ($ch == 5) and $b[1]->{records} and push @r,@{ $b[1]->{records} }[2,3];
    push @data,(@r)?cat map($_->{data},@r):undef;
  }
  @data;
}

1;
__END__



