<doctype! html>
<html>
<head>
	<style>
		body{
			background-image: url("wooter.png"), url("wooter.png"), url("blue.jpg");
			background-position: top, left, bottom -40px right -50px;
			background-repeat: repeat-x, repeat-y, no-repeat;
			text-align: center;
			background-attachment: fixed;
		}
		.tcenter { text-align: center; }
		.tcolor { background: rgba(255,255,255,0.8); }
		.button:hover { background: cyan; cursor: pointer; }
	</style>
	<title> 
		Register An Account with Wooter
	</title>
	<meta charset="UTF-8">
</head>

<?php
include "user_mgmt.php";
session_start();
// catch information from register.php via session
$full_name=$_SESSION['full_name'];
$age=$_SESSION['age'];
$email=$_SESSION['email'];
?>

<body>
	<table border="0" align="center" class="tcenter tcolor" style="margin-top:220px">
		<tr>
			<td>
				<form action="login.php" method="post">
					
					<?php
					if(isset($_POST['login_sub'])){
						$username=strtolower($_POST['username']); // just attempted login
						$password=$_POST['password'];

						$user_obj = new User;
						$user_obj = read_user($username);
						if(isset($user_obj->username)){
							if( $user_obj->password != $_POST['password'] ) {
								echo "<p>You have entered in an incorrect password. Please go <a href=\"login.php\">back</a> and try again.</p>";
							} else { 
								$_SESSION['user_obj']=$user_obj;
								header("Location: home.php");
							}
						} else {
							echo "<p>".$_POST['username']." is not a valid username. Please go <a href=\"login.php\">back</a> and try again.</p>";
						}
					} 
					else {
						$username=strtolower($_SESSION['username']); 
						$password=$_SESSION['password'];
					}
					if(isset($_SESSION['registered_ok'])){	
						echo "</td><td><p align=\"center\"> Congratulations, $full_name! </p>";
						echo "<p align=\"center\"> Your registration as user <b>$username</b> has completed successfully.</p>";
						unset($_SESSION['registered_ok']);
					}
					else if (!isset($_POST['username'])){ 
						echo "</td></tr><tr><td><h4><p align=\"center\"> Welcome to the login page.</p><p>Please enter your credentials below. </p></h4></tr></td>";
					}
					?>

				</td>
			</tr>
		</table>
		<table border="0" align="center" class="tcenter tcolor"  style="margin-top:20px">
			<tr>
				<td>

					<?php
					if(!isset($_POST['login_sub'])){
						echo "<p>Username: <input type=\"text\" name=\"username\"></p>";
						echo "<p>Password: <input type=\"password\" name=\"password\"></p>";
						echo "<p><input type=\"submit\" name=\"login_sub\" value=\"Login\"></p>";
					}
					?>

				</form>
			</td>
		</tr>
	</table>
</body>
</html>