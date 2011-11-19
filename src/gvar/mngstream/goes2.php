<?php
	// $Id: goes2.php,v 1.3 2005/02/10 06:16:04 crueda Exp $
	header("Content-type: video/x-mng");
	$home = dirname(__FILE__);
	passthru($home . '/gvnetapi2 169.237.153.55 - 999999 2> /dev/null');
?>
