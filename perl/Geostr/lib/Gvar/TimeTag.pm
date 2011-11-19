package Gvar::TimeTag;

use strict 'vars';
use fields qw(BCD Y j H M S ms date);
use Date::Manip;
#Date_Init("TZ=GMT");

sub new {
  my Gvar::TimeTag $t = shift;
  unless (ref $t) {
    $t = fields::new($t);
  }
  if (my $bcd=shift) {
    $t->{BCD}=$bcd;
    my @b = unpack('c8',$bcd);
    return undef if ($b[0] == -1 or $b[0]==0);
    $t->{Y}=(($b[0]>>4)&0xf)*1000+
	      ($b[0]&0xf)*100+
	      (($b[1]>>4)&0xf)*10+
	      ($b[1]&0xf);
    $t->{j}=(($b[2]>>4)&0xf)*100+($b[2]&0xf)*10+(($b[3]>>4)&0xf);
    $t->{H}=($b[3]&0xf)*10+(($b[4]>>4)&0xf);
    $t->{M}=($b[4]&0xf)*10+(($b[5]>>4)&0xf);
    $t->{S}=(($b[5]&0xf)*10+(($b[6]>>4)&0xf));
    $t->{ms}=(($b[6]&0xf)*100+(($b[7]>>4)&0xf)*10+($b[7]&0xf));
    my $date=ParseDate(sprintf "%.3d-%.3d",$t->{Y},$t->{j});
    my $delta=ParseDateDelta(sprintf "+0:0:0:%d:%d:%d",$t->{H},$t->{M},$t->{S});
    $t->{date}=sprintf "%s UT",DateCalc($date,$delta);
  }
  $t;
}

sub asString {
    shift->{date};
}

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::TimeTag - Reads the Gvar Time format

=head1 SYNOPSIS

  use Gvar::TimeTag;

=head1 DESCRIPTION
Internal Object

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
