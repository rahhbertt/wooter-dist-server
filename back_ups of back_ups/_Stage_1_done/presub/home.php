<doctype! html>
<html>
<head>
	<title> Welcome to <?php whose(); ?> Home Page! </title>
	<meta charset="UTF-8"> 
	<style>
		body {
			background: url(wooter.png), url(blue.jpg);
			background-repeat: repeat-y, no-repeat;
			text-align: center;
			background-position: right,  bottom -100px left -70px;
			background-attachment: fixed;
		}
		.fleft { float: left; }
		.fright { float: right; }
		.tcenter { text-align: center; }
		.tleft { text-align: left; }
		.tcolor { background: rgba(255,255,255,0.8); }
		.tbcolor { background: rgba(0,255,255,0.8); }
		.button:hover { background: cyan; cursor: pointer; }
	</style>
</head>

<?php
include "user_mgmt.php";
session_start();
// once logged in
// to log out, close the session and go back to the main page
$user_obj=$_SESSION['user_obj'];
$logged_in_user=$user_obj->username;
unset($_SESSION['user']); // so only goes to following page
unset($_SESSION['other_user']);
if(isset($_POST['log_out_sub'])){ // destroy session on log out
	session_unset();
	session_destroy();
	header("Location: index.html");
} else if(!isset($logged_in_user) &&  !isset($_GET['user']) ){ 
	header("Location: login.php"); // if not logged in, and try to access home.php
} 

// if logged in or not, accessing another user's page
if(isset($_GET['user'])){
	$_SESSION['user']=$_GET['user'];
	$other_user=new User;
	$other_user=read_user($_GET['user']);
	if(!isset($other_user->id)||$other_user->free_bit=="d"){ $user_dne=true; } // will display error or hide parts of layout
	else { $_SESSION['other_user']=$other_user; }
}

//~ print_r($user_obj);
//~ print_r($other_user);
if(!isset($_GET['user'])){ $id_list=load_following($user_obj, "followers", 10); }
else { $id_list=load_following($other_user, "followers", 10); }
$cult=array();
for($index=0; $index<10;$index++){
	$temp_usr=read_id($id_list[$index]);
	array_push($cult, $temp_usr);
}

if(!isset($_GET['user'])){ $id_list_2=load_following($user_obj, "followees", 10); }
else { $id_list_2=load_following($other_user, "followees", 10); }
$cult_2=array();
for($index=0; $index<10;$index++){
	$temp_usr=read_id($id_list_2[$index]);
	array_push($cult_2, $temp_usr);
}


if(isset($_POST['new_woot']) && !isset($_SESSION['prev_woot'])){
	write_woot($user_obj, $_POST['new_woot'],date("Y-m-d -> G:i:s", time()));
	$_SESSION['prev_woot']=$_POST['new_woot']; 
}
else if(isset($_SESSION['prev_woot'])){
	if($_SESSION['prev_woot']!=$_POST['new_woot']){
		write_woot($user_obj, $_POST['new_woot'],date("Y-m-d -> G:i:s", time()));
		$_SESSION['prev_woot']=$_POST['new_woot'];
	} // only allow an woot to be written if its not the same as the one immediatley before
	// if they are equal write nothing
}

// do an array of woots here as well

$woot_list=array();
if(!isset($_GET['user'])){ $woot_list=load_woots($user_obj, 10); }
else { $woot_list=load_woots($other_user, 10); }
//~ print_r($woot_list);







//if(isset($_POST['searching_for']) && !isset($_SESSION['searching_for']) ){
	// $_SESSION['searching_for']=$_POST['searching_for'];
//}

function who(){ return $_GET['user']; }

function whose(){
	if(isset($_GET['user'])){ echo $_GET['user'] . "'s"; }
	else{ echo "Your"; }
}

function whoare(){
	if(isset($_GET['user'])){ echo $_GET['user'] . " is"; }
	else{ echo "You're"; }
}
// de : does exist, ifn: if not
function de_other_user($string){ global $user_dne; if(isset($_GET['user'])&&!isset($user_dne)){ echo $string; } }
function de_user($string){ global $user_dne; if(!isset($user_dne)){ echo $string; } }
function home_other($string){ if(isset($_GET['user'])){ echo $string; } }
function home_user($string){ if(!isset($_GET['user'])){ echo $string; } }
function ifn_get_user($string){ if(!isset($_GET['user'])){ echo $string; } }
function if_get_user($string){ if(isset($_GET['user'])){ echo $string; } }

?>

