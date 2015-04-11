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
session_start();
// catch information from last page via session
$full_name=$_SESSION['full_name'];
$age=$_SESSION['age'];
$email=$_SESSION['email'];
$username=$_SESSION['username'];
$password=$_SESSION['password'];

//~ $_SESSION['full_name']=$full_name;
//~ $_SESSION['age']=$age;
//~ $_SESSION['email']=$email;
//~ $_SESSION['username']=$username;
//~ $_SESSION['password']=$password;

?>
<!-- fix can't go back to login page without FORM RESUBMISISON prompt -->
<!-- WHY ARE THERE SESSIONS AT THIS PAGE, WHEN REGISTER POSTS HERE -->
<!-- LOGIN NO LONGER SELF LOOPS, DONT NEED SESSIONS HERE; DONT EVNE USE THEM-->

<body>
	<table border="0" align="center" class="tcenter tcolor" style="margin-top:220px">
		<tr>
			<td>
				<form action="test_users.php" method="post">
					
					<!-- ERROR:
					if user goes to login.php after logging in, it shows "CONGRATS ON REGISTERING"
					 and empty password at that
					-->
					<?php
					if(isset($username)){	
						// get rid of this or fix it since no longer mock up
						echo "</td><td><p align=\"center\"> Congratulations, $full_name! </p>";
						echo "<p align=\"center\"> Your registration as user <b>$username</b> has completed successfully.</p>";
						echo "<p align=\"center\"> We know you're $age years old, your email is $email, and your password is $password.";
						echo "<p align=\"center\"> Nonetheless, please login to start using Wooter. </p></td><td>";
					}
					if(!isset($username)){ 
						echo "<h4><p align=\"center\"> Welcome to the login page.</p><p>Please enter your credentials below. </p></h4>";
					}
					?>

				</td>
			</tr>
		</table>
		<table border="0" align="center" class="tcenter tcolor"  style="margin-top:20px">
			<tr>
				<td>
					
					<?php
					echo "<p>Username: <input type=\"text\" name=\"username\"></p>";
					echo "<p>Password: <input type=\"password\" name=\"password\"></p>";
					echo "<p><input type=\"submit\" name=\"login_sub\" value=\"Login\"></p>";
					?>

				</form>
			</td>
		</tr>
	</table>
	<?php
	//~ session_unset();
	?>
</body>
</html>
