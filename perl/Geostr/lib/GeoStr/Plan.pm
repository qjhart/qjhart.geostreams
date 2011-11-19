package GeoStr::Plan;
use strict;
use warnings;
use Graph::Directed;
use Graph::Traversal::DFS;
use GeoStr;
use GeoStr::GeoBox;
use GeoStr::Node::Layer;
use GeoStr::Node::Send;
use GeoStr::Node::Average;
use GeoStr::Node::Interpolate;
use GeoStr::Node::Project;
use GeoStr::Node::Halve;
use GeoStr::Node::MapCalc;
use POSIX (qw/ceil floor/);

use Class::MethodMaker
  [
   scalar => [ { -type => 'Graph::Directed'},qw/ graph /],
   scalar => qw / name /,
   scalar => qw / averaging /,
   new => [ qw/ _new / ],
  ];

sub layer {
  my ($plan,$name)=@_;
  foreach my $child ($plan->graph->predecessorless_vertices()) {
    return $child if ($child->isa('GeoStr::Node::Layer') and ($child->name eq $name));
  }
}

sub new {
  my ($class,%parms) = @_;
  my $self = $class->_new();
  foreach my $key (keys %parms) {
    $self->$key($parms{$key}) if $self->can($key); 
  }
  $self->graph(new Graph::Directed(vertices=>$parms{layers}));
  $self;
}

sub add_query {
  my ($plan,$query) = @_;
  
  my $layer = $plan->layer($query->layer) || 
    die "Bad Source: ".$query->layer;

  my $interpolate;

  if ($query->proj) {
    $interpolate = new GeoStr::Node::Project 
      (
       query=>$query,
       name=>$plan->name.'_'.$query->id,
      );
    # Now fix the bounding box to Be in GOES space
    my $new_box= new GeoStr::GeoBox(box=>$query->box,
				    proj=>$query->proj,
				   height=>$query->height,
				   width=>$query->width);

    my @new_box= @{$new_box->project({+proj=>'goes',+goes=>10,+row_col=>'-1'})};
    my @res=@{ $query->resolution };
    $new_box[0]-=2*$res[0];
    $new_box[1]-=2*$res[1];
    $new_box[2]+=2*$res[0];
    $new_box[3]+=2*$res[1];

    $query->box(\@new_box);

    # These are used to pick the resolution.  This currently just
    # picks whatever rsolution corresponds to a similar size in the LL
    # image.

    $query->width($query->width+4);
    $query->height($query->height+4);

  } else {
    $interpolate = new GeoStr::Node::Interpolate 
      (
       query=>$query,
       name=>$plan->name.'_'.$query->id.'_'.$layer->name.'_int',
      );
  }

  if ($layer->eq) { 		# Equation
    my $eq = $plan->mapcalc($query,$layer);
    $plan->graph->add_edge($eq,$interpolate);
  } else {
    my $average_node = $plan->average($query,$layer);
    $plan->graph->add_edge($average_node,$interpolate);
  }

  $plan->graph->add_edge
    (
     $interpolate,
     GeoStr::Node::Send->new
     (
      query=>$query,
      name=>$plan->name.'_'.$query->id,
     )
    );
  $plan;
}


sub average_each {
 my ($plan,$query,$source) = @_;
 my $res=$query->minimum_resolution;
 my $src_res=$source->resolution;
 my $lres = floor((log($res)-log($src_res))/log(2));
 my $name=$plan->name.'_'.$query->id.'_'.$source->name;
 
COUNT: for (my $n=1; $n <= $lres; $n++) {
   my $halve=new GeoStr::Node::Halve(plan=>$plan,
				     query=>$query,
				     name=>$name . '_'. $n
				    );
   $plan->graph->add_edge($source,$halve);
   $source=$halve;
 }
 $source;
}

sub average_share {
  my ($plan,$query,$source) = @_;
  my $g=$plan->graph;
  my $res=$query->minimum_resolution;
  my $src_res=$source->resolution;
  my $lres = floor((log($res)-log($src_res))/log(2));
  my $name=$plan->name.'_'.$source->name;

 COUNT: for (my $n=1; $n <= $lres; $n++) {
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
       name=>$name .'_'.$n,
      );
    $g->add_edge($source,$halve);
    $source=$halve;
  }
  $source;
 }

sub average_neighbors {
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

sub average {
  my $self=shift;
  my %Average =
    (
     each=>*average_each,
     share=>*average_share,
     neighbors=>*average_neighbors,
    );
  &{$Average{$self->{averaging}}}($self,@_);
}


sub mapcalc_each {
  my ($plan,$query,$source)=@_;
  my $g=$plan->graph;
  my $res=$query->minimum_resolution;
  my $src_res=$source->resolution;
  my $lres = floor((log($res)-log($src_res))/log(2));
  my $eq = new GeoStr::Node::MapCalc
    (
     query=>$query,
     eq=>$source->eq,
     name=>$plan->name.'_'.$query->id.'_'.$source->name.'_'.$lres,
    );
  
  my %avg;
  foreach (@{$source->source}) {
    $avg{$_} = $plan->average($query,$plan->layer($_));
    $g->add_edge($avg{$_},$eq);
  }
  $eq;
}

sub mapcalc_share {
  my ($plan,$query,$source) = @_;
  my $g=$plan->graph;
  my $res=$query->minimum_resolution;
  my $src_res=$source->resolution;
  my $lres = floor((log($res)-log($src_res))/log(2));
  my $eq;
  # DO We Have a lres, version?
 HAVE: {
    foreach my $potential_factor ($g->successors($source)) {
      if ($potential_factor->resolution_number==$lres) {
	$potential_factor->query_push($query);
	$eq=$potential_factor;
	last HAVE;
      }
    }
    $eq = new GeoStr::Node::MapCalc
      (
       query=>$query,
       eq=>$source->eq,
       resolution_number=>$lres,
       name=>$plan->name.'_'.$source->name.'_'.$lres,
      );
    $g->add_edge($source,$eq);
  }

  my %avg;
  foreach (@{$source->source}) {
    $avg{$_} = $plan->average($query,$plan->layer($_));
    $g->add_edge($avg{$_},$eq);
  }
  $eq;
}


sub mapcalc {
  my $self=shift;
  my %mc =
    (
     each=>*mapcalc_each,
     share=>*mapcalc_share,
    );
  &{$mc{$self->{averaging}}}($self,@_);
}

sub execute {
  my ($plan,%args) = @_;
  my $g=$plan->graph;
  my $t=Graph::Traversal::DFS->new
    ($g,
     first_root => sub {
       my ($t,$unseen)=@_;
       foreach my $child ($plan->graph->predecessorless_vertices()) {
	 next if $child->eq;	# Skip Computed layers
	 if ($unseen->{$child}) {
	   $t->set_state('parent',$child);
	   return $child;
	 }
       }
       return ();
     },
     next_root => sub {
       my ($t,$unseen)=@_;
       foreach my $child ($plan->graph->predecessorless_vertices()) {
	 next if $child->eq;	# Skip Computed layers
	 if ($unseen->{$child}) {
	   $t->set_state('parent',$child);
	   return $child;
	   }
       }
       return ();
     },
     pre => sub{
       my ($node,$t)=@_;
       my @parents = map($_->[0],$g->edges_to($node));
#       print STDERR "NODE:$node.\n";
       my $make = $node->grass_make(@parents);
       chomp $make;
#       print STDERR $make."\n";
       $t->set_state('plan',join("\n",
		      $t->get_state('plan'),$make));
       1;
     },
    );
  $t->set_state('plan','# Plan execute');
  $t->dfs;
  $t->get_state('plan');
}

1;
