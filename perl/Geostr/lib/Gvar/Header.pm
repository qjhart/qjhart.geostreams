package Gvar::Header;
use strict qw(vars);

use fields 
  qw(
     pos
     block_id
     word_size
     word_count
     product_identification
     repeat_flag
     version_number
     data_valid_flag
     ascii_binary_flag
     range_word
     block_count
     sps_time
     error_check
    );

# 30 8bit words sent three times.
sub new {
  my Gvar::Header $h = shift;
  unless (ref $h) {
    $h = fields::new($h);
  }
  my $header = shift;
  my $format='C C n n C C C C x C n x2 a8 x3 n';
  # Get the values
  ( $h->{block_id},
    $h->{word_size},
    $h->{word_count},
    $h->{product_identification},
    $h->{repeat_flag},
    $h->{version_number},
    $h->{data_valid_flag},
    $h->{ascii_binary_flag},
    $h->{range_word},
    $h->{block_count},
    $h->{sps_time},
    $h->{error_check}) = unpack($format,$header);
  $h;
}

sub isDoc { shift->{block_id} == 240; }
sub block_type {
  my $bid = shift->{block_id};
  {
    ($bid == 240) and return 'Doc';
    (($bid == 1) or ($bid == 2)) and return 'IR';
    (($bid >= 3) and ($bid <= 10)) and return 'Vis';
    ($bid == 11) and return 'Auxil';
    ($bid == 15) and return 'EIB';
  }
  undef;
}

# Returns size in bytes;
sub data_bytes {
   my $self = shift;
   my $wc= $self->{word_count} - 2 ;
   my $ws= $self->{word_size};
   # words
   my $blk_size = int((($wc-2)*$ws/16))+1+((($wc-2)*$ws)%16?1:0);
   # bytes
   $blk_size<<1;
}

sub asString {
  my ($h) = @_;
  my $str;
  foreach (qw(block_id word_size word_count product_identification repeat_flag data_valid_flag block_count )) {
    $str .= "\t$_=$h->{$_}";
  }
  $str;
}
1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::Header - The Header part of a GVAR::Block

=head1 SYNOPSIS

  use Gvar::Header

=head1 DESCRIPTION

This is something that could be documented.

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
