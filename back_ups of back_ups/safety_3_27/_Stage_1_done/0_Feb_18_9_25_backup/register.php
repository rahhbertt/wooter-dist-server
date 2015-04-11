<doctype! html>
<html>
<body>

<?php
session_start();
include 'includes.php';

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
	go_back("");
	exit;
}

// apparently PHP handles destruction by scope itself?

// only MAKE the user object if no errors
if(check_errors($full_name, $age, $email, $username, $password, $re_password)){
	$new_user= new user();
	$new_user->full_name=$full_name;
	$new_user->age=$age;
	$new_user->email=$email;
	$new_user->username=$username;
	$new_user->password=$password;
	register_user($new_user);
	$_SESSION['registered']=true;
	$_SESSION['new_user']=$new_user;
	header("Location: login.php");
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
	else if( exists_user($username) ){
		echo "<p> The username <b>$username</b> is already taken.";
		go_back(" with a different username");
	}
	// add upper limit for age as well?
	// add password security requirements
	// if the email address is already beign used, dont let it make anotehr account
	// password cannot match username
	// user name must not already exist
	// make sure can't enter blank field for any of them, or file gets all messed up
	else { return true; }
	return false;
}


// without a database, this is the best we can do for a small website


function register_user($new_user){
	$file_name="/var/www/html/user_base.txt";
	$needs_header= !(file_exists($file_name));
	$user_base=fopen($file_name, 'a') or die("Unable to open file: $file_name!");
	// with serialization the header becomes dificult
	//if($needs_header) { fwrite($user_base, "Full Name" . "\t\t" . "Age" . "\t\t" . "Email Address" . "\t\t" . "Username" . "\t\t" . "Password" . "\t\t" . PHP_EOL); }
	// for some reason the PHP_EOL appends an empty space as well to the password field
	// making authenticaiton problems
	$serial_user=serialize( $new_user );
	fwrite($user_base, $serial_user);
	fwrite($user_base, PHP_EOL);
	// can't use EOL with serialization???
	//fwrite($user_base, $full_name . "\t\t" . $age . "\t\t" . $email . "\t\t" .  $username . "\t\t" . $password . "\t\t" . PHP_EOL);
	fclose($user_base);

}


// near end of project, create error page if somebody just tries to reload a php page ??
// if really bored figure out how to force a character limit or something
// so that the columns in the suer file actually line up
// and it doesnt mess up the exploding each line by the same char "\t\t"
?>

</body>
</html>
