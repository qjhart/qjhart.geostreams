JAVA_HOME=/usr/local/jdk1.5.0_10
PATH=$JAVA_HOME/bin:$PATH
export PTII=/usr/local/ptII6.0.beta

# Runs my VergilUtils which processes the --openLib option
# and calls VergilApplication

PROJECT_HOME=/home/carueda/workspace/viewer
CP=$PROJECT_HOME/_generated/BUILD/classes

export CLASSPATH=$CP

mkdir -p tmp

cat $PTII/bin/vergil | sed \
	s/ptolemy.vergil.VergilApplication/geostreams.ptolemy.VergilUtils/g > tmp/vergil
	
sh tmp/vergil $@
