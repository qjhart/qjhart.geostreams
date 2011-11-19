=pod

=head1 NAME

GeoStr - Streaming GOES Server/Client libraries

=head1 SYNOPSIS

  use GeoStr;

=head1 ABSTRACT

This set of functions allow the creation of a server of streaming GOES
data.

=cut

package GeoStr;

use 5.008004;
use strict;
use warnings;

#use GeoStr::Client;
#use GeoStr::Server;

use GeoStr::Plan;
#use GeoStr::Plan::All;
#use GeoStr::Plan::Merge;
#use GeoStr::Plan::Halve;

use GeoStr::Box;
use GeoStr::Node;
use GeoStr::Node::Layer;
use GeoStr::Node::Send;
use GeoStr::Node::Average;
use GeoStr::Node::Interpolate;
use GeoStr::Node::Halve;
use GeoStr::Node::MapCalc;

use GeoStr::Query;

require Exporter;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use geostrd ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.02';


# Preloaded methods go here.

1;
__END__
