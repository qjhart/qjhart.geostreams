=pod

=head1 NAME

GeoStr::Query - Query on a GeoStr Server

=head1 SYNOPSIS

  use GeoStr;
  my $q = new GeoStr::Query();
  $q->layer("GOES:1");
  $q->bbox([-400000,-650000,600000,450000]); #E,S,W,N
  $q->srs("epsg:2479");
  $q->resolution(550,500); # Height,Width
  $q->time(); #TBD

  # OR
  my $q = new GeoStr::Query(layer=>"GOES:1",
                            bbox=>[-400000,-650000,600000,450000], #E,S,W,N
                            srs=>"epsg:2479",
                            resolution=>[550,500], # Height,Width
                            time=>[[$date,$time],[$date,$time]]);


=head1 DESCRIPTION

This Class allows for new queries to be inserted and used in the
L<GeoStr::Plan>.

=cut

package GeoStr::Query;
use strict;
use warnings;
use GeoStr::Box;

use Class::MethodMaker
  [ 
   scalar => 'client_id',
   scalar => 'id',
   scalar => 'layer',
   scalar => 'height',
   scalar => 'width',
   scalar => [{-type=>'GeoStr::Box'},qw/ box /],
#   scalar => 'proj',
   scalar => [{-type=>'HASH'}, qw/ proj /],
   new => [ qw/ _new / ],
  ];

=pod

=head1 METHODS

=over 8

=cut

=pod

=item B<new>
Create a new Query

=cut

sub new {
  my $class = shift;
  my @parms=@_;
  my $self = $class->_new();
  while (my $key = shift) { $self->$key(shift) if $self->can($key); }
  $self;
}

=pod

=item B<resolution>,B<height>,B<width>

B<resolution> gets/sets the size of the pixels in the box with
(dx,dy).  Setting the resolution will reset the number in height and
width of the box.  Since height and width need to be integers, setting
the resolution will not always result in the exact resolution being set.

=cut

sub resolution {
  my ($b,$x,$y) = @_;
  if (defined($x)) {
    $y=$x unless defined($y);
    my ($w,$s,$e,$n)=@{$b->box};
    $b->width(ceil(($e-$w)/$x));
    $b->height(ceil(($n-$s)/$y));
  }
  die "Can't determine resolution: width and/or height not set"
    unless ($b->width_isset and $b->height_isset);
  my ($w,$s,$e,$n)=@{$b->box};
  (($e-$w)/$b->width,($n-$s)/$b->height);
}

sub minimum_resolution {
  my $b=shift;
  my ($x,$y) = $b->resolution();
  ($x<$y)?$x:$y;
}

1;

__END__

=back

=head1 AUTHOR

Quinn Hart <qjhart@ucdavis.edu>

=cut
