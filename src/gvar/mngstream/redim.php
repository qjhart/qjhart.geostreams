<?php
// $Id: redim.php,v 1.2 2005/02/10 05:52:48 crueda Exp $
header("Content-type: image/jpeg");
$zoom = $_GET["zoom"];

if ( !$zoom ) {
	$zoom = 20;
}
if ( $zoom < 5 ) {
	$zoom = 5;
}
if ( $zoom > 100 ) {
	$zoom = 100;
}

$home = dirname(__FILE__);
$imgname = $home . "/saved_vis.jpg";

$im = @imagecreatefromjpeg($imgname);
if ($im) {
	$oldw = 2000;
	$oldh = 1777;
	$oldx0 = 230;
	$oldy0 = 240;
	$oldx1 = $oldw - $oldx0;
	$oldy1 = $oldh - $oldy0;
	$neww = $zoom * ($oldx1 - $oldx0) / 100;
	$newh = $zoom * ($oldy1 - $oldy0) / 100;
	$im2 = imagecreate($neww,$newh);
	imagecopyresized($im2, $im, 0,0, $oldx0,$oldy0, $neww,$newh, ($oldx1 - $oldx0),($oldy1 - $oldy0));
	imagejpeg($im2);
	imagedestroy($im2);
}
else {
	$im  = imagecreate(150, 30);
	$bgc = imagecolorallocate($im, 255, 255, 255);
	$tc  = imagecolorallocate($im, 0, 0, 0);
	imagefilledrectangle($im, 0, 0, 150, 30, $bgc);
	imagestring($im, 1, 5, 5, "Error loading $imgname", $tc);
}
imagedestroy($im);

?>
