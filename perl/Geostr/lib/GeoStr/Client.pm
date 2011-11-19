package GeoStr::Client;
use strict;
use warnings;

use Data::Dumper;
use Getopt::Long;
use Pod::Usage;
use JSONRPC::Transport::LOCAL;

use Class::MethodMaker
  [ scalar => [ { -type=>'JSONRPC'},qw/ transport /],
    scalar => 'proxy',
    new => [ qw/ _new /],
  ];

sub new {
  my $client = shift->_new;
  while (my $key = shift) { $client->$key(shift); }
#  my $json = JSONRPC::Transport::HTTP->proxy($client->proxy);
  my $json=JSONRPC::Transport::LOCAL->proxy($client->proxy);
  $client->transport($json);
  $client;
}

sub register_query {
  my ($client,$query) = @_;
  my $res=$client->transport->call('register_query',[{%$query}])->result;
  die $res->error if $res->error;
  $res->result->[0];
}

1;
