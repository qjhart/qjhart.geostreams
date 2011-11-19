package GeoStr::Box;

use overload '+=' => \&expand,fallback=>1;

sub new {
  my ($class,$in) = @_;
  my $box= [@$in];
  die "Not an ARRAY" unless ref($box) eq 'ARRAY';
  bless $box;
}

sub delta {
  my $box = shift;
  if ($#$box==-1) { 
    (0,0);
  } else {
    ($$box[2]-$$box[0],$$box[3]-$$box[1]);
  }
}

sub expand {
  my ($box,$add)=@_;
  if ($#$box==-1) { 
    @$box=@$add;
  } else {
    foreach (0,1) {
      $$box[$_]=$$add[$_] if ($$add[$_] < $$box[$_]);
    }
    foreach (2,3) {
      $$box[$_]=$$add[$_] if ($$add[$_] > $$box[$_]);
    }
  }
  $box;
}

1;
