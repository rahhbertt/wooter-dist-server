<doctype! html>
<html>
<body>

<?php
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

if( empty($_POST["first_name"]) ) { echo "<p>Please fill in your first name.</p>"; }
if( empty($_POST["last_name"]) ) { echo "<p>Please fill in your last name.</p>"; }
if( empty($age) ) { echo "<p>Please fill in your age.</p>"; }
if( empty($email) ) { echo "<p>Please fill in your email addrses.</p>"; }
if( empty($username) ) { echo "<p>Please choose a username.</p>"; }
if( empty($password) ) { echo "<p>Please choose a password.</p>"; }
else if( empty($re_password) ) { echo "<p>Please re-enter your password on the previous page..</p>"; }


if( empty($_POST["first_name"])|| empty($_POST["last_name"]) || empty($age) || empty($email) || empty($username) || empty($password) || empty($re_password) ){
	echo "<p> Please <a href=\"#\"' onclick=\"history.back();\"> try again. </a> </p>";
	exit;
}

check_errors($full_name, $age, $email, $username, $password ,$re_password);

function check_errors($full_name, $age, $email, $username, $password, $re_password){
	if($age<13) { 
		echo "We're sorry, $full_name, but you must be at least 13 to create an account with Wooter."; 
	}
	else if( strpos($email, '@') == FALSE ) { 
		echo "Please go <a href=\"#\"' onclick=\"history.back();\">and enter a valid email address."; 
	}
	else if (strlen($username) < 10 ) {
		echo "<p> Your username must be between 10 and 32 characters.</p>";
		echo "<p> Please <a href=\"#\"' onclick=\"history.back();\"> try again. </a> </p>";
	}
	else if( $password != $re_password ) {
		echo "<p> The passwords you have entered do not match.</p>";
		echo "<p> Please go <a href=\"#\"' onclick=\"history.back();\"> back </a> and correct this.</p>";
	}
	else if ( strlen($password) <10 ) {
		echo "<p> Your password must be between 10 and 32 characters.</p>";
		echo "<p> Please <a href=\"#\"' onclick=\"history.back();\"> try again. </a> </p>";
	}
	else if( exists_user($username) ){
		echo "<p> The username <b>$username</b> is already taken.";
		echo "<p> Please <a href=\"#\"' onclick=\"history.back();\"> try again </a> with a different username. </p>";

	}
	// if the email address is already beign used, dont let it make anotehr account
	// password cannot match username
	// user name must not already exist
	// make sure can't enter blank field for any of them, or file gets all messed up
	else {
		register_user($full_name, $age, $email, $username, $password);
		echo "<p> Congratulations, $full_name! </p>";
		echo "<p> Your registration as user <b>$username</b> has completed successfully.</p>";
		echo "<p> Please <a href=\"login.php\"> login </a> to start using Wooter. </p>";
	}
}


// without a database, this is the best we can do for a small website
function exists_user($username) {
	$file_name="/var/www/html/user_base.txt";
	if(file_exists($file_name)) {
		$user_base=fopen($file_name, 'r') or die("Unable to open user file!");
		while(($line = fgets($user_base))) {
			$all_words=explode("\t", $line);
			if( $all_words[3] == $username) { return true; }
		}
	}
	return false;
}

function register_user($full_name, $age, $email, $username, $password){
	$file_name="/var/www/html/user_base.txt";
	$needs_header= !(file_exists($file_name));
	$user_base=fopen($file_name, 'a') or die("Unable to open file: $file_name!");
	if($needs_header) { fwrite($user_base, "Full Name" . "\t" . "Age" . "\t" . "Email Address" . "\t" . "Username" . "\t" . "Password" . PHP_EOL); }
	fwrite($user_base, $full_name . "\t" . $age . "\t" . $email . "\t" .  $username . "\t" . $password . PHP_EOL);
	fclose($user_base);

}


// near end of project, create error page if somebody just tries to reload a php page ??
// if really bored figure out how to force a character limit or something
// so that the columns in the suer file actually line up
// and it doesnt mess up the exploding each line by the same char "\t"
?>

</body>
</html>
