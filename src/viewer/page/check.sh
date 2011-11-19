#
# check.sh: checks that data is being received from the
# geostreams server.
# Carlos Rueda - 2007-07-19
# Version $Id: check.sh,v 1.5 2007/08/01 18:18:40 crueda Exp $
#
# This script launches the data download of one of the GOES channels
# from the geostreams server. Every 15s (parameter DELAY), it checks
# the size of the file being downloaded. If the size is big enough
# (MINSIZE), the check is successful. This check is performed for a
# maximum number of times (ATTEMPTS).
# A log file is updated and an html file with a summary of the result is created.
#
# NOTES: 
#   - Although this script can be run anytime, it is mainly designed to be
#     run via a crontab. So, avoid having two processes running simultaneously.
#
#   - TODO: upon the first error, maybe create a control file and make the script
#     return silently until the control file is removed.
#
#   - perhaps using the wget options -Q and -i would make this script simpler;
#     unfortunately, these options are not working as I would expect.

# Number of attempts
ATTEMPTS=60

# In each attempt, we sleep this time while WGET is working
DELAY=15s

# So, the total wait would be about 15min, and hence the period to 
# run the cronjob should be greater that 15min, say 20min, eg.:
#    0,20,40 * * * *       check.sh

# minimum size of downloaded file to determine everything seems OK
# 300 seems enough
MINSIZE=300

# we use wget to download data
WGET=/usr/bin/wget

# Working directory:
WORKDIR=/home/carueda/public_html/geostreams

# this is the page with a summary of the results;
CHKADDRESS="http://geostreams.ucdavis.edu/~carueda/geostreams/check.html"

# location of html report to be generated
CHKHTML=$WORKDIR/check.html

# stream to download:
STREAM="http://geostreams.ucdavis.edu:9060/geostream?format=1&channels=2&nodata"

# name of the downloaded file
OUTFILE=$WORKDIR/stream.bin

# the log file updated by this script
SLOG=check.log
LOG=$WORKDIR/$SLOG

# the log file updated by WGET
SWGETLOG=wget.log
WGETLOG=$WORKDIR/$SWGETLOG

# prints a log message to $LOG
log() {
	msg=$1
	echo "`date --iso=seconds`: $msg" >> $LOG
}

# prints a text to $CHKHTML
html() {
	msg=$1
	echo "$msg" >> $CHKHTML
}

filesize() {
	file=$1
	ls -l $file | awk '{ print $5 }'
}

# creates a .bak of a file if its is greater than a max size
rotatelog() {
	log=$1
	maxsize=$2
	if [ -f $log ]; then
		size=`filesize $log`
		test $size -gt $maxsize
		if [ $? = 0 ]; then
			mv $log $log.bak
			> $log
		fi
	fi
}


# --------------------------------------
# start working:

rotatelog $LOG 5000
rotatelog $WGETLOG 15000

# launch WGET in background and get PID
PID=`$WGET -O $OUTFILE -a $WGETLOG -b $STREAM | sed "s/.*pid.\(.*\)\./\1/g"`
log "wget launched. PID = $PID"

EXITCODE=0
ATTEMPT=
SIZE=

if [ $? != 0 ]; then
	log "ERROR: wget returned $?"
	EXITCODE=$?
else
	for ATTEMPT in `seq 1 $ATTEMPTS`; do
		# let WGET work for some time:
		log "attempt=$ATTEMPT   Sleeping for $DELAY"
		sleep $DELAY
		
		# check size of OUTFILE
		SIZE=`filesize $OUTFILE`
		test $SIZE -gt $MINSIZE
		if [ $? = 0 ]; then
			break
		fi
	done

	# kill WGET
	kill $PID
	log "kill $PID ==> $?"

	# check size of OUTFILE
	SIZE=`filesize $OUTFILE`
	test $SIZE -lt $MINSIZE
	if [ $? = 0 ]; then
		log "Result: ERROR: file size $SIZE < minsize $MINSIZE"
		EXITCODE=55
	else
		log "Result: OK: file size $SIZE >= minsize $MINSIZE"
	fi
fi

echo "" >> $LOG

# update html file
echo "<html>" > $CHKHTML
html "<head> <title>geostreams server check `date --iso=seconds`</title> </head>"
html "<body>"
html "<tt>`date` <br>"
html "Result of check: "
if [ $EXITCODE != 0 ]; then
	html "<font color=red><strong>ERROR</strong></font>"
	html "<br> File size $SIZE &lt; minsize $MINSIZE"
else
	html "<font color=green><strong>OK</strong></font>"
	html "<br> File size $SIZE &gt;= minsize $MINSIZE after attempt $ATTEMPT"
fi
html "</tt>"
html "<hr/>"
html "<a href=$SLOG>$SLOG</a> - <a href=$SLOG.bak>$SLOG.bak</a><br>"
html "<a href=$SWGETLOG>$SWGETLOG</a> - <a href=$SWGETLOG.bak>$SWGETLOG.bak</a><br>"
html "<hr/>"
html "Check parameters: <tt> ATTEMPTS=$ATTEMPTS DELAY=$DELAY MINSIZE=$MINSIZE </tt>"
html "<hr/>"
html "Crontab: <tt>`crontab -l | grep geostreams/check.sh`</tt>"
html "</body> </html>"

# if error, output some info
if [ $EXITCODE != 0 ]; then
	# to send this info in the email by cron:
	tail $LOG
	echo
	echo $CHKADDRESS
fi

exit $EXITCODE
