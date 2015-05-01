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
$home_user=$_SESSION['user_obj']; // login.php passes user as $_SESSION['user_obj']

// LOGGING OUT
if(isset($_POST['log_out_sub'])){ // destroy session on log out
	session_unset();
	session_destroy();
	header("Location: index.html");
} 
else if(!isset($home_user->username) &&  !isset($_GET['user']) ){ 
	header("Location: login.php"); // if not logged in, and try to access your own home page
} 

// VIEWING ANOTHER USER
if(isset($_GET['user'])){ // this means you're trying to view another user's page
	if($_GET['user']!=$home_user->username){ // if that user is not yourself, read them in
		$other_user=new User;
		$other_user=read_user($_GET['user']);
		$_SESSION['user']=$_GET['user'];
		$_SESSION['other_user']=$other_user;
		if(!isset($other_user->id)||$other_user->free_bit=="d"){ $user_dne=true; } // will display error or hide parts of layout
		// if condition not met, $user_dne will not be set, and isset($user_dne) works
	}
	else { 
		unset($_GET['user']); 
		unset($_SESSION['user']);
	} // if you're trying to view your own page, show your home page
}
else {  unset($_SESSION['user']); }

// FOLLOWING AND UNFOLLOWING
if(isset($home_user->username)){ // only use the follow/unfollow buttons if you're logged in
if(isset($_POST['follow'])){  follow($home_user, $other_user); }
if(isset($_POST['unfollow'])) {  unfollow($home_user, $other_user); }
}

// RETRIEVING FOLLOWERS AND FOLLOWEES 
unset($id_list_fe); // destroy the old lists to be safe
unset($fe_cult);
if(!isset($_GET['user'])){ $id_list_fe=load_following($home_user, "followees", 10); }
else { $id_list_fe=load_following($other_user, "followees", 10); }
$fe_cult=array();
if(!empty($id_list_fe)){
	for($index=0; $index<10;$index++){
		$temp_usr=read_id($id_list_fe[$index]);
		array_push($fe_cult, $temp_usr);
	}
}
unset($id_list_fr); // repeat for followers
unset($fr_cult);
if(!isset($_GET['user'])){ $id_list_fr=load_following($home_user, "followers", 10); }
else { $id_list_fr=load_following($other_user, "followers", 10); }
$fr_cult=array();
if(!empty($id_list_fr)){
	for($index=0; $index<10;$index++){
		$temp_usr=read_id($id_list_fr[$index]);
		array_push($fr_cult, $temp_usr);
	}
}
// POSTING NEW WOOTS
if(isset($_POST['new_woot']) && !isset($_SESSION['prev_woot'])){
	write_woot($home_user, str_replace(PHP_EOL, '',$_POST['new_woot']) ,date("Y-m-d -> G:i:s", time()));
	$_SESSION['prev_woot']=$_POST['new_woot']; 
}
else if(isset($_SESSION['prev_woot'])){
	if($_SESSION['prev_woot']!=$_POST['new_woot']){ // only write the woot if its not identical to the one just before it
		write_woot($home_user, str_replace(PHP_EOL, '',$_POST['new_woot']),date("Y-m-d -> G:i:s", time()));
		$_SESSION['prev_woot']=$_POST['new_woot'];
	} // if they are equal write nothing
}

// RETRIEVING WOOTS
$woot_list=array();
if(!isset($_GET['user'])){ $woot_list=load_woots($home_user, 10); }
else { $woot_list=load_woots($other_user, 10); }


