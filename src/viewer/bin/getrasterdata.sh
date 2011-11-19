MYDIR=`dirname $0`
. $MYDIR/setenv.sh

java $RDEBUG -ea -Xms60M -Xmx250M -classpath "$CP" geostreams.util.GetRasterData $@

