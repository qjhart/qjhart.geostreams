# runkepler.sh
# Runs kepler with my project's class directory added in
# first place in the CLASSPATH defined in kepler.sh
# This is an ad hoc mechanism to run the demonstration
# geostreams workflows.

if [ -z "$KEPLER" ]; then
	KEPLER=/usr/local/kepler20070526
fi
echo "KEPLER = $KEPLER"

PROJECT_HOME=/home/carueda/workspace/viewer
CP=$PROJECT_HOME/_generated/BUILD/classes

JTS=/home/carueda/software/jts-1.8.0/lib/jts-1.8.jar
CP=$CP:$JTS

CIMIS=/home/carueda/workspace/CIMIS/cimis-download.jar
CP=$CP:$CIMIS

# The following adjustments according to kepler's nightly build 20070526:
#	1) KEP redefined to the $KEPLER value above
#	2) No "cd" command
#	3) Put my $CP in first place in CLASSPATH
cat $KEPLER/kepler.sh | \
	sed s#^KEP=.*#KEP=$KEPLER#g | \
	sed s/cd\ \$KEP//g | \
	sed s#CLASSPATH=#CLASSPATH=$CP:#g > ./_gs_kepler
	
sh ./_gs_kepler $*
