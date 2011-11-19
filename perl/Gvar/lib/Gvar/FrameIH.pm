package Gvar::FrameIH;
use Gvar::IMAGEHEADER;

use base Gvar::Frame;
use fields 
  qw (
      image_header
      );

# On Open, read and validate the new style image header.
sub open {
  my $self = shift;
  if ($self->{fh}) {
    $self->seek(0,0);
  } else {
    if (my $input = $self->{input}) {
      unless ($self->{fh}=new IO::File "<$input") {
	warn "Can't Open $input\n";
	return undef;
      }
    } else {
      warn "No File specified";
      return undef;
    }
  }
  {
      $self->read(my $hbuf,606500);
      my $ih;
  
      unless ($self->{image_header} = Gvar::IMAGEHEADER->new($hbuf)) {
	  warn "Can't Read ImageHeader\n";
	  return undef;
      }
  }
  $self;
}
1;
