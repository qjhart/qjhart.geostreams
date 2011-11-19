package GeoStr::Node::Interpolate;
use base 'GeoStr::Node';
use Class::MethodMaker 
  [
   scalar => 'width',
   scalar => 'height',
  ];

sub grass_make {
  my ($self,$parent)=@_;
  my ($q)=$self->query;
  my $parent_fn = $parent->grass_name;
  my $fn = $self->grass_name;
  my ($w,$s,$e,$n)=@{$q->box};
  my $nsres=($n-$s)/$q->height;
  my $ewres=($e-$w)/$q->width;
  my $rules = <<RULES;
region,$fn,w=$w s=$s e=$e n=$n ewres=$ewres nsres=$nsres
MASK_from_region,$fn,$fn
interpolate,$fn,$parent_fn
RULES
  $rules;
}

1;
