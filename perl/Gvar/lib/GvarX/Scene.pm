package GvarX::Scene;
use fields qw( bbox cover channels pdl fs doc);
use strict 'vars';
use URI;
use PDL;

{
  my @DIV = (undef,[1,1],[4,4],[4,8],[4,4],[4,4]);
  sub divisor { shift; @{ $DIV[shift] || [1,1] } ; } 
}

# file://%Y%j_%c?BBOX=w,s,e,n
# file://%Y%j_%c?LAYERS=1,2,3,4,5&BBOX=16000,3800,19000,4700
sub new {
  my GvarX::Scene $sc=shift;
  my %p=@_;
  if ($p{uri}) {
    my $uri=new URI $p{uri};
    my %q= $uri->query_form;
    $p{bbox} ||= [split(',',$q{BBOX})];
    my $fs=$uri->path;
    $fs=~ s/\?.*//;
    $p{fs}||=$fs;
    $p{channels}||=[split(',',$q{LAYERS})];
  }
  unless (ref $sc) {
    $sc = fields::new($sc);
    foreach (qw(fs channels bbox cover)) {
      $sc->{$_}=$p{$_} if $p{$_};
    }
    $sc->{doc}=[];
    $sc->{pdl}=[undef,undef,undef,undef,undef,undef];
  }
  return $sc;
}

sub bbox { shift->{bbox}; }

sub pixels {
  my ($x,$y) = shift->size(@_);
  $x*$y;
}

sub size {
  my $scene = shift;
  my $a = $scene->{bbox};
  my $ch = shift || 1;
  my @div = $scene->divisor($ch);

  # Compute the X,Y
  (1 + int(($$a[2]-$$a[0]) / $div[0]),1 + int(($$a[1]-$$a[3]) / $div[1]));
}

sub overlap {
  my ($as,$bs) = @_;
  my @a=@ { $as->{bbox} };
  my @b = @{ $bs->bbox() };	# Scene or Scan or Doc

  # (w,s,e,n) but n<s in this projection
  #((min S of a,b)-(max N of a,b)) * ((min E of a,b)-(max W of a,b)) 
  my $o = [ ( $a[0] > $b[0] )?$a[0]:$b[0] ,
	    ( $a[1] < $b[1] )?$a[1]:$b[1],
	    ( $a[2] < $b[2] )?$a[2]:$b[2] ,
	    ( $a[3] > $b[3] )?$a[3]:$b[3]
	  ];
  return undef if ($$o[0] > $$o[2]) or ($$o[1] < $$o[3]);
  $o;
}

sub match {
  my $scene = shift;
  my $b = shift;
  my $match;

  if ($scene->{bbox}) {
    my $over = $scene->{cover} || $scene->pixels;
    $over *= $scene->pixels if ($over <= 1.0);
    if (my $bbox=$scene->overlap($b)) {
      my $o = GvarX::Scene->new(bbox=>$bbox);
      $match++ if ($o->pixels >= $over);
    }
  }
  $match;
}

sub insert {
  my ($scene,$scan) = @_;

  my $o;
  my $doc=$scan->doc;

  return undef unless ($o=$scene->overlap($scan));

  # Get relative overlaps.
  my ($scener,$scanr) = 
    map([$$o[0] - $$_[0],$$o[1] - $$_[3],$$o[2] - $$_[0],$$o[3] - $$_[3]],
	($scene->{bbox},$scan->bbox)
       );

  # First to Visible
  if (defined($scene->{pdl}->[1])) {
      my $scene_d=$scene->{pdl}->[1];
      my $scan_d=$scan->data(1);
      if (defined($scan_d)) {
	  # Put in PDL x:x,y:y order
	  my ($adim,$bdim) = 
	      map(
		  [[ $$_[0],$$_[2] ],[$$_[1],$$_[3]]],
		  ($scener,$scanr)
		  );
	  (my $j = $scene_d->mslice(@$adim)) .= $scan_d->mslice(@$bdim);
	  # Save doc blocks;
	  grep( $ { $scene->{doc}}[$_]=$doc,($scener->[3]..$scener->[1]));
      }
  }

  # IR not yet done
}

# Reads in 
sub read_frame {
  my ($scene,$frame,%p) =@_;
  my (undef,$y)=$scene->size;
  my @rows_to_do=(0..$y-1);
  
  $scene->{pdl}=[undef,zeroes(float,$scene->size)];
  
  # Okay, now we need to read the file if any of the scenes need
  # data.
  $frame->open;
  my $last_header;
  
 SCAN: while (! $frame->eof) {
    my ($h,$b);
    # Read the next Header or seek to the next good one.
    unless ($h = $frame->read_header) {
      if ($last_header) {
	$frame->seek($last_header->{pos},0);
	$frame->read_header;
      }
      $frame->next_header;
      next;
    }
    $last_header = $h;
    # Read the next block, or start again if bad
    #print STDERR $h->{block_count}.":";
    next unless ($b = $frame->read_block(Header=>$h,Data=>$h->isDoc));
    next unless $h->isDoc;
    #print STDERR "line:",join(",",@{$b->bbox})," scene:",join(",",@{$scene->{bbox}}),"\n";
    # Save if requested
    if ($p{save_doc}) {
	$p{save_doc}->syswrite($h->{buffer});
	$p{save_doc}->syswrite($b->{buffer});
	$p{save_doc}->syswrite('  ');
    }
    # Skip if we're past end
    last SCAN if (($ { $b->bbox }[1] - $ { $scene->bbox }[1]) > 500);
    if ($scene->overlap($b)) {
      if (my $scan=$frame->read_scan(Doc=>$b,
				     Channels=>$scene->{channels})) {
	$scene->insert($scan);
	@rows_to_do=grep(!defined($scene->{doc}->[$_]),@rows_to_do);
	last SCAN unless @rows_to_do;
      }
    }
  }
  # Return rows with data filled in (or count)
  grep(defined($scene->{doc}->[$_]),(0..$y-1));
}
