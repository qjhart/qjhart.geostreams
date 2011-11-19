package Gvar;
use strict;
use warnings;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
our %EXPORT_TAGS = ( 'all' => [ qw(
	evln	gpoint	lmodel	lpoint	setcon	time50 	timex
				  ) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our $VERSION = '0.08';

bootstrap Gvar $VERSION;


#Just get some packages
use Gvar::Frame;
use Gvar::Scan;
use Gvar::Header;
use Gvar::Block;
#use Gvar::OA;
use Gvar::Block::Doc;
use Gvar::Block::10bit;
use Gvar::Block::Vis;
use Gvar::Block::IR;
use Gvar::Block::Auxil;
use Gvar::Block::EIB;

# Here are some system constants...
our %imager=
  (
   incmax=>6136,		# increments/cycle
   scnincr=>16,			# change scan angle/incre [mrad]
   elvincr=>8,			# change in elev/incr [mrad]
   scnpx=>16,
   elvln=>28,
   scnmax=>undef,
   envmax=>undef,
  );

our %sounder=
  (
   incmax=>2805,		# increments/cycle
   scnincr=>35,			# change scan angle/incre [mrad]
   elvincr=>17.5,			# change in elev/incr [mrad]
   scnpx=>280,
   elvln=>280,
   scnmax=>undef,
   envmax=>undef,
  );

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar - Perl extension for reading GOES GVAR files

=head1 SYNOPSIS

  use Gvar;
  use Getopt::Long;

  my $file;

  GetOptions("file=s"=>\$file);

  Gvar::Frame->new->save(Reader=>Gvar::Reader->new(file=>$file));

=head1 DESCRIPTION

This package is supposed to allow us to read GOES data files.

=head1 Gvar Objects.

A Gvar::Frame corresponds to a simgle multi channel imager image from
the GOES satellite.  It would be nice if Frames always correspond to
files, but that is not always the case.  You can tell when frames
start and end by looking at the Gvar::Doc blocks; particularly the
{ISCAN} parameter of the doc block.


There are many Gvar::Scan objects within a Gvar::Frame.  All
Gvar::Scan start with a Gvar::Doc object which contains info about the
scan and the frame. The doc object also includes information regarding
the size of the current frame.  Frames are $doc->{IFRAM} is a counter
of the current frame.  It rolls over every 255.

A Gvar::Scan is basically one sweep of the imager detectors, it is
made up of 8 scan lines.  Not all the Gvar:Scan objects contain valid
data.  $doc->{INSLN} shows what absolute lines are contained in the
current Scan.  It contains a list of Gvar::Blocks, including the Doc
Block and either Vis or IR image blocks.


=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

perl(1).

=cut