// various misc. functions
function whose(){
	if(isset($_GET['user'])){ echo $_GET['user'] . "'s"; }
	else{ echo "Your"; }
}
function whoare(){
	if(isset($_GET['user'])){ echo $_GET['user'] . " is"; }
	else{ echo "You're"; }
}
function home_other($string){ if(isset($_GET['user'])){ echo $string; } }

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
							if(isset($home_user->username)){ // if you're logged in, show your username in the top left
							echo "<a class='button' href='home.php'> $home_user->username </a>&nbsp 
							<input class=\"button\"  type=\"submit\" value=\"Log out\" name=\"log_out_sub\">";
							} // if viewing as the public, dont show that
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
							<font class="tcolor" size="6"> <-Wooter 1.4-> </font> 
							<?php // if its your own home page, show a welcome message  		
							if(!isset($_GET['user'])){ echo "</p><p><font class='tcolor'> Welcome to Your Wooter Home Page. </font>"; }
							?>
						</p>

						<?php 
						// SHOW DIFFERENT PAGES IF YOUR HOME PAGE OR ANOTHER USER'S	
						home_other("<table style='border-collapse: collapse' cellpadding='10' class='tcolor'> <tr style='border:1px solid #000'><td>");
						if(isset($_GET['user'])&&!isset($user_dne)){ echo "<a href='home.php?user=".$_GET['user']."'> "; }
						home_other("<font size='5'>");	
						if(isset($_GET['user'])&&!isset($user_dne)){ echo $_GET['user']; }
						if(isset($user_dne)){ // if the user youre trying to view does not exist, output an error
							echo "<p>We're sorry, but <a href=\"home.php?user=".$_GET['user']."\">".$_GET['user']."</a> is not a valid account. The user may have been deleted, deactivated, or never have existed.</p>";
							echo "<p>Please click here to <a href=\"#\" onclick=\"history.go(-1)\">go back</a> and try again.";
						}

						// FOLLOW/UNFOLLOW BUTTON WITH OTHER USERS
						home_other(" </font> </a> </td></tr> <tr><td><form method='post'>");
						if(isset($_GET['user'])&&!isset($user_dne)){
							$answer = already_following($home_user, $other_user->id);
							if(!isset($answer)){ echo "<input type='submit' class='button' name='follow' value='Follow' style='width:100%'>" ;  }
							else { echo "<input type='submit' class='button' name='unfollow' value='Unfollow' style='width:100%'>" ;  }
						}
						home_other("</form></td></tr><table>");
						?>
					</p>
				</td>
			</tr>

			<!-- Your Followers table -->
			<!-- second row of overarching table -->

			<tr>
				<td>
					<?php					
					// SHOW TABLE OF FOLLOWERS
					if(!isset($user_dne)){ 
						echo "<table> <tr><td><p class='tcenter'><font class='tbcolor'>";
						whose();
						echo " Followers:</font> <table border='1' class='tbcolor' align='center'>";
						$fr_woots=array();
						for($index=0; $index<10; $index++){ 
							if(!empty($fr_cult)){ 
								array_push($fr_woots, load_woots($fr_cult[$index], 1) ); 
							}	
						}
						for($index=0; $index<10; $index++){
							echo"<tr><td>
							<table><tr>
								<td><a href='home.php?user=".($fr_cult[$index]->username)."'>".($fr_cult[$index]->username)."</a></td>
								<td><font size='2'>".($fr_cult[$index]->full_name)."</font> </td> 
								<td><font size='2' color='grey'>".($fr_cult[$index]->age)." </font></td>
							</tr><tr>
							<td><i>".$fr_woots[$index][0]->message."</i></td>
							<td><font size='2'><i>".$fr_woots[$index][0]->timestamp."</i></font></td></tr></table></td></tr>";
						}
						echo "</table><tr><td align='right'> <font class='tcolor'>
						<form action='seeall.php' method='post'><input type='submit' name='followers' value='See all..'></form></font></td></tr></td></tr></table>"; 
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
							// SUBMIT A WOOT BOX
							if(!isset($_GET['user'])){  
								echo "<tr><td><form action='home.php' method='post'>";
								echo "<textarea rows='6' cols='40' name='new_woot' placeholder=\"&#10&#09   What's on your mind?&#10&#09Tell the world in a woot!&#10&#09  (Max. 100 characters)\" maxlength='100'></textarea> <br>";
								echo "<input class='fright' type='submit' value='Woot it' style='width:340'> </form> </td> </tr>"; 
							}
							?> 

							<tr>
								<td>
									<?php
									// SHOW LIST OF WOOTS
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
							// SHOW TABLE OF FOLLOWEES
							if(!isset($user_dne)){
								echo "<p class='tcenter'><font class='tbcolor'>";
								whoare();
								echo " Following:</font> <table border='1' class='tbcolor' align='center'>";
								$fe_woots=array();
								for($index=0; $index<10; $index++){ 
									if(!empty($fe_cult)){ 
										array_push($fe_woots, load_woots($fe_cult[$index], 1) ); 
									}
								}	

								for($index=0; $index<10; $index++){
									echo"<tr><td>
									<table><tr>
										<td><a href='home.php?user=".($fe_cult[$index]->username)."'>".($fe_cult[$index]->username)."</a></td>
										<td><font size='2'>".($fe_cult[$index]->full_name)."</font> </td> 
										<td><font size='2' color='grey'>".($fe_cult[$index]->age)." </font></td>
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
	
	<?php
	if(isset($home_user->username)){ // else letting a non logged in user deactivate is a problem
	echo '<form action="deactivate.php" method="post" align="right">
		<a href="deactivate.php" class="button tcolor"> Deactivate your account </a>
	</form>';
	}
	?>
</table>
</td></tr>

</body>
</html>
