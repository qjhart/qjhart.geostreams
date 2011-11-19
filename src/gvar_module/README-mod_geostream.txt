Updates regarding mod_gesotream
Carlos Rueda
$Id: README-mod_geostream.txt,v 1.2 2007/09/07 08:44:27 crueda Exp $

	Building mod_geostream
	Handling config parameters

Building mod_geostream
----------------------

This is a quick way to re-compile the geostream apache module (Yeah, it should
be made more clear... will do when time permits ;/ )

PATH=/usr/sbin:$PATH     #  for apxs

GVAR_INCLUDEDIR=../libgvar/ GVAR_LIBDIR=../libgvar/.libs/ make mod_geostream.so

# then, as root:

apxs -i -n 'geostream' mod_geostream.so

# however, this is creating /usr/lib/httpd/modules/mod_geostream.so (surely
# because of new apache version or sthg like that); but Jie's httpd is 
# reading modules from /opt/httpd/2.0.51/modules/
# So, while I get time to do a good update, i just copy the file, but
# first stop httpd:

cd ~jiezhang/
./httpd/bin/httpd stop

# copy the module:
cp /usr/lib/httpd/modules/mod_geostream.so  /opt/httpd/2.0.51/modules/

# and restart httpd as usual:
./httpd/bin/httpd start

# Done!  the new module is working! See my smiley in the getInfo request: ;-)

# wget -qO- "http://geostreams.ucdavis.edu:9060/geostream?getInfo"
CH 0 1 1 0.888889 10 Channel 1, 0.52-0.75microns - cloud cover  ;-)
CH 1 4 4 0.888889 10 Channel 2,   3.8-4.0microns - night cloud cover  ;-)
CH 2 4 8 0.888889 10 Channel 3,   6.4-7.0microns - water vapor  ;-)
CH 3 4 4 0.888889 10 Channel 4, 10.2-11.2microns - sea surface temp.  ;-)
CH 4 4 4 0.888889 10 Channel 5, 11.5-12.5microns - sea surface temp.  ;-)


Handling config parameters
--------------------------

Today, I also made adjustments so that the module can read parameters
from httpd.conf. It now reads GvarName and GvarPort, so these paramters are
no longer hard-coded in the module. These parameters can be placed anywhere
at top-level in httpd.conf.
