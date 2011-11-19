package Gvar::Block::IR;
use strict 'vars';

use base 'Gvar::Block::10bit';
use fields;

sub num_records {
  (shift->{Header}->{block_id} == 1)?4:3;
}
1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::Block::IR - An IR Block from a GVAR stream.

=head1 SYNOPSIS

  use Gvar::Block::IR;

=head1 DESCRIPTION

Internal Object

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
