<doctype! html>
<html>
<head></head>
<body>
	<!-- FOR PURE PHP FILES, LIKE user_mgmt, should I include this head, body stuff? -->
	<?php
	include "user_mgmt.php";
	session_start();
// catch information from last page via session
// FIGURE OUT SESSION IDS??
// DONT USE SESSIONS, HAVE REGISTER.PHP CREATE THE USER
// LOGIN JUST USERNAME, LOOK IT UP, AND GET INFO FROM THERE
	print_r($_POST);
	$user_obj = new User;
	$user_obj = read_user($_POST['username']);
	print_r($user_obj);
	if($user_obj->password==$_POST['password']){
		$_SESSION['logged_user_obj']=$user_obj;
		echo "<br> MADE IT <br>";
		header("Location: home.php");
	}
	else{
		echo "incorrect password.";
	}
	if(!isset($user_obj->id)){
		echo "did not find user pre-existing<br>";
		$user_obj->full_name=$_SESSION['full_name'];
		$user_obj->age=$_SESSION['age'];
		$user_obj->email=$_SESSION['email'];
		$user_obj->username=$_SESSION['username'];
		$user_obj->password=$_SESSION['password'];

		create_new_user($user_obj);
		echo "what".$user_obj->id."\ntest<br>";
	}
	else { echo "Yaaa"; }
	?>


</body>
</html>
