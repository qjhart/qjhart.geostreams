package GeoStr::Box;
use Geo::Proj4;
use POSIX qw(floor ceil);

#use overload '+'=>\&add;

use Class::MethodMaker 
  [
   scalar => 'height',
   scalar => 'width',
#   array => [{-default=>1},'resolution'],
   scalar => [{-type=>'ARRAY'},qw/ box /],
   scalar => [{-type=>'HASH'}, qw/ proj /],
   new => [ qw /_new/]
   ];

sub new {
  my $class = shift;
  my $self = $class->_new();
  while (my $key = shift) { $self->$key(shift); }
  $self;
}

sub delta {
  my $box = shift->box;
  ($$box[2]-$$box[0],$$box[3]-$$box[1]);
}


=pod

=item B<height>,B<width>

Gets/Sets the number of pixels for the height and width of the image.

=cut

# These callbacks could set resolution on new changes to height and width
# but it seems to be recursive to me, and I don't know how to change that.

#my $height_cb = sub {
#  my $b=$_[0];
#  my $height=floor($_[1]);
#  my ($x,$y)=$b->resolution;
#  my ($w,$s,$e,$n)=@{$b->box};
#  $b->resolution($x,($n-$s)/$h);
#  $height;
#}
#my $width_cb = sub {
#  my $b=$_[0];
#  my $width=floor($_[1]);
#  my ($x,$y)=$b->resolution;
#  my ($w,$s,$e,$n)=@{$b->box};
#  $b->resolution(($e-$w)/$w,$y);
#  $width;
#}

=pod

=item B<resolution>,B<height>,B<width>

B<resolution> gets/sets the size of the pixels in the box with
(dx,dy).  Setting the resolution will reset the number in height and
width of the box.  Since height and width need to be integers, setting
the resolution will not always result in the exact resolution being set.

=cut

sub resolution {
  my ($b,$x,$y) = @_;
  if (defined($x)) {
    $y=$x unless defined($y);
    my ($w,$s,$e,$n)=@{$b->box};
    $b->width(ceil(($e-$w)/$x));
    $b->height(ceil(($n-$s)/$y));
  }
  die "Can't determine resolution: width and/or height not set"
    unless ($b->width_isset and $b->height_isset);
  my ($w,$s,$e,$n)=@{$b->box};
  (($e-$w)/$b->width,($n-$s)/$b->height);
}

sub minimum_resolution {
  my $b=shift;
  my ($x,$y) = $b->resolution();
  ($x<$y)?$x:$y;
}

sub project {
  my ($b,$proj)=@_;
  $proj ||= {+init=>'epsg:4269'}; # Default projection is longlat

  my ($W,$S,$E,$N);				# New BOX
  my ($w,$s,$e,$n)=@{$b->box};

  my $op= new Geo::Proj4(%{$b->proj});
  my $np= new Geo::Proj4(%$proj);

  my $fixup = sub {
	my ($x,$y)=$np->forward($op->inverse(@_));
	my $fixed=0;
	$W=$x and $fixed++ if $x<$W; 
	$E=$x and $fixed++ if $x>$E; 
	$S=$y and $fixed++ if $y<$S; 
	$N=$y and $fixed++ if $y>$N;
	$fixed;
  };
  # DO corners
  ($W,$S)=$np->forward($op->inverse($w,$s));
  ($E,$N)=($W,$S);
  &$fixup($w,$n);
  &$fixup($e,$s);
  &$fixup($e,$n);

  # I think this could really fail on some weird shapes.
  {
    # by default be very fast with resolution;
    my $max_iterations = eval { int(log($b->minimum_resolution)/log(2))+1 };
    if ($@) { $max_iterations = 2; }
    our $fixside = sub {
      my $side=shift;
      my $it=shift;				# Iterations
      return if $it++ > $max_iterations;
      my ($w,$s,$e,$n)=@_;
      my $t;
      if ($side==0 or $side==2) {
	$x=($side==0)?$w:$e;
	$y=($n+$s)/2;
	if (&$fixup($x,$y)) {
	  &$fixside($side,$it,$w,$s,$e,$y);
	  &$fixside($side,$it,$w,$y,$e,$n);
	}
      } elsif ($side==1 or $side==3) {
	my $y=($side==1)?$s:$n;
	$x=($w+$e)/2;
	if (&$fixup($x,$y)) {
	  &$fixside($side,$it,$x,$s,$e,$n);
	  &$fixside($side,$it,$w,$s,$x,$n);
	}
      }
    };
  }
  &$fixside(0,0,$w,$s,$e,$n);
  &$fixside(1,0,$w,$s,$e,$n);
  &$fixside(2,0,$w,$s,$e,$n);
  &$fixside(3,0,$w,$s,$e,$n);

  [$W,$S,$E,$N];
}

1;
