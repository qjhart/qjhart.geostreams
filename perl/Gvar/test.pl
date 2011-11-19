# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

use Test;
BEGIN { plan tests => 1 ; }
END {print "not ok 1\n" unless $loaded;}
use Gvar;
$loaded = 1;
print "ok 1\n";
use constant pi => 4*atan2(1,1);

sub deg2rad { map(pi * $_ / 180,@_); }
sub rad2deg { map(180 * $_ / pi,@_); }

######################### End of black magic.

# Test Reader
my $file='block0.gvr';
my $frame = new Gvar::Frame(input => $file);
#my $header=$frame->read_header;
my $doc = $frame->first_doc;

# Test Header
my $h=$doc->{Header};
ok($doc->{ISCAN}->frame_start);
ok($h->{pos},0);
ok($h->{data_valid_flag});

#Test Documentation Block...
ok($doc);

#Read in the original test data;
my $instr=1;
#Gvar::setcon($instr,4,3487,2,3068);
# As in TEST.F:
Gvar::setcons(4,3068,2,3068,4,1402,2,1402);

my @oa;
#open(OA,'IMCSET_I.DAT')  or die "IMCSET_I.DAT missing";
open(OA,'ELREC.DAT')  or die "ELREC.DAT missing";
my @line=<OA>;
@oa=map (substr($_,0,20),@line);
$rec=pack('d336',@oa);
$imc=0;
$epoch=Gvar::timex(1994,299,00,00,0.0);
$time=Gvar::timex(1994,299,16,05,0.0);

# arbitrarely:
my  $flipflag = -1;   # 1=normal    (-1=flipped) 

my @model=Gvar::lmodel($time,$epoch,$rec,$imc);
my @pl = (11662,5032);

my @se = Gvar::pl2se($instr,@pl);

@lonlat=Gvar::se2ll($instr, $flipflag, @se);

my @lonlat2=Gvar::pl2ll($instr, $flipflag, @pl);
ok($lonlat[0]==$lonlat2[0] and $lonlat[1]==$lonlat2[1]);

my @pl2=Gvar::ll2pl($instr, $flipflag, @lonlat);
ok(abs($pl2[0]-$pl[0]) < 0.03 and abs($pl2[1]-$pl[1]) < 0.03);

@se = Gvar::ll2se($instr, $flipflag, @lonlat);
@pl2=Gvar::se2pl($instr,@se);
ok(abs($pl2[0]-$pl[0]) < 0.03 and abs($pl2[1]-$pl[1]) < 0.03);

########################
# Test with image header
my $oa= $doc->{oa};

#Gvar::setcon($instr,$doc->{IOFNC},$doc->{IOFNI},$doc->{IOFEC},$doc->{IOFEI});
# parameters for sounder????   Would be: SOFNC, SOFNI, SOFEC, SOFEI. Here set to zeroes:
Gvar::setcons($doc->{IOFNC},$doc->{IOFNI},$doc->{IOFEC},$doc->{IOFEI}, 0, 0, 0, 0);

# take the yaw-flipped flag:
$flipflag = ($doc->{ISCAN}->yaw_flip) ? -1 : 1;


# Instrument Pointing Pixels
@sub=($doc->{SUBLO},$doc->{SUBLA});
@igv=($doc->{IGVPX},$doc->{IGVLN});

my $e=$oa->{epoch};
$epoch=Gvar::timex($e->{Y},$e->{j},$e->{H},$e->{M},$e->{S}+$e->{ms}/1000);
#print $epoch, "\n";

$e=$doc->{TCURR};
my $time=Gvar::timex($e->{Y},$e->{j},$e->{H},$e->{M},$e->{S}+$e->{ms}/1000);

#print join(":",@{$oa->{parms}});

$imc = ($doc->{ISCAN}->IMC_active) ? 0 : 1;

@mod=Gvar::lmodel($time, $epoch, $oa->{data}, $imc);
my @pl=($doc->{IEFPX},$doc->{ISFLN});
my @ll=($doc->{IFSE2},$doc->{IFSE1});

@lonlat2=Gvar::pl2ll($instr, $flipflag, @pl);
@pl2=Gvar::ll2pl($instr, $flipflag, @lonlat2);
ok(abs($pl[0]-$pl2[0]) < 0.03 and abs($pl[1]-$pl2[1]) < 0.03);

# CA SouthWest, NorthEast
@en=Gvar::ll2pl($instr, $flipflag, deg2rad(-125,43));
@ws=Gvar::ll2pl($instr, $flipflag, deg2rad(-113,32));

ok(1);
