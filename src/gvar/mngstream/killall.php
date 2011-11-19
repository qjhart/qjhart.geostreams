<?php
	header("Content-type: text/html");
	echo '<pre>';
	echo 'killall gvnetapi';
	system('/usr/bin/killall gvnetapi', $retval);
?>
</pre>

<a href="ps.php" target=control>
Refresh</a>

