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
	include "user_mgmt.php";
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
	$username = strtolower(htmlspecialchars($username));
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
					go_back("", "registration.html");
					exit;
				}
// if no errors, go to login.php
				if(!any_errors($full_name, $age, $email, $username, $password, $re_password)){
					$_SESSION['full_name']=$full_name;
					$_SESSION['age']=$age;
					$_SESSION['email']=$email;
					$_SESSION['username']=$username;
					$_SESSION['password']=$password;
					
					$user_obj = new User;
					$user_obj = read_user($_SESSION['username']);
					if(!isset($user_obj->id)){
						$user_obj->full_name=$_SESSION['full_name'];
						$user_obj->age=$_SESSION['age'];
						$user_obj->email=$_SESSION['email'];
						$user_obj->username=$_SESSION['username'];
						$user_obj->password=$_SESSION['password'];
						$_SESSION['registered_ok']=true;
						create_new_user($user_obj);
						header("Location: login.php");
					} else{
						echo "The username ".$_SESSION['$username']." is already taken. Please go <a href=\"registration.html\">back</a> and try again.";
					}

				}
				?>
			</td>
		</tr>
	</table>

	<?php

	function go_back($string, $location){
		echo "<p></p>Please <a href=\"". $location ."\"> try again</a>" . $string . ". </p>";  
	}


	function any_errors($full_name, $age, $email, $username, $password, $re_password){
		// the input fields limit the max lenghth anyhow, but just in case, or if I want to remove that for whatever reason
		global $MIN_USER_LEN, $MAX_USER_LEN, $MIN_PWD_LEN, $MAX_PWD_LEN;
		$errors=false;
		if($age<13) { 
			echo "<p> We're sorry,".$full_name.", but you must be at least 13 to create an account with Wooter. </p"; 
			$errors=true;
		}
		else if( strpos($email, '@') == FALSE ) { 
			echo "<p> Please enter a valid email address. </p>";
			$errors=true;
		}
		else if( $password != $re_password ) {
			echo "<p> The passwords you have entered do not match.</p>";
			$errors=true;
		}
// won't check full name since don't need a minimum there, one char is fine
		
		$errors=($errors || !correct_field_size("username", $username, $MIN_USER_LEN, $MAX_USER_LEN) );
		$errors=($errors || !correct_field_size("password", $password, $MIN_PWD_LEN, $MAX_PWD_LEN) );
		if($errors) { go_back("", "registration.html"); }
		return $errors;
	}

	function correct_field_size($field_name, $field, $min_len, $max_len){
		if ( strlen($field) < $min_len || strlen($field) > $max_len ){
			echo "<p> Your " . $field_name . " must be between " . $min_len . " and " . $max_len . " characters</p>";
			return false;
		}
		return true;
	}

	?>

</body>
</html>
