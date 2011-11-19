package Gvar::Block::10bit;
use Gvar::LineDoc;
use Gvar::Record;

use strict 'vars';
use PDL;
use vars qw ($EV $LDD);

use base Gvar::Block;
use fields qw( records);

# 10bit -> 16 bit conversions
$LDD = [
	['SPCID',0],
	['SPSID',1],
	['LSIDE',2],
	['LIDET',3],
	['LICHA',4],
	['RISCT',5,6],
	['L1SCAN',7],
	['L2SCAN',8],
	['LPIXLS',9,10],
	['LWORDS',11,12],
	['LZCOR',13],
	['LLAG',14],
	['LSPAR',15],
       ];

$EV = [
       [0x00,0,6, 0x3ff],	# 0
       [0x3f,4,12,0x00f],
       [0x00,0,2, 0x3ff],	# 2
       [0x03,8,8, 0x0ff],
       [0xff,2,14,0x003],	# 4
       [0x00,0,4, 0x3ff],
       [0x0f,6,10,0x03f],	# 6
       [0x00,0,0, 0x3ff],
      ];

sub convert_10_to_16 {
  
  my $in = shift;		# These are the 16 bit words read.
  # From our Gvar server, the sorts are packed little-endian,
  # swap them if this isn't the case on other machines
  # $in->bswap2 if isbigendian;
  # The new GVAR server packs them bigendian
  $in->bswap2;
  
  my ($count)=$in->dims;
  my $out = zeroes ushort,$count*16/10;
  $in=$in->splitdim(0,5);
  $out=$out->splitdim(0,8);
  
  my $a=0;
  foreach (my $b=0; $b<8; $b++) {
    my $o = $out->slice("($b)");
    my ($mm,$ms,$ls,$lm)=@{ $EV->[$b] };
    if ($lm == 0x3ff) {	# In one 16bit word
      my $l = $in->slice("($a)");
      $o .= ($l>>$ls)&$lm;
    } else {
      my $m = $in->slice("($a)");
      $a++;
      my $l = $in->slice("($a)");
      $o.=(($m&$mm)<<$ms)+(($l>>$ls)&$lm);
    }
  }
  $out->clump(2);
}


sub read_data {
  my $self=shift;
  my $buffer = shift;
  my $pdl = zeroes ushort,$self->{Header}->data_bytes/2 ;
  
  ${ $pdl->get_dataref() } = $buffer;
  $pdl->upd_data();
				# Change to 16 bit data
  $pdl = convert_10_to_16($pdl);
  
  my $records = [];
  foreach (1..$self->num_records) {
    my $record = new Gvar::Record;
    my $ld = new Gvar::LineDoc;
    my @ldd=list $pdl->slice('0:15');
    foreach ( @$LDD ) {
      my ($var,$ms,$ls)=@{ $_ };
      if (defined($ls)) {
	$ld->{$var}=($ldd[$ms]<<10) + $ldd[$ls];
      } else {
	$ld->{$var}=$ldd[$ms];
      }
    }
    $record->{lineDoc}= $ld;
    # The ldd data is still attached to data

    # Sometimes, the LPIXLS value is FUBAR so try and fix.
    my $lpixls=($ld->{LPIXLS}<=$ld->{LWORDS}-16)?$ld->{LPIXLS}+15:$ld->{LWORDS}-1;
    eval {
      $record->{data}=$pdl->mslice([16,$lpixls])->copy;
    };
    if ($@) {
      print STDERR $@;
    }
    # Throw away words
    $pdl = $pdl->mslice([$ld->{LWORDS},-1]);
    push @$records,$record;
  }
  $self->{records}=$records;
} 

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::Block::10bit - A 10bit Block from a GVAR stream.

=head1 SYNOPSIS

  use Gvar::Block::10bit;

=head1 DESCRIPTION

Internal Object

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
