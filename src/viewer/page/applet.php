<?php
	// $Id: applet.php,v 1.12 2005/07/19 22:26:30 crueda Exp $
	
	$codebase = "http://geostream.cs.ucdavis.edu/~carueda/applet";
	
	// this URL provides the list of available streams
	$streams = "streams.txt";
	
	// Name of saved file with complete image
	$saved = "saved_vis.jpg";
	
	// Name of file containing some geograhic lines
	$geolines = "geolines.txt";
	
	// open applet in a new window?
	$newframe = "yes";
	
	if ( $newframe == "yes" ) {
		$width = 600;
		$height = 40;
	}
	else {
		$width = 850;
		$height = 625;
	}
?>

<html>
<head>
<title> Geostreams Viewer Applet </title>
</head>
<body bgcolor =" #ffffff">

<!-- <table style="margin-left: auto; margin-right: auto;" border="0"> -->

<table border="0">
  <tbody>
    <tr>
	  <td>
	  </td>
	  <td>
	  	<b>Real-time Geostreams Visualization</b>
	  </td>
	</tr>
    <tr>
      <td style="vertical-align: top;">
	  	<img alt="GeoStreams Logo" border="0" src="img/gvar-left-rotated.png">
      </td>
	  
      <td  style="width: 600px;text-align: justify;vertical-align: top;">
		
	  <br>
		<applet
		 codebase=<?= $codebase ?>
		 code="edu.ucdavis.geostreams.applet.GeostreamsApplet.class" 
		 archive="geostreams.jar" 
		 width=<?= $width ?> height=<?= $height ?> >
			<param name="newframe" value="<?= $newframe ?>" >
			<param name="streams" value="<?= $streams ?>" >
			<param name="saved" value="<?= $saved ?>" >
			<param name="geolines" value="<?= $geolines ?>" >
		</applet>
		
	  <br>
	  <br>
	  
	  The Geostreams Viewer will allow you to visualize incoming data
	  from the GOES West satellite in real time. You can choose the
	  channels you want to visualize. All open channels are updated
	  continuously with the data sent by the Geostreams server. Each
	  channel is rendered in a tab. The status line will show
	  information about the current selected channel (the number of
	  rows received).
	  
	<table border="0" style="text-align: left;">
	  <tbody>
		<tr>
		  <td>
			  <b>Commands</b>
			  <ul>
				<li>Drag the mouse to pan over.</li>
				<li>Use the slider to change the zoom factor.</li>
				<li>Check the "Geo lines" box to see geograhic lines</li>
				<li>Check the "Frame" box to see rectangles for received
				    sectors and the current scan line indicator</li>
			  </ul>
		   </td>
		  <td>
			  <img src="img/snapshot0.png" alt="Geostreams Viewer"
			   width="100%">
		  </td>
		 </tr>
		  </tbody>
		</table>
	  ...
	  
	  <br>
	  <br>
	  TODO:
	  <ul>
		<li>show more related metadata
		<li>combine 3 channels into an RGB image
	  </ul>
	  </td>
    </tr>
  </tbody>
</table>

</body>
</html>


