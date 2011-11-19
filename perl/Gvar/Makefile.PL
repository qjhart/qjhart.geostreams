use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile
  (
   'NAME'	=> 'Gvar',
   'OPTIMIZE'=>'-g',
   'VERSION_FROM' => 'lib/Gvar.pm', # finds $VERSION
#   'EXE_FILES'=>['bin/goes-get','bin/goes-slice'],
   'PREREQ_PM'		=> {PDL=>2.2},
   ($] >= 5.005 ?    ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM => 'lib/Gvar.pm', # retrieve abstract from module
       AUTHOR     => 'Quinn Hart <qjhart@ucdavis.edu> and Carlos Rueda <carueda@ucdavis.edu>') : ()),
    'LIBS'		=> '-lf2c -lm', # e.g., '-lm'
    'INC'		=> '', # e.g., '-I/usr/include/other',
  );
