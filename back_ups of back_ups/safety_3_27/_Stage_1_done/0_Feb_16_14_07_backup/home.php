<doctype! html>
<html>


<?php
// INCLUDE OF CUSTOM CLASS MUST COME BEFORE SESSION START OR WONT KNOW HOW TO 
// DESERIALIZE IT
include 'includes.php';
session_start();
$logged_in_user=$_SESSION['logged_in_user'];
$_SESSION['logged_in_user']=$logged_in_user;
if(!isset($logged_in_user->username)){
	echo "Sorry, please  <a href=\"../login.php\"> login</a> and then use this page.";
	exit;
}
$other_username=$_GET['user'];
$other_user=read_user($other_username);
if(isset($other_user)){
	//echo "<br><br> YAAAAAAAAAAAAAAAAAAAAAAA";
	//echo "$other_user<br>Welcome to $other_user's Wooter Home Page.<br>";
	$current_user=read_user($other_user);
	print_r($current_user);
}
else { 
	//echo "<br><br> ZAAAAAAAAAAAAAAAAAAAAAAA <br><br>";
	$current_user=$logged_in_user;

	//echo "$current_user->username<br>Welcome to your Wooter Home Page";
	//print_r($logged_in_user);
}
// CANNOT ECHO SESSION VARS DIRECTLY
//print_r($_POST['logged_in_user']);

// LOG OUT BUTTON

// ADD THIS CHECK TO EACH PAGE ONLY FOR LOGGED INF OLK

?>



<head>
<style>
body {
	background: url(../wooter.png), url(../blue.jpg);
	background-repeat: repeat-y, no-repeat;
	text-align: center;
	background-position: right,  bottom left;

}
.left {
    float: left;
}

.right {
	float: right;
}

.submit {
	    float: left;
}
.submit:hover {
	    background: cyan;
	    cursor: pointer;
}

</style>
<?php
	if(!isset($other_user) && isset($other_username)){
		// somebody put in a ?user=jjohnson2 but the user no longer exists
		echo "<title> ERROR: User does not exist </title>";
		echo "<p>Sorry, but $other_username is not an active user account.</p>";
		echo "<p>That account either never existed or has been deleted by the user.</p>";
		echo "<p> Please click <a href=\"#\"' onclick=\"history.back();\"> here </a> to go back"; 
		exit;
	}
		// Wooter home page! </title> 
 ?>

	<meta charset="UTF-8"> 
</head>
<body>

      
<form class="submit" action="main.html" method="post">
       <?php echo "<a href=\"home.php\"> $logged_in_user->username </a>"; 
       echo "<input class=\"submit\" type=\"submit\" value=\"Log out\">";
       	echo "<title> Welcome to "; 
		whose();
		echo " Wooter Home Page!</title>";
	
       ?>
       <!-- move the logout buttont to the right of username -->
       <!-- code for IF SESSION VAR "Log out" is set, then do log out procedure-->
       <!-- auto redirect to next page-->
</form>

<!-- 

FIGURE OUT HOW TO 
MOVE BOTH FOLLOWER AND FOLLOWEES TO CENTER SOMEHOW

	-->

<p><br><br><br>
	Welcome to 
	<?php whose(); ?>
	 Wooter Home Page.
</p>


<p> <br><br>
	<?php whose(); ?>
	Woots:
<table align="center" border="1" bgcolor="cyan">
	<tr>
	    <td><i>1_recent_woot</i></td>
	    <td><font size="2" color="grey"> 1_time_stamp </font></td>
	</tr>
	<tr>
	    <td><i>2_recent_woot</i></td>
	    <td> <font size="2" color="grey">2_time_stamp</font> </td>
	</tr>
</table>


</p>



<p class="left"> 
	<?php whose(); ?>
 Followers: 
<table border="1" bgcolor="cyan">
<tr><td>
	<table>
	<tr>
	    <td><a href="home.php?user=<?php echo "jjohnson2z"?>">Follower_1_Username</a></td>
	    <td><font size="2"> F_1_Full_Name </font> </td> 
	    <td><font size="2" color="grey"> F_1_age </font></td>
	</tr>
	<tr>
	    <td><i>F_1_recent_woot</i></td>
	</tr>
	</table>
</td></tr>

<tr><td>
	<table>
	<tr>
	    <td>Follower_2_Username</td>
	    <td><font size="2"> F_2_Full_Name </font> </td> 
	    <td><font size="2" color="grey"> F_2_age </font></td>
	</tr>
	<tr>
	    <td><i>F_2_recent_woot</i></td>
	</tr>
	</table>
</td></tr>

</table>
</p>

<p class="right"> Users 
	<?php 
	if(!isset($other_user)) { 
		echo "You're";
	}
	else { 
		echo "$other_user->username is";
	}
	?>
 Following: 
<table border="1" bgcolor="cyan">
<tr><td>
  	<table>
	<tr>
	    <td>Followee_1_Username</td>
	    <td><font size="2"> F_1_Full_Name </font> </td> 
	    <td><font size="2" color="grey"> F_1_age </font></td>
	</tr>
	<tr>
	    <td><i>F_1_recent_woot</i></td>
	</tr>
	</table>
</td></tr>
<tr><td>
	<table>
	<tr>
	    <td>Followee_2_Username</td>
	    <td><font size="2"> F_2_Full_Name </font> </td> 
	    <td><font size="2" color="grey"> F_2_age </font></td>
	</tr>
	<tr>
	    <td><i>F_2_recent_woot</i></td>
	</tr>
	</table>
</td></tr>
</table>
</p>


<!--
	<p>
		Here are some users you might find interesting:
	</p> -->
<?php
// for the first 10 random users (random # for line # from arrays)
// list username as a link to their web page, which is $username.html
?>




</body>
</html>
