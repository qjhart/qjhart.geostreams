package Gvar::Record;
use strict 'vars';
use fields qw( lineDoc data );

sub new {
  my Gvar::Record $r = shift;
  unless (ref $r) {
    $r = fields::new($r);
  }
  $r;
}

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::Record - A single Record in a Block from a GVAR stream.

=head1 SYNOPSIS

  use Gvar::Block;

=head1 DESCRIPTION

Internal Object

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
