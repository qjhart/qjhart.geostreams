use 5.008004;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile
  (
   NAME              => 'Geostr',
   VERSION_FROM      => 'lib/GeoStr.pm', # finds $VERSION
   EXE_FILES=>[qw( bin/geostr bin/goes.proj_info bin/goes.fixup)],
   PREREQ_PM         => {Getopt::Long=>0,
			}, # e.g., Module::Name => 1.1
   ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
    (ABSTRACT_FROM  => 'lib/GeoStr.pm', # retrieve abstract from module
     AUTHOR         => 'Quinn Hart <qjhart@ucdavis.edu>') : ()),
  );
