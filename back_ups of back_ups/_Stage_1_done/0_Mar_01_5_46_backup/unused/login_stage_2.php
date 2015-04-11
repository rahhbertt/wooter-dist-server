<doctype! html>
<html>
<body>

<?php
include 'includes.php';
session_start();
$registered=$_SESSION['registered'];
$new_user=$_SESSION['new_user'];

// should it be secured? how?
if(isset($_POST["login_sub"])){
	$username_sub=$_POST["username"];; 
	$password_sub=$_POST["password"];
	$username_sub = htmlspecialchars($username_sub);
	$password_sub = htmlspecialchars($password_sub);
	$valid_user=verify_user($username_sub, $password_sub);
	$_SESSION['logged_in_user']=$valid_user;
	if(!empty($valid_user)){ header("Location: home.php"); }
}	

else {
	if(isset($new_user->username)){	
		echo "<p align=\"center\"> Congratulations, $new_user->full_name! </p>";
		echo "<p align=\"center\"> Your registration as user <b>$new_user->username</b> has completed successfully.</p>";
		echo "<p align=\"center\"> Please login to start using Wooter. </p>";
	}
	echo "<form action = \"login.php\" method=\"post\">";
	if(!isset($new_user->username)){ 
		echo "<p> Welcome to the login page. Please enter your credentials below. </p>";
	}
	echo "<p>Username: <input type=\"text\" name=\"username\"></p>";
	echo "<p>Password: <input type=\"password\" name=\"password\"></p>";
	echo "<p><input type=\"submit\" name=\"login_sub\" value=\"Login\"></p>";
	echo "</form>";	
	session_unset();
}

// pop up windows would be nicer

// google php login to see secure ways to do it 

?>

</body>
</html>
