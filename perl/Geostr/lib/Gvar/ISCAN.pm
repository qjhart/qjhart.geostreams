package Gvar::ISCAN;
use strict 'vars';

#Add in the bit functions
{
  my @bit =
    qw (
	frame_start
	frame_end
	frame_break
	pixels_lost
	priority_1
	priority_2
	east_to_west
	south_to_north
	IMC_active
	lost_header
	lost_trailer
	lost_telemetry
	time_break
	side_2
	vis_norm
	IR_cal
	yaw_flip
	IR_1_invalid
	IR_2_invalid
	IR_3_invalid
	IR_4_invalid
	IR_5_invalid
	IR_6_invalid
	IR_7_invalid
	vis_1_invalid
	vis_2_invalid
	vis_3_invalid
	vis_4_invalid
	vis_5_invalid
	vis_6_invalid
	vis_7_invalid
	vis_8_invalid
       );

  for (my $i=0; $i<=$#bit; $i++) {
    my $datum = $bit[$i];
    my $v = $i;
    no strict "refs";
    *$datum = sub {
      use strict "refs";
      vec(${ shift @_ },$v,1);
    }
  }
}

=pod

=item new

A call to a new ISCAN returns a bit vector.  This is a reverse order
representation of what is passed from the GVAR file.

=cut
sub new {
  my $class = shift;
  my $self;
  $$self =pack('b*',reverse unpack('B*',shift)); 
  bless $self,$class;
}

sub asString {
  my $self= shift;
  unpack('b*',$$self);
}

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::ISCAN - Reads the Gvar ISCAN variable

=head1 SYNOPSIS

  use Gvar::ISCAN;
  use GVar::Doc;

  my $doc = new Gvar::Block::Doc(file)
  my $iscan=$doc->{ISCAN}
  printf "Block is YAW flipped\n" if ($iscan->yaw_flipped);

=head1 DESCRIPTION

This object stores the bit vector that is found in the ISCAN parameter
of the GVAR document header.  Creating these objects part of the
Document header reading, and not of your interest.

You should probably use the functional methods to access individual
bits of the ISCA parameter.  All bits, identified in the documentation
are available, the return code is whether the bit is set or not.

The string representation of this object is a line of 1s and 0s,
showing which flags are currently (un)set.

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(3pm) Gvar::Block::Doc(3pm)

=cut
