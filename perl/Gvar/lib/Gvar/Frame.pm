package Gvar::Frame;
use IO::File;
use Gvar::Block::Doc;
use strict 'vars';

use fields qw(input fh first_doc _last_doc);

# Here's the format for finding a valid header
our $HEADER_RE = '(.{14}).{16}((\1.{30})|(.{30}\1)).{16}';

#
# CONSTRUCTOR
#
sub new {
  my Gvar::Frame $f = shift;
  my %p=@_;
  unless (ref $f) {
    $f = fields::new($f);
  }
  $f->{input} = $p{input} if $p{input};
  $f;
}

sub bbox {
  my $frame=shift;
  my $doc=$frame->{first_doc};
  [$doc->{IWFPX},$doc->{ISFLN},$doc->{IEFPX},$doc->{INFLN}];
}

# SIMPLE IO::Handler Routines
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
  $self;
}

sub seek {
  shift->{fh}->seek(@_);
}

sub read {
  shift->{fh}->read(@_);
}

sub tell {
  shift->{fh}->tell;
}

sub eof {
  shift->{fh}->eof;
}

sub close {
  my $self = shift; 
  $self->{fh}->close;
  $self->{fh}=undef;
}

=pod 
=item read_header

Reads a header at the current fh location.  If a valid header does not 
exist, it returns undef and does not move the file pointer.

=cut

# Search for the next good header
sub read_header{
  my $self = shift;
  my $cur = $self->tell;
  $self->read(my $hbuf,30*3);
  my $header;
  
  if (!($hbuf =~ /^\x00{5}/) and 
      ($hbuf =~ /^$HEADER_RE/s ) and 
      ($header =  Gvar::Header->new($hbuf))
     ) {
    $header->{pos}=$cur;
  } else {
    $self->seek($cur,0);
  }
  $header;    
}

=pod
=item next_header

Sets the file position at the next valid header.

Parameters:

=over 4

=item block_type

Searches for the next header of that block_type.

=cut
  
sub next_header {
  my ($self,$count) = @_;
  $count  ||= 100000;		# Default look ahead
  my $buffer;
  my $pos;
 READ: for (my $bs = $self->tell; 
	    $self->read($buffer,$count) > 8040;	# Length of DOC Block
	    $bs=$self->tell) {
    
    while ( $buffer =~ /$HEADER_RE/sg) {
      next if $buffer =~ /\x00{30}/;
      $pos = $bs + pos($buffer) - 90;
      last READ;
    }
    $self->seek(-90,1);
  }
  $self->seek($pos,0) if $pos;
  $pos;
}

=pod

=item read_block

Reads a block at the current fh location.  If no valid block exists at
that location then it returns undef, and the file location is not
moved.  If the 'Data' parameter is passed true, then the Data for the 
block will be read otherwise the data will be skipped.

If the 'Header' parameter is passed, the function assumes that the
Header was previously read, and will try and read the rest of the
block, starting after the Header.

If the Header says the data is filler, then the data will not be read
regardless of what the 'Data' paramter is.

A valid block is one where the fh is pointing to a header, and we find
enough data for the rest of the block.  Validation of the data within
the blocks is still sketchy.

=cut

sub read_block {
  my $self = shift;
  my %i = @_;
  my $block;
  my $start = $self->tell;
  my $header = ($i{Header})?$i{Header}:$self->read_header;

  if ($header) {
    my $type=$header->block_type;
    if ($type) {
      my $class = "Gvar::Block::$type";
      $block = $class->new(Header=>$header);

      if ($i{Data}) {
	my $buffer;
	my $sz = $header->data_bytes;
	( $self->read($buffer,$sz) == $sz  and
	  $block->read_data($buffer)       and
	  $self->seek(2,1)	# This is the CRC check
	) or $block = undef;
      } else {
	# 2 bytes for the CRC check.
	$self->seek($header->data_bytes+2,1) or $block = undef;
      }
    $self->seek($start,0) unless $block;
    }
  }
  $block;
}

=pod

=item read_scan

Reads a scan at the current fh location.  If no valid block exists at
that location then it returns undef, and the file location is not
moved.  If the 'Channels' parameter is passed (an arrayref), then the
data corresponding to those channels (0..5) are read. 

If the 'Doc' parameter is passed, the function assumes that the
Doc Block was previously read, and will try and read the rest of the
scan, starting after the Doc.

=cut

