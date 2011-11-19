#
# Stript to make geostreams viewer source distribution
# $Id: mksrcdist.sh,v 1.4 2005/07/13 02:40:09 crueda Exp $
#
if [ -z $1 ]; then
	echo "USAGE:  mksrcdist.sh <VERSION>"
	exit 1
fi
VERSION=$1
if [ "$VERSION" != `cat VERSION` ]; then
	echo "VERSION parameter must be equal to VERSION file contents"
	exit 2
fi
tag=`echo release-$VERSION | tr -t . _`

echo "Make tag [y/-]?:  cvs -q tag $tag"
read dotag

# $cvsroot: used to export the source from cvs
# (should be anonymously...
#cvsroot=":pserver:anonymous@cvs.casil.ucdavis.edu:/cvsroot/geostreams"
# but that's not working, so I'll be using my developer account for now:)
cvsroot=":ext:crueda@cvs.casil.ucdavis.edu:/cvsroot/geostreams"

(test "$dotag" != "y" || cvs -q tag $tag) &&\
mkdir -p DISTDIR &&\
cd DISTDIR &&\
cvs -d$cvsroot -q export -r $tag -d geostreams-viewer-$VERSION src/viewer &&\
tar cf geostreams-viewer-$VERSION.tar geostreams-viewer-$VERSION &&\
gzip -9 geostreams-viewer-$VERSION.tar &&\
rm -rf geostreams-viewer-$VERSION &&\
ls -l &&\
echo "Done."

