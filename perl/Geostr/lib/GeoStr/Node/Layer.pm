package GeoStr::Node::Layer;
use base 'GeoStr::Node';
use Class::MethodMaker 
  [
   scalar => 'mapset',
   scalar => 'resolution',
   scalar => 'source',
   scalar => 'eq',
  ];

sub grass_name {
  my $self=shift;
#  $self->name .'@'. $self->mapset;
  $self->name;
  }

sub grass_make {
  my $l=shift; 
  my $name=$l->name;
#  my $cellhd='$(loc)/'.$l->mapset.'/cellhd/'.$l->layer;
#  my $make=<<"MAKE";
#.PHONY: $name
#$name: $cellhd
#MAKE
#  $make;
  "";
}
  1;