sub read_scan {
  my $self = shift;
  my %i = @_;
  my $scan;
  my $start = $self->tell;
  my $doc = ($i{Doc})?$i{Doc}:$self->read_block(Data=>1);
  # Is the header okay?
  unless ($doc and $doc->isDoc) {
    $self->seek($start,0);
    return undef;
  }
  $$scan[0] = $doc;
  
  my @read_block;
  if ($i{Channels}) {
    foreach my $ch ( @{ $i{Channels} } ) {
      grep($read_block[$_]++,Gvar::Scan::channel_blocks($ch));
    }
  }
  for (1..10) {
    unless ($$scan[$_] = $self->read_block(Data=>$read_block[$_]) 
	    and $$scan[$_]->{Header}->{block_id} == $_ ) {
      $self->seek($start,0);
      return undef;
    }
  }
  # Read to the next DocBlock
  while (! $self->eof) {
    if (my $b = $self->read_block) {
      if ($b->isDoc) {
	$self->seek($b->{Header}->{pos},0);
	return new Gvar::Scan(blocks=>$scan);
      }
    }else {
      $self->seek($start,0);
      return undef;
    }
  }
  $self->seek($start,0);
  return undef;	
}

# High Level Functions 
sub first_doc {
  my $self=shift;
  my $doc;
  unless ($doc = $self->{first_doc}) {
    $self->open;
    $doc=$self->next_doc;
    $self->{first_doc}=$doc;
#    $self->seek(0,0);
  }
  $doc;
}

sub next_doc {
  my $self=shift;
  my $doc;
  READ: while (! $self->eof) {
      if (my $h = $self->read_header) {
	if ($h->isDoc) {
	  $doc = $self->read_block(Header=>$h,Data=>1);
	  last READ;
	} else {
	  $self->read_block(Header=>$h);
	}
      } else {
	$self->next_header;
      }
    }
  $doc;
}

sub last_doc {
  my $self=shift;
  my $doc;
  unless ($doc = $self->{_last_doc}) {
    $self->open;
  READ: while (! $self->eof) {
      if (my $h = $self->read_header) {
	$self->read_block(Header=>$h,Data=>$h->isDoc);
      } else {
	$self->next_header;
      }
    }
  }
  $self->{_last_doc}=$doc;
  $doc;
}

sub check_blocks {
  my $self = shift;
  my %i = @_;
  my @err;

  if ($self->open) {
    if (my $doc = $self->first_doc) {
      my $last_doc;
      my $pos;
      push @err,['File',$pos,"DocBlock"] unless
	(($pos = $doc->{Header}->{pos}) == 0);

      push @err,['Frame',$pos,"Not Frame_Start"] unless
	$doc->ISCAN->frame_start;
      
      # Check the Remaining blocks
      my $last;
      while (! $self->eof) {
	if (my $h = $self->read_header) {
	  if (my $b=$self->read_block(Header=>$h,
				      Data=>$h->isDoc)) {
	    $last = $h;
	    # Here is where we can check Doc blocks to the Frame doc
	    if ($b->isDoc) {
	      $last_doc = $b;
	      push @err,['CRITICAL',$h->pos,"New Frame $b->IFRAM"]
		unless $doc->IFRAM == $b->IFRAM;
	    }	      
	  } else {
	    push @err,['File',
		       $h->pos,
		       sprintf( "Bad Block %s (%s)",
				$h->block_count,$h->block_type)
		      ];
	    $self->next_header;
	  }
	} else {
	  push @err,['File',$last->pos,"Corrupt"];
	  $self->seek($last->pos,0);
	  $self->read_header;	# Start from last good one.
	  $self->next_header;
	}
      }
      # Save the last_doc;
      $self->{_last_doc}=$last_doc;
      push @err,['Frame',$pos,"Not Frame_End"] unless
	$last_doc->ISCAN->frame_end;      

      if (@err) {
	push @err,['File',$self->tell,"EOF"];
      }
    } else {    
      push @err,['CRITICAL',undef,"No Doc Block"];
    }
  } else {
    push @err,['CRITICAL',undef,"No File"];
  }
  @err;
}

sub asString {
  shift->b0->asString;    
}

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Gvar::Frame - Single frame for GVAR data.

=head1 SYNOPSIS

  use Gvar::Frame;

=head1 DESCRIPTION

The Frame holds all the Channels

=head1 AUTHOR

qjhart@ucdavis.edu

=head1 SEE ALSO

Gvar(1)

=cut
