package Grass;
use strict;
use warnings;
use File::Copy;
use File::Temp qw(tempfile);

our $VERSION = '0.01';

use fields qw(_gisbase _gisrc _mapset _location_name _gisdbase _location _grass_gui _monitor);

sub new {
  my $g = shift;
  my %p=@_;
  unless (ref $g) {
    $g = fields::new($g);
  }
  $p{gisbase} ||= '/usr/local/grass5';
#  unless $p{gisrc} ||= "$ENV{HOME}/.grassrc5";
  unless ($p{gisrc}) {
      (undef,$p{gisrc})=tempfile("gisrcXXXXX",UNLINK=>1);
 }
  $g->gisrc($p{gisrc});
  for (qw(gisbase gisdbase location_name mapset)) {
      $g->$_($p{$_}) if $p{$_};
  }
  $g;
}

sub gisbase {
  my $g=shift;
  if (@_) {
    my $gdb=shift;
    $g->{_gisbase}=$gdb;
    $ENV{GISBASE}=$gdb;
    $ENV{PATH}="$gdb/bin:$gdb/scripts:".$ENV{PATH};
  }
  $g->{_gisbase};
}

sub gisrc {
  my $g=shift;
  if (@_) {
    $g->{_gisrc}=shift;
    $ENV{GISRC}=$g->{_gisrc};
    if (-f $g->{_gisrc}) {
      my %p;
      my $file=$g->{_gisrc};
      open(RCO,"$file") or die "Can't read $file";
      my @in=<RCO>;
      close RCO;
      foreach (@in) {
	chomp;
	my($k,$v)=split(/:\s*/);
	$p{"\L$k"}||=$v;
      }
      foreach (qw(mapset location_name gisdbase)) {
	$g->{"_$_"}=$p{$_} if $p{$_};
      }
    }
  }
  $g->{_gisrc};
}
    
sub location {
  my $g=shift;
  "$g->{_gisdbase}/$g->{_location_name}";  
}

sub write {
  my $g = shift;
  my $file = $g->{_gisrc};
  open(RC,">$file") or die "Can't write $file";
  for (qw(mapset gisdbase location_name)) {
    printf RC "%s: %s\n","\U$_",$g->{"_$_"} if defined($g->{"_$_"});
  }
  close(RC);
}

sub gisdbase {
  my ($g,$gisdbase)=@_;
  return $g->{_gisdbase} unless $gisdbase;
  return undef unless (-d "$gisdbase");
  $g->{_gisdbase}=$gisdbase;
  $g->write;
  $g;
} 

sub location_name {
  my ($g,$location_name)=@_;
  return $g->{_location_name} unless $location_name;
  return undef unless (-d "$g->{_gisdbase}/$location_name");
  $g->{_location_name}=$location_name;
  $g->write;
  $location_name;
} 

sub mapset {
  my ($g,$mapset)=@_;
  return $g->{_mapset} unless $mapset;
  return undef unless (-d "$g->{_gisdbase}/$g->{_location_name}/$mapset");
  $g->{_mapset}=$mapset;
  $g->write;
  $mapset;
} 
 
sub delete_mapset {
  my ($g,$mapset)=@_;
  my $l=$g->location;
  if (-d "$l/$mapset") {
    my $rm="rm -rf $l/$mapset";
    system $rm;
  }
}
  
sub new_mapset {
  my ($g,$mapset,%p)=@_;
  my $l=$g->location;
  return undef if (-d "$l/$mapset");
  $g->{_mapset}=$mapset;
  my $dir="$l/$mapset";
  mkdir $dir;
  copy "$l/PERMANENT/DEFAULT_WIND","$dir/WIND";
  $g->write;
  $mapset;
}

sub set_or_new_mapset {
  my ($g,$mapset)=@_;
  my $ms;
  unless ($ms=$g->mapset($mapset)) {
    $ms=$g->new_mapset($mapset);
  }
  $ms;
}

package Grass::Region;
sub new {
  my ($class,$file)=@_;
  my $region={};

  if ($file) {
    open(REGION,"$file") or die "Can't read $file";
    while (<REGION>) {
      chomp;
      my($k,$v)=split(/:\s*/);
      $region->{$k}=$v;
    }
  }
  bless $region,$class;
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Grass - Perl extension for running under grass

=head1 SYNOPSIS

  use Grass;
  use Getopt::Long;

my $grass=new Grass::Environment;

GetOptions(
	   $grass->Getopt::Long,
	   "file=s"=>\$file
	   );

$grass->envrionment;

=head1 DESCRIPTION

This package is supposed to allow access to Grass from PERL


=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1), perl(1).

=cut
