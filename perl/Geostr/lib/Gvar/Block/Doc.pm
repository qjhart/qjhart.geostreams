package Gvar::Block::Doc;
use Gvar::ISCAN;
use Gvar::TimeTag;

use base Gvar::Block;

use fields 
  qw (
	 SPCID SPSID ISCAN IDSUB
	 TCURR TCHED TCCTL TLHED TLTRL TIPFS TINFS TISPC TIECL TIBBC TISTR TLRAN TIIRT TIVIT TCLMT TIONA
	 
	 RISCT AISCT INSLN IWFPX IEFPX INFLN ISFLN IMDPX IMDLN IMDCT 
	 IGVLN IGVPX SUBLA SUBLO CZONE V1PHY G1CNT G2CNT PBIAS LBIAS ISCP1
	 IDBER RANGE GPATH XMSNE TGPAT TXMSN ISTIM IFRAM IMODE 
	 IFNW1 IFNW2 IFSE1 IFSE2 IG2TN ISCP2 ISCA2 PARITY
      
      oa

    IOFNC IOFEC IOFNI IOFEI

    IVCRB IVCR1 IVCR2 IVRAL

	);

sub bbox {
  my ($doc,$ch) = @_;
  [$doc->{IWFPX},$doc->{INSLN}+7,$doc->{IEFPX},$doc->{INSLN}];
}

sub set_projection {
    my ($doc)=@_;
    my $oa=$doc->{oa};
    # This is Image specfic right now.
    Gvar::setcons($doc->{IOFNC},$doc->{IOFNI},$doc->{IOFEC},$doc->{IOFEI},0,0,0,0);
    my $ep=$oa->{epoch};
    my $epoch=Gvar::timex($ep->{Y},$ep->{j},$ep->{H},$ep->{M},$ep->{S}+$ep->{ms}/1000);
    
    my $start_time=$doc->{TCHED};
    my $time=Gvar::timex($start_time->{Y},
			 $start_time->{j},
			 $start_time->{H},
			 $start_time->{M},
			 $start_time->{S}+$start_time->{ms}/1000);
	      
    my @mod=Gvar::lmodel($time,$epoch,$oa->{data},($doc->{ISCAN}->IMC_active)?0:1)
}

sub set_special {
    my ($doc)=@_;
    my $oa=$doc->{oa};
    # This is Image specfic right now.
    Gvar::setcons($doc->{IOFNC},$doc->{IOFNI},$doc->{IOFEC},$doc->{IOFEI},0,0,0,0);
    my $ep=$oa->{epoch};
    my $epoch=Gvar::timex($ep->{Y},$ep->{j},$ep->{H},$ep->{M},$ep->{S}+$ep->{ms}/1000);
    
    my $start_time=$doc->{TCHED};
    my $time=Gvar::timex($start_time->{Y},
			 $start_time->{j},
			 $start_time->{H},
			 $start_time->{M},
			 $start_time->{S}+$start_time->{ms}/1000);
	      
    my @mod=Gvar::lmodel($time,$epoch,$oa->{data},($doc->{ISCAN}->IMC_active)?1:0)
}

sub convert_float {
  my ($e,@m) = unpack('C*',shift);
  my $f = 1.0;
  if ($e & 0x80) {
    ($e,@m)=map($_ ^ 0xff,$e,@m);
    $f=-1.0;
  }
  my $man = unpack('N',pack('C*',0x00,@m));
  if($e > 0x40) {
    $f *= $man*(1<<(4*($e-0x40)))/0x1000000; 
  } else { 
    $f *= $man/(1<<(4*(0x40-$e)))/0x1000000; 
  }
}

