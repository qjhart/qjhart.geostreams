package GeoStr::Node::Send;
use base 'GeoStr::Node';

sub grass_make {
  my ($self,$parent)=@_;
  my $parent_fn = $parent->name;
  my $fn = $self->name;
  my ($q)=$self->query;
  my ($w,$s,$e,$n)=@{$q->box};
  my $nsres=($n-$s)/$q->height;
  my $ewres=($e-$w)/$q->width;
  my $rules = <<RULES;
region,$fn,w=$w s=$s e=$e n=$n,ewres=$ewres nsres=$nsres
MASK_from_region,$fn,$fn
query,$fn,$parent_fn
RULES
  $rules;
}
1;
