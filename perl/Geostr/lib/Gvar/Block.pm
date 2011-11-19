package Gvar::Block;
use PDL;
use strict 'vars';
use fields qw(Header Data invalid);

sub new {
  my $b = shift;
  my %p=@_;
  unless (ref $b) {
    $b = fields::new($b);
  }
  $b->{Header} = $p{Header} if $p{Header};
  $b;
}

sub isDoc { shift->{Header}->isDoc; }

sub asString {
  my ($self,%p) = @_;
  my $delim = $p{delim} || "\n";
  my $parms = $p{parms} || [];
  my @vals;
    foreach ( @$parms ) { 
      my $val = $self->{$_};
      push @vals,join("=",$_,(ref($val))?$val->asString:$val);
    }
  join($delim,@vals);
}

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::Block - A single Block from a GVAR stream.

=head1 SYNOPSIS

  use Gvar::Block;

=head1 DESCRIPTION

Internal Object

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
