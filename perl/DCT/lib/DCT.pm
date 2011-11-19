use 5.008003;
use strict;
use warnings;
use List::SkipList;

our $VERSION = '0.00_01';
$VERSION = eval $VERSION;  # see L<perlmodstyle>

package DCT::LastSkipList;
use base qw(List::SkipList);

sub new {
  my $class=shift;
  my %p=@_;
  my $l = $class->SUPER::new(node_class=>$p{node_class});
  $l->{TO_ID}=$p{to_id};
  $l->{ID}=$p{id};
  $l;
}

sub insert {
  $DEBUG::a_insert++;
  my $l = shift;
  my $r = shift;
  $r->{priv}->{$l->{ID}}++;
  my $key =  $l->{TO_ID}->($r);
  $l->SUPER::insert($key,$r,@_);
}

sub delete {
  $DEBUG::a_delete++;
  my $l = shift;
  my $key =  $l->{TO_ID}->(shift());
  $l->SUPER::delete($key,@_);
}

sub find {
  my $l = shift;
  my @keys;
  my $n = $l->list;
  while (defined($n->next)) {
	$n=$n->next;
	push @keys,$n->key;
  }
  @keys;
}

package DCT::LinkedSkipList;
use base qw(List::SkipList);

sub insert {
  my $l = shift;
  my $update_ref = $l->SUPER::insert(@_);
  my $node = $l->{LASTINSRT}->[0];

  $node->prev( $update_ref->[0] );
  my $next = $node->header->[0];
  $next->prev( $node ) if ($next);
  $node;
}

# Need to rewrite to add in the ->prev link.
sub delete {
  my ($self, $key, $finger) = @_;
  use integer;
  my $list = $self->list;
  my ($x, $update_ref, $cmp) = $self->_search_with_finger($key, $finger);
  if ($cmp == 0) {
	my $value = $x->value;
	my $level = $x->level-1; 
	for (my $i=$level; $i>=0; $i--) {
	  my $y   = $update_ref->[$i] || $list;
	  if ($y == $list) { $y = $update_ref->[$level]; }
	  while ((my $fwd=$y->header()->[$i]) != $x) {
		$y = $fwd;
	  }
	  $y->header()->[$i] = $x->header()->[$i]; 
	}
	my $next = $x->header()->[0];
	if ($next) { $next->prev( $x->prev ); }
	$self->{LASTINSRT} = undef;
	$self->{SIZE} --;
	return $value;
  }
}

package DCT::IntervalSkipList;
use fields qw( id cascade cur to_keys min_endp max_endp current_node_max current_node_min NODECLASS);

sub new {
  my $class = shift;
  my %p=@_;
  my $l;
  unless (ref $class) {
    $l = fields::new($class);
  }
  $l->{to_keys}=$p{to_keys};

  $l->{id}=$p{id};
  $l->{cascade}=$p{cascade} if $p{cascade};
  $l->{NODECLASS}=$p{node_class};

  # Minimum List
  $l->{min_endp} = new DCT::LinkedSkipList node_class=>$p{node_class};
  $l->{current_node_max}=$l->{min_endp}->list;
  # Maximum List
  $l->{max_endp} = new DCT::LinkedSkipList node_class=>$p{node_class};
  $l->{current_node_min}=$l->{max_endp}->list;

  $l;
}

sub insert {
  $DEBUG::insert++;
  my $t = shift;
  my $r = shift;
  $r->{priv}->{$t->{id}}++;
  my ($min_key,$max_key)=$t->{to_keys}->($r);
  my $min= $t->{min_endp}->insert($min_key,$r);
  my $max= $t->{max_endp}->insert($max_key,$r);
  # Continue Inserting?
  if ($t->{current_node_max}->key_cmp($min_key) >= 0 and 
	  $t->{current_node_min}->key_cmp($max_key) < 0) {
	$t->{cascade}->insert($r);
  }
}

sub delete {
  $DEBUG::delete++;
  my $t = shift;
  my $r = shift;
  my ($min_key,$max_key)=$t->{to_keys}->($r);
  # CASCADE Deleting 
  if ($t->{current_node_max}->key_cmp($min_key) >= 0 and 
	  $t->{current_node_min}->key_cmp($max_key) < 0) {
	$t->{cascade}->delete($r);
  }
  # Fixup Current Node if necessary
  if ($t->{current_node_max}->key_cmp($min_key)==0) {
	$t->{current_node_max}=$t->{current_node_max}->prev;
  }
  $t->{min_endp}->delete($min_key);

  # Fixup Current Node if necessary
  if ($t->{current_node_min}->key_cmp($max_key)==0) {
	$t->{current_node_min}=$t->{current_node_min}->prev;
  }
  $t->{max_endp}->delete($max_key);

}