<body>

	<!-- The Overarching Table -->

	<table width="100%" id="entire_page">
		<tr><td>
			<table width="100%" border="0">
				<tr>
					<td>
						<form action="home.php" method="post" align="left">
							<?php 
							if(isset($logged_in_user)){
								echo "<a class='button' href='home.php'> $logged_in_user </a>&nbsp 
								<input class=\"button\"  type=\"submit\" value=\"Log out\" name=\"log_out_sub\">";
							}
							?>
						</form>
					</td>
					<td>
						<form method="get" align="right">
							<input type="text" name="user" placeholder="Search users here..">
						</form>

					</td>
				</tr>
			</table>
		</td></tr>

		<!-- The Followers/Followees/Woots/Users Table -->

		<tr><td>
			<table width="100%" border="0">
				<tr > 
					<td></td>
					<td align="center">
						<p class="tcenter">  
							<font class="tcolor" size="6"> <-Wooter 1.1-> </font> 
							<?php   		
							home_user("</p><p>
							<font class='tcolor'>
								Welcome to Your Wooter Home Page.
							</font>");
							?>
						</p>
						
						<?php
						home_other("<table style='border-collapse: collapse' cellpadding='10' class='tcolor'> <tr style='border:1px solid #000'><td>");
						de_other_user("<a href='home.php?user=".$_GET['user']."'> ");
						home_other("<font size='5'>");	
						de_user( $_GET['user'] );
						if(isset($user_dne)){
							echo "<p>We're sorry, but <a href=\"home.php?user=".$_GET['user']."\">".$_GET['user']."</a> is not a valid account. The user may have been deleted, deactivated, or never have existed.</p>";
							echo "<p>Please click here to <a href=\"#\" onclick=\"history.go(-1)\">go back</a> and try again.";
						}
						home_other(" </font> </a> </td></tr> <tr><td><form method='post'>");

						if(isset($_GET['user'])&&!isset($user_dne)){
							if(!isset($_POST['follow'])){
								echo "<input type='submit' class='button' name='follow' value='Follow' style='width:100%'>" ; 
							}
							else {
								follow($user_obj, $other_user);
								echo "<input type='submit' class='button' name='unfollow' value='Unfollow' style='width:100%'>" ; 
							}
							if(isset($_POST['unfollow'])){ unfollow($user_obj, $other_user); }
							
						}
						home_other("</form></td></tr><table>");
						
						//~ unset($_SESSION['searching_for']);
						//~ unset($_SESSION['new_woot']);
							// so it only comes up once and doesnt persist after a reload
						?>

					</p>
				</td>
			</tr>

			<!-- Your Followers table -->
			<!-- second row of overarching table -->

			<tr>
				<td>
				<?php					
				if(!isset($user_dne)){
					echo "<table> <tr><td><p class='tcenter'><font class='tbcolor'>";
					whose();
					echo " Followers:</font> <table border='1' class='tbcolor' align='center'>";

					$fr_woots=array();
					for($index=0; $index<10; $index++){ array_push($fr_woots, load_woots($cult[$index], 1) ); }	
					for($index=0; $index<10; $index++){
						echo"<tr><td>
							<table><tr>
								<td><a href='home.php?user=".($cult[$index]->username)."'>".($cult[$index]->username)."</a></td>
								<td><font size='2'>".($cult[$index]->full_name)."</font> </td> 
								<td><font size='2' color='grey'> Age:".($cult[$index]->age)." </font></td>
							</tr><tr>
								<td><i>".$fr_woots[$index][0]->message."</i></td>
								<td><font size='2'><i>".$fr_woots[$index][0]->timestamp."</i></font></td>
							</tr></table>
						</td></tr>";

					}
					echo "</table><tr><td align='right'>
						<font class='tcolor'>
							<form action='seeall.php' method='post'><input type='submit' name='followers' value='See all..'></form>
						</font>
					</td></tr></td></tr></table>";
			}?>

	</p>
</td>

<!-- Your Recent Woots -->
<!-- sub-table to center both WOOT HERE and YOUR WOOTS -->
<td>
	<table align="center">
		<tr><td>
			<table align="center" border="0" height="600">
				<?php
				ifn_get_user("<tr>
					<td>
						<form action='home.php' method='post'>
							<textarea rows='6' cols='40' name='new_woot' placeholder=\"&#10&#09   What's on your mind?&#10&#09Tell the world in a woot!&#10&#09  (Max. 100 characters)\" maxlength='100'></textarea>
							<br>
							<input class='fright' type='submit' value='Woot it' style='width:340'>
						</form>
					</td>
				</tr>"); ?> 

				<tr>
					<td>
						<?php
						if(!isset($user_dne)){
							echo "<p class='tcenter'> <font class='tbcolor'>";
							whose();
							echo " Recent Woots: </font> <table border='1' class='tbcolor' align='center'>";
							for($index=0; $index<10; $index++){
								echo "<tr><td><i>".$woot_list[$index]->message."</i></td>
								<td><font size='2' color='grey'>".$woot_list[$index]->timestamp."</font></td></tr>";
							}
							echo"</table><font class='tcolor' align='right'>
							<form action='seeall.php' method='post'><input type='submit' name='woots' value='See all..'></form>
							</font></p></td>";  
						}
						?>
				</tr>
			</table>
		</tr></td>
	</table>
</td>

<!-- Your Followees Table -->

<td>
	<table align="right">
		<tr>
			<td>
			<?php				
			if(!isset($user_dne)){
				echo "<p class='tcenter'><font class='tbcolor'>";
				whoare();
				echo " Following:</font> <table border='1' class='tbcolor' align='center'>";
				$fe_woots=array();
				for($index=0; $index<10; $index++){ array_push($fe_woots, load_woots($cult_2[$index], 1) ); }	
				
				for($index=0; $index<10; $index++){
					echo"<tr><td>
						<table><tr>
							<td><a href='home.php?user=".($cult_2[$index]->username)."'>".($cult_2[$index]->username)."</a></td>
							<td><font size='2'>".($cult_2[$index]->full_name)."</font> </td> 
							<td><font size='2' color='grey'> Age:".($cult_2[$index]->age)." </font></td>
						</tr><tr>
								<td><i>".$fe_woots[$index][0]->message."</i></td>
								<td><font size='2'><i>".$fe_woots[$index][0]->timestamp."</i></font></td>
						</tr></table>
					</td></tr>";
				}
				echo "</table><tr><td align='right'>
					<font class='tcolor'>
						<form action='seeall.php' method='post'><input type='submit' name='followees' value='See all..'></form>
					</font>
				</td></tr></td></tr></table>";
			}
			?>
		</table>
	</p>
</td>
</tr>
</table>
</td></tr>
<tr><td>
	<form action="deactivate.php" method="post" align="right">
		<a href="deactivate.php" class="button tcolor"> Deactivate your account </a>
	</form>
</table>
</td></tr>

</body>
</html>
