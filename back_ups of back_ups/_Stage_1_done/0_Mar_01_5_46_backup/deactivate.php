<doctype! html>
<html>

<head>
	<title> Deactivate Your Account </title>
	<meta charset='UTF-8'>
	<style>
		body {
			background: url(invert.png);
			background-repeat: repeat-all;
			text-align: center;
			background-position: right;
			background-attachment: fixed;
		}
		.tcenter { text-align: center; }
		.tcolor { background: rgba(255,255,255,0.8); }
		.tbcolor { background: rgba(0,255,255,0.8); }
		.button:hover { background: cyan; cursor: pointer; }
	</style>
</head>
<?php
session_start();
$_SESSION['deactivate']=$_POST['deactivate'];

function hide($string){
	if(!isset($_SESSION['deactivate'])){
		echo $string;
	}	
}
function show($string){
	if(isset($_SESSION['deactivate'])){
		echo $string;
	}	
}
?>
<body>

	<table style='margin-top:150px' align='center' class='tcenter tcolor' border='1'>
		<tr>
			<td>
				<?php
				hide("<font size='8'>
					Are you sure you want to do that?
				</font>
				<p>
					<form action='deactivate.php' method='post'>
						<input type='submit' name='deactivate' value='Click here to deactivate'>

					</form>
				</p>");
				show(" <font size='8'> :((( We're sad to see you go. </font>");
				?>
			</td>
			<?php
			show("</tr><tr><td><p><font size='4'> Your account will be regrettably removed from the system within 7 to 14 days. </font></p></td>");
			echo "<tr><td>Please <a href=\"#\"' onclick=\"history.back();\"> click here</a> to go back.</td>";  
			?>
		</tr>
	</table>
	
</body>
</html>