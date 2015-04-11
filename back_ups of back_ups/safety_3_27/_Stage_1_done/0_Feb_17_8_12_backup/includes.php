<doctype! html>
<html>
<body>

<?php

class user{
	public $full_name, $age, $email, $username, $password;
	public $followers, $followees;
	// add a pointer to a list of followers
	// add a pointer to a list of followees
}

function go_back($string){ echo "<p></p>Please <a href=\"#\"' onclick=\"history.back();\"> try again</a>$string. </p>";  }

function exists_user($username) {
	$file_name="/var/www/html/user_base.txt";
	if(file_exists($file_name)) {
		$file_handle=fopen($file_name, 'r') or die("Unable to open file: $file_name!");
		while( $user_base=fgets($file_handle) ) {
			$user_obj=unserialize($user_base);
			if( !empty($user_obj)){
				if($user_obj->username == $username) { return true; }
			}
		}
	}
	return false;
}

function verify_user($username, $password) {
	$file_name="/var/www/html/user_base.txt";
	if(file_exists($file_name)) {
		$file_handle=fopen($file_name, 'r') or die("Unable to open file: $file_name!");
		while( $user_base=fgets($file_handle) ) {
			$user_obj=unserialize($user_base);
			if( !empty($user_obj)){
				if($user_obj->username == $username) { 
						if($user_obj->password != $password){ 
							echo "You have entered an incorrect password.</p>";
							go_back("");
							return false;
						}
					return $user_obj;
				}
			}
		}
	}
	else {
		echo "The user database file is inacccessible.<br>";
		return false;
	}
	echo "<p> $username is not a valid username.</p>";
	go_back("");
	return false;
}

function read_user($username){
	$file_name="/var/www/html/user_base.txt";
	if(file_exists($file_name)) {
		$file_handle=fopen($file_name, 'r') or die("Unable to open file: $file_name!");
		while( $user_base=fgets($file_handle) ) {
			$user_obj=unserialize($user_base);
			if( !empty($user_obj)){
				if($user_obj->username == $username) { 
					return $user_obj;
				}
			}
		}
	}
}

function whose($other_user){
	if(!isset($other_user)) { 
		echo "Your";
	}
	else { 
		echo "$other_user->username's";
	}
}

?>

</body>
</html>