sub find {
  my $t= shift;
  my $stab = shift;
  my ($stab_min_key,$stab_max_key) = $t->{to_keys}->($stab);
  # The Stab ID should be undef
  $stab_min_key->[1]=undef;
  $stab_max_key->[1]=0;			#  Backup PAST equal weights
  my ($min_key,$max_key);
  my $cascade=$t->{cascade};
  my ($new,$next);						# For node movement

  # Shrink Max (backup current_node_max in minendp
  for ($new=$t->{current_node_max};$new->key_cmp($stab_max_key) > 0;$new=$new->prev ) {
	(undef,$max_key)=$t->{to_keys}->($new->value);
	$cascade->delete($new->value) if 
	  ($t->{current_node_min}->key_cmp($max_key) < 0 );
  }
  $t->{current_node_max}=$new;

  # Shrink Min (forward current_node_min)
  for($new=$t->{current_node_min},$next=$new->next; 
	  defined($next) and $next->key_cmp($stab_min_key) < 0;
	  $new=$next,$next=$new->next ) {
	($min_key,undef)=$t->{to_keys}->($next->value);
	$cascade->delete($next->value) if
	  ( $t->{current_node_max}->key_cmp($min_key) >= 0);
  }
  $t->{current_node_min}=$new;

  # Grow Max (forward current_node_max)
  for($new=$t->{current_node_max},$next=$new->next; 
	  defined($next) and $next->key_cmp($stab_max_key) < 0;
	  $new=$next,$next=$new->next ) {
	(undef,$max_key)=$t->{to_keys}->($next->value);
	$cascade->insert($next->value) if
	  ( eval "key_cmp $t->{NODECLASS}(\$max_key,\$stab_min_key)" > 0 );
  }
  $t->{current_node_max}=$new;
  # Grow Min (backup current_node_min)
  for ($new=$t->{current_node_min};$new->key_cmp($stab_min_key) > 0;$new=$new->prev ) {
	($min_key,undef)=$t->{to_keys}->($new->value);
	$cascade->insert($new->value) if 
	  (eval "key_cmp $t->{NODECLASS}(\$min_key,\$stab_max_key)" <=0 );
  }
  $t->{current_node_min}=$new;

  # CASCADE Find
  $cascade->find($stab);
}

package DCT::Node;
use base qw( List::SkipList::Node );
use Carp::Assert; # this is required since the parent uses this

use enum qw( PREV=4 );

sub next {
  shift->header()->[0];
}

sub prev {
   my ($self, $prev) = @_;
   return (@_ > 1) ? ( $self->[PREV] = $prev ) : $self->[PREV];
 }

package Region::Node;
use base qw (DCT::Node);

sub validate_key {
  my $self = shift;
  my $key = shift;
  UNIVERSAL::isa($key,"Region");
}

sub key_cmp {
  $DEBUG::key_cmp++;
  my $self = shift;
  my $left= $self->key;
  my $right = shift();
  unless (defined $left) { return -1; }
  ($left <=> $right);
}


package Region::Node2;
use base qw( Region::Node );

sub key_cmp {
  $DEBUG::key_cmp++;
  my $self = shift;
  my $left;
  if ( ref $self ) {
	$left= $self->key;
  } else {
	$left= shift;
  }
  # We should gracefully handle $left being undefined
  unless (defined $left) { return -1; }

  my ($left1,$left2)  = @{ $left };  # node key
  my $right = shift();
  my ($right1, $right2) = @{ $right };    # value to compare the node key with
  if ($left1 == $right1) {
	(defined($right2))?($left2 <=> $right2):-1;
  } else {
	($left1 <=> $right1);
  }
}

package DCT;

sub new {
  my $t = shift;
  my %p=@_;
  
  my $A = new DCT::LastSkipList(id=>'A',
								node_class => 'Region::Node',
								to_id => sub { shift()->{id}; });
  my $Y = new DCT::IntervalSkipList(id=>'Y',
									node_class => 'Region::Node2',
									cascade=>$A,
									to_keys=>
									sub { my $r=shift; 
										  ([$r->{bbox}->[1],$r->{id}],
										   [$r->{bbox}->[3],$r->{id}]);
										},
								   );
  my $X = new DCT::IntervalSkipList(id=>'X',
									node_class => 'Region::Node2',
									cascade=>$Y,
									to_keys=>
									sub { my $r=shift; 
										  ([$r->{bbox}->[0],$r->{id}],
										   [$r->{bbox}->[2],$r->{id}]);
										},
								   );
  $X;
}

1;
__END__

# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

DCT - Perl extension Dynamic Cascade Trees

=head1 SYNOPSIS

  use DCT;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for DCT, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.


=head1 HISTORY

=over 8

=item 0.00_01

Original version; created by h2xs 1.23 with options

  -ABC
	-X
	-f
	-n
	DCT

=back



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Quinn Hart, E<lt>quinn@E<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2004 by Quinn Hart

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.3 or,
at your option, any later version of Perl 5 you may have available.


=cut
