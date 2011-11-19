<?php
	header("Content-type: video/x-mng");
	$iters = $_GET['iters'];
	$delay = $_GET['delay'];
	$home = "/var/lib/gforge/chroot/home/users/crueda/public_html/mng/";
	passthru($home . 'pgms2mng - ' . $iters . ' ' . $delay 
	. ' lena.pgm lena200x200.pgm');
?>
