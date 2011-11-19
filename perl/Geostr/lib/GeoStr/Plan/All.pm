package GeoStr::Plan::All;
use base 'GeoStr::Plan';
use POSIX (qw/ceil floor/);

sub add_average {
  my ($plan,$query,$source) = @_;

  my $res=$query->minimum_resolution;
  my $lres = ceil(log($res)/log(2));

 COUNT: for (my $n=1; $n <= $lres, $n++) {
    my $halve=new GeoStr::Node::Halve
      (plan=>$plan,
       query=>$query,
       name=>$query->id . '_' . $n
      );
    $plan->graph->add_edge($source,$halve);
    $source=$halve;
  }
  $source;
}

sub add_average_old {
  my ($plan,$query,$source) = @_;
  
  my $ires;
  my $last=$source;
  # If the resolution is > 25, then we halve the image.
  for ($ires=$query->minimum_resolution>>1;$ires > 12;$ires>>=1) {
    my $halve=new GeoStr::Node::Halve(plan=>$plan, query=>$query);
    $plan->graph->add_edge($last,$halve);
    $last=$halve;
  }

  $ires = ($ires<<1)+1;
  my $avg = new GeoStr::Node::Average(
				      plan=>$plan,
				      resolution=>$ires,
				      query=>$query);

  $plan->graph->add_edge($last,$avg);
  $avg;
}

1;

__END__


