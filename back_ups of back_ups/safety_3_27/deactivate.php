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
include "user_mgmt.php";
session_start();
$_SESSION['deactivate']=$_POST['deactivate'];
$home_user=$_SESSION['user_obj'];

if(isset($_SESSION['deactivate'])){
	$home_user->free_bit="d";
	$u_file="users/u_".(int)($home_user->id/$USERS_PER_FILE).".txt";
	$user_base=fopen($u_file, "r+") or die("Could not open file: ".$u_file);
	flush_user($user_base, $home_user, ($home_user->id%$USERS_PER_FILE)*$MAX_ULINE_LEN);
	fclose($user_base);
}
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
				show(" <font size='8'> :((( We're sad to see you go. <br> The next time you login your account will reactivate. </font>");
				?>
			</td>
			<?php
			show("</tr><tr><td><p><font size='4'> Your account will be regrettably removed from the system within 7 to 14 days. </font></p></td>");
			echo "<tr><td>Please <a href=\"#\"' onclick=\"history.back();\"> click here</a> to go back.</td>";  
			if(isset($_SESSION['deactivate'])){
				session_unset();
				session_destroy();
			}
			?>
		</tr>
	</table>
	
</body>
</html>
