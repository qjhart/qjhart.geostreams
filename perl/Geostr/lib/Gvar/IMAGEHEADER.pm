package Gvar::LINETABLE;
use strict qw(vars);

use fields 
  qw(
     index
     );


package Gvar::BLOCKTABLE;
use strict qw(vars);

use fields 
  qw(
     blockpos
     blocksize
     ulreserved
     blocktype
     blocksubtype
     blockcheck
     breserved
     );

package Gvar::IMAGEHEADER;
use strict qw(vars);

#format ID for indexed gvar dataset, does not change
our $IGVAR_FORMATID=0x371D4C62;

use fields 
  qw(
   formatid
   numlines
   headersize
   viswidth
   irwidth
   irrecsize
   viswidthcnt
   irwidthcnt
   irrecsizecnt
   imagedesc
   imageattr
   imageid
   testsum
   validstartline
   validendline
   validstartpixel
   validendpixel
   validabsframe
   line
   block
     );

sub new {
 my Gvar::IMAGEHEADER $h = shift;
  unless (ref $h) {
    $h = fields::new($h);
  }
  my $header = shift;
  my $format='V V V V V V V V V a256 a40 V V V V V V V x32700 x49152 x524288';
  # Get the values
  (
   $h->{formatid},
   $h->{numlines},
   $h->{headersize},
   $h->{viswidth},
   $h->{irwidth},
   $h->{irrecsize},
   $h->{viswidthcnt},
   $h->{irwidthcnt},
   $h->{irrecsizecnt},
   $h->{imagedesc},
   $h->{imageattr},
   $h->{imageid},
   $h->{testsum},
   $h->{validstartline},
   $h->{validendline},
   $h->{validstartpixel},
   $h->{validendpixel},
   $h->{validabsframe},
   $h->{line},
   $h->{block}) 
      = unpack($format,$header);
 return undef unless $h->{formatid} == $IGVAR_FORMATID;
 $h;
}

sub isDoc { shift->{block_id} == 240; }

sub asString {
  my ($h) = @_;
  my $str;
  foreach (qw(imageattr)) {
    $str .= "\t$_=$h->{$_}";
  }
  $str;
}
1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::IMAGEHEADER

=head1 SYNOPSIS

  use Gvar::IMAGEHEADER

=head1 DESCRIPTION

This is something that could be documented.

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
