package GeoStr::Server;
%GeoStr::Server=(
		 QueryId=>0,
		 Query=>{},
		);

use GeoStr::Query;

# JSON Accessor
sub register_query {
  my $class = __PACKAGE__;
  my ($json_client, $query_parms) = @_;
  my $id=++$class->{QueryId};
  my $query = GeoStr::Query->new(%$query_parms);
  $query->id($id);
  $class->{Query->{$id}}=$query;
  $class->plan($query);
  $id;
}

sub query_info {
}

sub plan {
  my ($self,$query) = @_;
  my $box=new GeoStr::Box(box=>$q->box,
			  proj=>$q->proj,
			  width=>$q->width,
			  height=>$q->height);
  my $goes=$box;
  #my $goes=$box->project({+proj=>'goes',+goes=>10});

  # For GOES data, the resolution is integer units
  my ($x,$y) = map(floor($_),$goes->resolution);
  # Need to be at least half as big for proper averaging
  $goes->expand($x/2,$y/2);
  $plan="goes->Average($x,$y)->select($goes)->Project($proj,$res)->select($box)";

}


package GeoStr::Server::wcs;

sub GetCapabilities {
    my ($class, $elem) = @_;
	my $xml = qq|<wcs:Capabilities xmlns:wcs="http://www.opengis.net/wcs" version="1.0.20"></wcs:Capabilities>|;

	return SOAP::Data->type('xml' => $xml );
}

sub DescribeCoverage {
  my ($class,$elem) = @_;
  my $xml = qq|<CoverageDescription xmlns:ows="http://www.opengis.net/ows" xmlns:wcs="http://www.opengis.net/wcs" xmlns:xsi="http://www.w3.org/2001/XMLSchemainstance" xsi:schemaLocation="http://www.opengis.net/wcs http://www.ionicsoft.com:8080/wcs/coverage/SPOT5J /REQUEST/get/DATA/LPR/wcs/1.0.20/wcsDescribeCover age.xsd" xmlns="http://www.opengis.net/wcs" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:gml="http://www.opengis.net/gml"> <wcs:CoverageOffering> <wcs:description> Coverage </wcs:description> <wcs:name>TIF</wcs:name>|;
  	return SOAP::Data->type('xml' => $xml_content );
}

sub GetCoverage {
  my ($class,$elem) = @_;
  my $xml = qq|FOO BAR|;
  return SOAP::Data->type('xml' => $xml_content );
}

1;
