package GeoStr::Plan::Merge;
use base 'GeoStr::Plan';
use POSIX (qw/ceil floor/);

sub add_average {
  my ($plan,$query,$source) = @_;

  my $ires=$query->minimum_resolution>>1;

  my $last=$source;
  # If the resolution is > 25, then we halve the image.
  for ($ires=$query->minimum_resolution>>1;$ires > 12;$ires>>=1) {
    my @existing_nodes=$last->getAllChildren;
  FIND: {
      foreach my $n (@existing_nodes) {
	if ($n->isa('GeoStr::Node::Halve')) {
	  $n->getNodeValue->query_push($query);
	  $last=$n;
	  last FIND;
	}
      }
      my $halve=new GeoStr::Node::Halve(plan=>$plan,query=>$query);
      $plan->graph->add_edge($last,$halve);
      $last=$halve;
    }
  }

  $ires = ($ires<<1)+1;
 FIND: {
    foreach my $n ($last->getAllChildren) {
      if ($n->getNodeValue->isa('GeoStr::Node::Average') and
	  $n->getNodeValue->resolution == $ires) {
	$n->getNodeValue->query_push($query);
	$last=$n;
	last FIND;
      }
    }
    my $avg = new GeoStr::Node::Average
      (
       plan=>$plan,
       resolution=>$ires,
       query=>$query);
    $plan->graph->add_edge($last,$avg);
    $last=$avg;
  }
  $last;
}
1;


