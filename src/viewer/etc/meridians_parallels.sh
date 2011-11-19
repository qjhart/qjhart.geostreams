#
# This script calls elug.line multiple times to generate 
# meridians and parallels
#
# Run:
#    ./geoline_parallels.sh
#

incr=5

meridians="-215 -195 -175 -155 -135 -115 -95 -75 -55"
parallels="-80 -70 -60 -50 -40 -30 -20 -10 0 10 20 30 40 50 60 70 80"

# meridians
for lon in $meridians ; do
	echo LINESTRING; 
	./elug.line $lon -75 $lon 75   0 $incr
done 

# parallels
for lat in $parallels ; do
	echo LINESTRING;
	./elug.line -250 $lat -20 $lat   $incr 0
done

