package GeoStr::Node::Halve;
use Scalar::Util qw(blessed);
use base 'GeoStr::Node';

sub grass_make {
  my ($self,$parent)=@_;
  my ($w,$s,$e,$n)=@{$self->box};
  my $fn=$self->grass_name;
  my $parent_fn=$parent->grass_name;
  <<RULES;
region,$fn,w=$w s=$s e=$e n=$n
MASK_from_region,$fn,$fn
halve,$fn,$parent_fn
RULES
}

1;
