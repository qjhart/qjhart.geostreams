package GeoStr::Node::MapCalc;
use Scalar::Util qw(blessed);
use base 'GeoStr::Node';
use Class::MethodMaker 
  [
   scalar => 'eq',
   scalar => 'resolution_number',
   array => 'source',
  ];

sub grass_make {
  my ($self,@parent)=@_;
  my ($w,$s,$e,$n)=@{$self->box};
  my $fn=$self->grass_name;
  my $eq=$self->eq;
  # I don't think ->source works at all
  #  my $parents=join(" ",map($_->grass_name,$self->source));
  my @parent_names=map($_->name,grep(!($_->isa('GeoStr::Node::Layer') && $_->eq),@parent));
  my $rast=$parent_names[0];
  my $parents=join(" ",@parent_names);

  <<RULES;
region,$fn,rast=$rast w=$w s=$s e=$e n=$n
MASK_from_region,$fn,$fn
mapcalc,$fn,$parents,$eq
RULES
}

1;
__END__

$fn,$parents,w=$w s=$s e=$e n=$n,NOMASK,mapcalc,$eq

