<?php
	// $Id: test.php,v 1.1 2004/06/03 20:26:28 crueda Exp $
	header("Content-type: text/html");
	$id = $_GET["id"];
?>

<html>
<body bgcolor =" #ffffff">

<?php if ( $id == "1" ) { ?>

	<b> pending </b>

<?php } else { ?>

	<applet code="TestApplet.class" codebase="classes/" 
		width="200" height="50">
	</applet>

<?php } ?>
	
	
</body>
</html>


