<?php
	// $Id: applet.php,v 1.14 2005/02/10 05:52:48 crueda Exp $
	
	// this URL provides the MNG stream
	//$url = "http://casil.ucdavis.edu/~crueda/goes/goes";
	$url = "http://geostream.cs.ucdavis.edu/applet/goes.php";
	
	// Name of saved file with complete image
	$saved = "saved_vis.jpg";
	
	// open applet in a new window?
	$newframe = "no";
	
	// region for the applet (should be 1x1 if $newframe="yes")
	$width = 750;
	$height = 525;
?>

<html>
<head>
<title> GOES Stream Applet </title>
</head>
<body bgcolor =" #ffffff">

<table border="0">
  <tbody>
    <tr>
	  <td>
	  </td>
	  <td>
	  	GOES Stream Applet
	  </td>
	</tr>
    <tr>
      <td>
	  	<img alt="GeoStreams Logo" border="0" 
			src="gvar-left-rotated.png">
      </td>
      <td>
		<applet code="geostreams.mngstream.applet.MngViewerApplet.class" 
		 archive="DIST/mngstream.jar" 
		 width=<?= $width ?> height=<?= $height ?> >
			<param name="newframe" value="<?= $newframe ?>" >
			<param name="url" value="<?= $url ?>" >
			<param name="saved" value="<?= $saved ?>" >
		</applet>
	  </td>
    </tr>
  </tbody>
</table>

</body>
</html>


