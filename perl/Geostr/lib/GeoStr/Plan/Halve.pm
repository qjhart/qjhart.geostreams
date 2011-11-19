package GeoStr::Plan::Halve;
use base 'GeoStr::Plan';
use POSIX (qw/ceil floor/);

sub add_average {
  my ($plan,$query,$source) = @_;
  my $g=$plan->graph;
  my $res=$query->minimum_resolution;
  my $lres = ceil(log($res)/log(2));

 COUNT: for (my $n=1; $n <= $lres, $n++) {
    foreach my $potential_factor ($g->successors($source)) {
      if ($potential_factor->isa('GeoStr::Node::Halve')) {
	$potential_factor->query_push($query);
	$source=$potential_factor;
	next COUNT;
      }
    }
    my $halve=new GeoStr::Node::Halve
      (plan=>$plan,
       query=>$query,
       name=>$source->layer .'_'.$n,
      );
    $g->add_edge($source,$halve);
    $source=$halve;
  }
  $source;
}
1;