sub read_data {
  my $doc=shift;
  my $buffer = shift;

  # Major Divisions....
  my $format='c c a4 H32 a128 a128 a1412 x4614 a94 a128';
  # NADIR (a94) 6305-6398
  my $vis_cal;
  # vis_cal (8detectors*4perbyte*4values)= a128
  my ($iscan,$times,$frame_info,$oa_info);

  ($doc->{SPCID},$doc->{SPSID},$iscan,$doc->{IDSUB},
   $times,
   $frame_info,
   $oa_info,
   $nadir,
   $vis_cal,
  ) = unpack($format,$buffer);

  $doc->{ISCAN}=new Gvar::ISCAN($iscan);

  {
    # Unpack Time tags
    my @time = unpack('a8 'x16,$times);
    foreach (qw(TCURR TCHED TCCTL TLHED TLTRL TIPFS TINFS TISPC TIECL TIBBC TISTR TLRAN TIIRT TIVIT TCLMT TIONA)) {
      $doc->{$_}=new Gvar::TimeTag(shift @time);
    }
  }
  {
    # Unpack Frame info
    # Now the Gvar is network order
    # Floating point format will be machine dependant
    my $format = join(' ',
		      'n10',
		      'n n a4 a4 c c n n n n c x',
		      'a4 a4 a4 a4 a8 a8 n c c',
		      'a4 a4 a4 a4 c c n x27 c',
		     );

    ($doc->{RISCT},$doc->{AISCT},$doc->{INSLN},$doc->{IWFPX},$doc->{IEFPX},$doc->{INFLN},$doc->{ISFLN},$doc->{IMDPX},$doc->{IMDLN},$doc->{IMDCT},
     $doc->{IGVLN},$doc->{IGVPX},$doc->{SUBLA},$doc->{SUBLO},$doc->{CZONE},$doc->{V1PHY},$doc->{G1CNT},$doc->{G2CNT},$doc->{PBIAS},$doc->{LBIAS},$doc->{ISCP1},
$doc->{IDBER},$doc->{RANGE},$doc->{GPATH},$doc->{XMSNE},$doc->{TGPAT},$doc->{TXMSN},$doc->{ISTIM},$doc->{IFRAM},$doc->{IMODE},
     $doc->{IFNW1},$doc->{IFNW2},$doc->{IFSE1},$doc->{IFSE2},$doc->{IG2TN},$doc->{ISCP2},$doc->{ISCA2},$doc->{PARITY})=unpack($format,$frame_info);
    # Do some extra conversions
    $doc->{TGPAT}=new Gvar::TimeTag($doc->{TGPAT});
    $doc->{TXMSN}=new Gvar::TimeTag($doc->{TXMSN});
    foreach (qw(SUBLA SUBLO IDBER RANGE GPATH XMSNE IFNW1 IFNW2 IFSE1 IFSE2)) {
      $doc->{$_}=convert_float($doc->{$_});
    }
  }
  {
    #Unpack the oa_info 279-1623? 
    my (@oa)=unpack('a4 'x336,$oa_info);
    $doc->{oa}={};
    $doc->{oa}->{imc_identifier}=$oa[0];
    $doc->{oa}->{epoch}=new Gvar::TimeTag(join('',@oa[11,12]));
    @oa[1..10,13..335]=map(convert_float($_),@oa[1..10,13..335]);
    $doc->{oa}->{parms}=[@oa];
#    $doc->{oa}->{data}=pack('d336',@oa);
    $doc->{oa}->{data}=pack('a8 d10 a8 a8 d323',@oa);
  }
  {
    #Unpack nadir location
    ($doc->{IOFNC},$doc->{IOFEC},$doc->{IOFNI},$doc->{IOFEI})=unpack('c c n n',$nadir);
  }

  {# Unpack vis_cal
    my @cal = map(convert_float($_),unpack('a4 'x32,$vis_cal));
    $doc->{IVCRB}=[@cal[0..7]];
    $doc->{IVCR1}=[@cal[8..15]];
    $doc->{IVCR2}=[@cal[16..23]];
    $doc->{IVRAL}=[@cal[24..31]];
  }
  $doc;

}

# Is the image flipped?
sub flipflag {
    my $doc=shift;
    ($doc->{ISCAN}->yaw_flip) ? -1 : 1;
}

sub translate {
  my $doc = shift;
  my $str = shift;
  my $date = $doc->{TIPFS};
  $str=$date->translate($str);
  $str =~ s/\%c/$doc->{IFRAM}/g;
  $str;
}

sub asString {
  my ($doc,%p) = @_;
  $p{delim} ||= "\n";
  $p{parms} ||= [ qw( IFRAM SPCID SPSID ISCAN TCURR TCHED RISCT AISCT INSLN IWFPX IEFPX INFLN ISFLN IFNW1 IFNW2 IFSE1 IFSE2 ) ];
  $doc->SUPER::asString(%p);
}

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::Block::Doc - A Doc (block id 0) Block from a GVAR stream.

=head1 SYNOPSIS

  use Gvar::Block::Doc;

=head1 DESCRIPTION

Internal Object

=pod
=item translate

translate is designed to do some substitutions on strings.  Primarily this
is for computing filenames using info from the Doc block.  The following
substitutions are made.  The Timetag used is the TIPFS tag.

%c = IFRAM

=cut

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
