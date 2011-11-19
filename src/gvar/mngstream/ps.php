<?php
	header("Content-type: text/html");
	echo '<pre>';
	echo 'ps -e | grep gvnetapi<br> ';
	system('/bin/ps -e | grep gvnetapi', $retval);
?>
</pre>

<a href="killall.php">
Click here to kill ALL current gvnetapi processes</a>


