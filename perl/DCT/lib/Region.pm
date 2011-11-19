#
# Simple Regions
#
package Region;
use strict qw(vars);
use fields qw( bbox id priv);
use Carp::Assert; # this is required since the parent uses this

sub new {
  my $r = shift;
  my %p=@_;
  unless (ref $r) {
    $r = fields::new($r);
  }
  grep ($r->{$_}=$p{$_},keys %p);
  $r;
}

sub area {
  my $r = shift;
  my ($w,$s,$e,$n)=@{$r->{bbox}};
  ($e-$w)*($n-$s);
}

sub asString {
  my $self=shift;
#  ((defined $self->{id})?$self->{id}.":":"")."[".join(",",@{$self->{bbox}})."]";
  join(" ",$self->{id},@{$self->{bbox}});
}

1;
