# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl geostrd.t'

#########################
use Math::Trig;

# change 'tests => 1' to 'tests => last_test_to_print';

use Test::More tests => 13;
BEGIN { use_ok('Gvar') };

#########################

# Test Reader
my $file='t/block0.gvr';
my $frame = new Gvar::Frame(input => $file);
#my $header=$frame->read_header;
my $doc = $frame->first_doc;

# Test Header
my $h=$doc->{Header};
ok($doc->{ISCAN}->frame_start==1,'Frame Start');
ok($h->{data_valid_flag}==1,'Data Valid Flag');

# Test Time Routines
#foreach (qw(TCURR TCHED TCCTL TLHED TLTRL TIPFS TINFS TISPC TIECL TIBBC TISTR TLRAN TIIRT TIVIT TCLMT TIONA)) {
#  print $_." ".$doc->{$_}->{date}."\n";
#}
ok($doc->{TCHED}->{date} eq '2005110618:45:13 UT', "Checking TimeTag ".$doc->{TCHED}->{date});

# Test that we can read the projection information 
my %this;
$this{goes}=$doc->{SPCID};

my $flipflag=$doc->flipflag;

my $ISCAN=$doc->{ISCAN};
ok($ISCAN->IMC_active==1,'IMC is active');

my $oa = $doc->{oa};
my $parms=$oa->{parms};
$this{lam} = $parms->[4];			# Longitude
$this{dr} = $parms->[5];			# radial distance from Nominal
$this{phi} = $parms->[6];			# latitude
$this{psi} = $parms->[7];			# Orbital YAW
$this{roll} = $parms->[8]; # Attitude Roll
$this{pitch} = $parms->[9];# Attitude pitch
$this{yaw} = $parms->[10]; # Attitude yaw

my %defaults=
  (
   goes=>10,
   lam=>[-135*atan(1)/45,1e-6],
   dr=>0,
   phi=>0,
   psi=>0,
   pitch=>0,
   roll=>0,
   yaw=>0,
  );

for (keys %defaults) {
  if (ref($defaults{$_}) eq ARRAY) {
    ok(abs($this{$_}-$defaults{$_}->[0]) < $defaults{$_}->[1],"Parameter $_ ($this{$_} within $defaults{$_}->[1])");
  } else {
    ok($this{$_}==$defaults{$_},"Parameter $_ ($this{$_})");
  }
}


