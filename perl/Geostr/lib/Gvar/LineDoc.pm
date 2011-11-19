package Gvar::LineDoc;
use strict 'vars';
use fields qw (
	       SPCID
	       SPSID
	       LSIDE
	       LIDET
	       LICHA
	       RISCT
	       L1SCAN
	       L2SCAN
	       LPIXLS
	       LWORDS
	       LZCOR
	       LLAG
	       LSPAR
	      );

sub new {
  my Gvar::LineDoc $l = shift;
  unless (ref $l) {
    $l = fields::new($l);
  }
  $l;
}

sub asString {
  my $l = shift;
  join(':',map(join('=',$_,$l->{$_}),keys %$l));
}

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::LineDoc - The LineDoc part of a GVAR::Record

=head1 SYNOPSIS

  use Gvar::LineDoc

=head1 DESCRIPTION

Internal Object

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
