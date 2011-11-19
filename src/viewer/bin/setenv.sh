JAVA_HOME=/usr/local/jdk1.5.0_10
PATH=$JAVA_HOME/bin:$PATH

#PTII=/home/carueda/software/Ptolemy/ptII5.0.1


#export PTII=/usr/local/ptII6.0.beta

export PTII=/home/carueda/workspace/ptII/bin

PATH=$PTII/bin:$PATH

PROJECT_HOME=/home/carueda/workspace/viewer
CP=$PROJECT_HOME/_generated/BUILD/classes

JTS=/home/carueda/software/jts-1.8.0/lib/jts-1.8.jar
CP=$CP:$JTS

CIMIS=/home/carueda/workspace/CIMIS/cimis-download.jar
CP=$CP:$CIMIS

export CLASSPATH=$CP

RDEBUG=
if [ "$1" == "+rdebug" ]; then
	shift
	RDEBUG="-Xdebug  -Xnoagent -Xrunjdwp:transport=dt_socket,address=2358,server=y,suspend=y"
fi
