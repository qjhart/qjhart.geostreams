<?php
	// $Id: last.php,v 1.3 2005/02/10 08:01:29 crueda Exp $
	header("Content-type: text/html");
	$delay = $_GET["delay"];
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
?>

<html>
<head>
<title>Last GOES full disk snapshot</title>
<? if ( $delay ) { ?>
<META HTTP-EQUIV="Refresh" 
CONTENT="<?= $delay ?>; URL=last.php?delay=<?= $delay ?>&zoom=<?= $zoom ?>">
<? } ?>
</head>
<body bgcolor =" #ffffff">

<table border="0">
  <tbody>
    <tr>
	  <td>
	  </td>
	  <td>
	  	<b>Last GOES full disk snapshot</b>
	  </td>
	</tr>
    <tr>
      <td valign="top">
	  	<img alt="GeoStreams Logo" border="0" src="gvar-left-rotated.png">
      </td>
      <td valign="top">
		<form action="last.php">
			<input name="cmd" type="submit" value="Refresh">

			&nbsp;&nbsp;&nbsp; Refresh rate 
			<input name="delay" type="input" size="3" value="<?= $delay ?>">
			&nbsp; seconds

			&nbsp;&nbsp;&nbsp; Zoom
			<input name="zoom" type="input" size="3" value="<?= $zoom ?>">
			&nbsp; %
		</form>

	  	<img alt="last snapshot" border="0"  src="redim.php?zoom=<?= $zoom ?>">
	  </td>
    </tr>
  </tbody>
</table>

</body>
</html>

