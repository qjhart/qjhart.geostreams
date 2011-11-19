package GeoStr::Node::Average;
use Scalar::Util qw(blessed);
use base 'GeoStr::Node';
use Class::MethodMaker 
  [
   scalar => 'resolution',
  ];

sub grass_name {
  my $self=shift;
  "avg".$self->resolution."_".$self->{_id};
}

sub grass_make {
  my ($self,$parent)=@_;
  my $parent_fn = ($parent->can("grass_name"))?$parent->grass_name:substr(ref($parent),14);

  my $fn = $self->grass_name;
  my ($w,$s,$e,$n)=@{$self->box};
  my $size=$self->resolution;
  $w-=$size;$s-=$size;$e+=$size,$n+=$size;
  my $queries = join(" ",map('Q'.$_->id,$self->query));
  my $rules = <<RULES;
region,$fn,w=$w s=$s e=$e n=$n
MASK_conflate,$fn,$queries
average,$fn,$size,$parent_fn;
RULES
  $rules;
}

1;
