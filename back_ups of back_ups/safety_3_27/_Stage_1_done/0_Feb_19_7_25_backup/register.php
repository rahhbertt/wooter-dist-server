<doctype! html>
<html>
<head>
	<title> ERROR: REGISTRATION FAILED </title>
	<meta charset="UTF-8">
	<style>
		body{
			background-image: url("wooter.png"), url("wooter.png");
			background-position: top, right;
			background-repeat: repeat-x, repeat-y;
			text-align: center;
			background-attachment: fixed;
		}
		.tcenter { text-align: center; }
		.tcolor { background: rgba(0,255,255,0.4); }
		.button:hover { background: cyan; cursor: pointer; }
	</style>
</head>
<body>


	<?php
	session_start();

// take in all variables from form on main page
	$full_name=$_POST["first_name"] . ' ' . $_POST["last_name"];
	$age=$_POST["age"];
	$email=$_POST["email"];
	$username=$_POST["username"];
	$password=$_POST["password"];
	$re_password=$_POST["re_password"];

// handle any html special characters so input can't insert html code
	$full_name = htmlspecialchars($full_name);
	$age= htmlspecialchars($age);
	$email = htmlspecialchars($email);
	$username = htmlspecialchars($username);
	$password = htmlspecialchars($password);
	$re_password = htmlspecialchars($re_password);
	?>


	<table border="0" align="center" class="tcenter tcolor" style="margin-top:230px">
		<tr>
			<td>
				<?php
// check for any empty fields once not html code
				// POST used since didnt store names separately here
				if( empty($_POST['first_name']) ) { echo "<p>Please fill in your first name.</p>"; }
				if( empty($_POST['last_name']) ) { echo "<p>Please fill in your last name.</p>"; }
				if( empty($age) ) { echo "<p>Please fill in your age.</p>"; }
				if( empty($email) ) { echo "<p>Please fill in your email addrses.</p>"; }
				if( empty($username) ) { echo "<p>Please choose a username.</p>"; }
				if( empty($password) ) { echo "<p>Please choose a password.</p>"; }
				else if( empty($re_password) ) { echo "<p>Please re-enter your password on the previous page..</p>"; }

				if( empty($_POST["first_name"])|| empty($_POST["last_name"]) || empty($age) || empty($email) || empty($username) || empty($password) || empty($re_password) ){
					go_back("");
					exit;
				}
// if no errors, go to login.php
				if(check_errors($full_name, $age, $email, $username, $password, $re_password)){
					$_SESSION['full_name']=$full_name;
					$_SESSION['age']=$age;
					$_SESSION['email']=$email;
					$_SESSION['username']=$username;
					$_SESSION['password']=$password;
					header("Location: login.php");
				}
				?>
			</td>
		</tr>
	</table>


	<?php

	function go_back($string){
		echo "<p></p>Please <a href=\"#\"' onclick=\"history.back();\"> try again</a>$string. </p>";  
	}


	function check_errors($full_name, $age, $email, $username, $password, $re_password){
		if($age<13) { 
			echo "We're sorry, $full_name, but you must be at least 13 to create an account with Wooter."; 
		}
		else if( strpos($email, '@') == FALSE ) { 
			go_back(" and enter a valid emaill address");
		}
		else if  (strlen($username) < 8  || strlen($username)>24 ) {
			echo "<p> Your username must be between 8 and 24 characters.</p>";
			go_back("");
		}
		else if( $password != $re_password ) {
			echo "<p> The passwords you have entered do not match.</p>";
			go_back("");
		}
		else if ( strlen($password) <10 ) {
			echo "<p> Your password must be between 10 and 32 characters.</p>";
			go_back("");
		}
		else { return true; }
		return false;
	}
	?>

</body>
</html>
