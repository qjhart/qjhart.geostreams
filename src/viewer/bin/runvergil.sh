MYDIR=`dirname $0`
. $MYDIR/setenv.sh

CP=$CP:$PTII

echo CP=$CP
export CLASSPATH=$CP

java $RDEBUG -ea -Xms60M -Xmx512M geostreams.ptolemy.VergilUtils --openLib workflows/geostreamslib.xml $@
#java $RDEBUG -ea -Xms60M -Xmx250M ptolemy.vergil.VergilApplication $@
