package GeoStr::Node;
use strict;
use warnings;

use GeoStr::Box;

our $Id;

use Class::MethodMaker 
  [
   array=>'query',
   scalar => [{-type=>'GeoStr::Plan'},qw/plan/],
   scalar => 'name',
   new => [ qw /_new/]
   ];

sub new {
  my $class = shift;
  my $self = $class->_new();
  $self->{_id}=$Id++;
  my ($classname)=reverse split '::', ref($self);
  $self->name(lc substr($classname,0,3) . $self->{_id}); # Overwrite
  while (my $key = shift) { 
    $self->$key(shift) if $self->can($key); 
  }
  $self;
}

sub id { shift->{_id}; }
#sub resolution { 9999 };

sub grass_name { shift->name; }

sub box {
  my $self=shift;
  my $box=new GeoStr::Box($self->query_index(0)->box);
  grep($box->expand($_->box),$self->query);
  $box;
}

1;


__END__

=back

=head1 AUTHOR

Quinn Hart <qjhart@ucdavis.edu>

=cut
