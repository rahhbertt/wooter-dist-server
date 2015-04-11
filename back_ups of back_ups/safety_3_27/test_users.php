<doctype! html>
<html>
<head></head>
<body>
	<?php
	include "user_mgmt.php";
	$user_obj = new User;
	$user_obj = read_user($_POST['username']);
	if(isset($user_obj->username)){
			//~ print_r($user_obj);
		if( $user_obj->password != $_POST['password'] ) {
			echo "<p>You have entered in an incorrect password. Please go <a href=\"login.php\">back</a> and try again.</p>";
		}
		else { echo "correct"; }
	}
	else {
		echo "<p>".$_POST['username']." is not a valid username. Please go <a href=\"login.php\">back</a> and try again.</p>";
	}
//~ // do htmlspecialchars, see if it covers spaces and whatnot
	//~ <!-- FOR PURE PHP FILES, LIKE user_mgmt, should I include this head, body stuff? -->
// DONT USE SESSIONS, HAVE REGISTER.PHP CREATE THE USER
// LOGIN JUST USERNAME, LOOK IT UP, AND GET INFO FROM THERE

	//~ print_r($_POST); 	echo "<br>";
	//~ print_r($_SESSION);	echo "<br><br>";

	//~ if(!isset($user_obj->id)){
		//~ echo "did not find user pre-existing<br>";
		//~ $user_obj->full_name=$_SESSION['full_name'];
		//~ $user_obj->age=$_SESSION['age'];
		//~ $user_obj->email=$_SESSION['email'];
		//~ $user_obj->username=$_POST['username'];
		//~ $user_obj->password=$_POST['password'];
//~ 
		//~ create_new_user($user_obj);
		//~ echo "what".$user_obj->id."\ntest<br>";
	//~ }
	//~ else { echo "Yaaa"; }
?>


</body>
</html>
