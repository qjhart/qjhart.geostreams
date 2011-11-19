<?php
	// $Id: control.php,v 1.5 2005/02/10 06:16:04 crueda Exp $
	header("Content-type: text/html");
	$cmd = $_GET["cmd"];
?>

<html>
<head>
<title>gvnetapi process control</title>
</head>
<body bgcolor =" #ffffff">

<table border="0">
  <tbody>
    <tr>
	  <td>
	  </td>
	  <td>
	  	gvnetapi process control
	  </td>
	</tr>
    <tr>
      <td>
	  	<img alt="GeoStreams Logo" border="0" 
			src="gvar-left-rotated.png">
      </td>
      <td valign="top">
	  
		<form action="control.php" >
			<table border="1">
			  <tbody>
				<tr>
				  <td>
					current gvnetapi processes: 
						<input name="cmd" type="submit" value="Refresh"><br>
					<pre><?php
						if ( $cmd == "Killall" ) {
							system('/usr/bin/killall gvnetapi ', $retval);
							sleep(2);
						}
						system('/bin/ps -e | grep gvnetapi', $retval);
					?>
					</pre>
					<br>
				  </td>
				</tr>
				<tr>
				  <td>
					<b>want to kill them all?</b>
					<br>
					Try to stop the applet first.
					<br>
					<input name="cmd" type="submit" value="Killall"> <br>
				  </td>
				</tr>
			</tbody>
			</table>
		</form>
	  
	  </td>
    </tr>
  </tbody>
</table>

</body>
</html>

