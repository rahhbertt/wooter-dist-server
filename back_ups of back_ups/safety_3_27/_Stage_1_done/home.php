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
	if(!isset($other_user->id)){ $user_dne=true; } // will display error or hide parts of layout
	else { $_SESSION['other_user']=$other_user; }
}

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

//~ if(isset($_POST['new_woot'])){
	// $_SESSION['new_woot']=$_POST['new_woot'];
//~ }
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
						

							// if(isset($_SESSION['new_woot'])){
								// echo "<br><br><font class='tcolor'>The woot you just posted to the world:<br>" . $_SESSION['new_woot'] . "</font><br>";
							// }
						unset($_SESSION['searching_for']);
						unset($_SESSION['new_woot']);
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
					
					de_user("<table> <tr><td><p class='tcenter'><font class='tbcolor'>");	
					if(!isset($user_dne)){whose();}
					de_user(" Followers: 
				</font>
				<table border='1' class='tbcolor' align='center'>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[0]->username)."'>".($cult[0]->username)."</a></td>
								<td><font size='2'>".($cult[0]->full_name)."</font> </td> 
								<td><font size='2' color='grey'> ".($cult[0]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_1_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[1]->username)."'>".($cult[1]->username)."</a></td>
								<td><font size='2'> ".($cult[1]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[1]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_2_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[2]->username)."'>".($cult[2]->username)."</a></td>
								<td><font size='2'> ".($cult[2]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[2]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_3_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[3]->username)."'>".($cult[3]->username)."</a></td>
								<td><font size='2'> ".($cult[3]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[3]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_4_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[4]->username)."'>".($cult[4]->username)."</a></td>
								<td><font size='2'> ".($cult[4]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[4]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_5_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[5]->username)."'>".($cult[5]->username)."</a></td>
								<td><font size='2'> ".($cult[5]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[5]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_6_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[6]->username)."'>".($cult[6]->username)."</a></td>
								<td><font size='2'> ".($cult[6]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[6]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_7_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[7]->username)."'>".($cult[7]->username)."</a></td>
								<td><font size='2'> ".($cult[7]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[7]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_8_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[8]->username)."'>".($cult[8]->username)."</a></td>
								<td><font size='2'> ".($cult[8]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[8]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_9_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult[9]->username)."'>".($cult[9]->username)."</a></td>
								<td><font size='2'> ".($cult[9]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult[9]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_10_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
				</table>

				<tr><td align='right'>
					<font class='tcolor'>
						<form action='following.php' method='post'><input type='submit' name='followers' value='See all..'></form>
					</font>
				</td></tr>
			</td></tr>
		</table>");
		?>

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
						de_user("<p class='tcenter'> 
							<font class='tbcolor'>
								<?php whose(); ?> Recent Woots: 
							</font>
							<table border='1' class='tbcolor' align='center'>
								<tr>
									<td><i>1_recent_woot</i></td>
									<td><font size='2' color='grey'> 1_time_stamp </font></td>
								</tr>
								<tr>
									<td><i>2_recent_woot</i></td>
									<td> <font size='2' color='grey'> 2_time_stamp</font> </td>
								</tr>
								<tr>
									<td><i>3_recent_woot</i></td>
									<td><font size='2' color='grey'> 3_time_stamp </font></td>
								</tr>
								<tr>
									<td><i>4_recent_woot</i></td>
									<td> <font size='2' color='grey'> 4_time_stamp</font> </td>
								</tr>
								<tr>
									<td><i>5_recent_woot</i></td>
									<td><font size='2' color='grey'> 5_time_stamp </font></td>
								</tr>
								<tr>
									<td><i>6_recent_woot</i></td>
									<td> <font size='2' color='grey'> 6_time_stamp</font> </td>
								</tr>
								<tr>
									<td><i>7_recent_woot</i></td>
									<td><font size='2' color='grey'> 7_time_stamp </font></td>
								</tr>
								<tr>
									<td><i>8_recent_woot</i></td>
									<td> <font size='2' color='grey'> 8_time_stamp</font> </td>
								</tr>
								<tr>
									<td><i>9_recent_woot</i></td>
									<td><font size='2' color='grey'> 9_time_stamp </font></td>
								</tr>
								<tr>
									<td><i>10_recent_woot</i></td>
									<td> <font size='2' color='grey'> 10_time_stamp</font> </td>
								</tr>


							</table>

							<font class='tcolor' align='right'>
								<form action='following.php' method='post'><input type='submit' name='woots' value='See all..'></form>
							</font>

						</p>
					</td>");  ?>
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
				de_user("<p class='tcenter'> 
					<font class ='tbcolor'>");
				whoare(); 
				de_user(" Following: </font>
				<table border='1' class='tbcolor' align='center'>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[0]->username)."'>".($cult_2[0]->username)."</a></td>
								<td><font size='2'>".($cult_2[0]->full_name)."</font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[0]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_1_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[1]->username)."'>".($cult_2[1]->username)."</a></td>
								<td><font size='2'> ".($cult_2[1]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[1]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_2_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[2]->username)."'>".($cult_2[2]->username)."</a></td>
								<td><font size='2'> ".($cult_2[2]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[2]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_3_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[3]->username)."'>".($cult_2[3]->username)."</a></td>
								<td><font size='2'> ".($cult_2[3]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[3]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_4_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[4]->username)."'>".($cult_2[4]->username)."</a></td>
								<td><font size='2'> ".($cult_2[4]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[4]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_5_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[5]->username)."'>".($cult_2[5]->username)."</a></td>
								<td><font size='2'> ".($cult_2[5]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[5]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_6_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[6]->username)."'>".($cult_2[6]->username)."</a></td>
								<td><font size='2'> ".($cult_2[6]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[6]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_7_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[7]->username)."'>".($cult_2[7]->username)."</a></td>
								<td><font size='2'> ".($cult_2[7]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[7]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_8_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[8]->username)."'>".($cult_2[8]->username)."</a></td>
								<td><font size='2'> ".($cult_2[8]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[8]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_9_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
					<tr><td>
						<table>
							<tr>
								<td><a href='home.php?user=".($cult_2[9]->username)."'>".($cult_2[9]->username)."</a></td>
								<td><font size='2'> ".($cult_2[9]->full_name)." </font> </td> 
								<td><font size='2' color='grey'> ".($cult_2[9]->age)." </font></td>
							</tr>
							<tr>
								<td><i>F_10_recent_woot</i></td>
							</tr>
						</table>
					</td></tr>	
				</table>

				<tr><td align='right'>
					<font class='tcolor'>
						<form action='following.php' method='post'><input type='submit' name='followees' value='See all..'></form>
					</font>
				</td></tr>
			</td></tr>
		</table>"); ?>
		</table>
	</p>
</td>
</tr>
<?php  

home_user("<tr>
	<td></td>
	<td width='100%'>
		<p style='clear:both' class='tcenter'> 
			<font class='tbcolor'>
				Other Users You Might Be Interested In:
			</font>
			<table border='1' class='tbcolor' align='center'>
				<tr><td>
					<table>
						<tr>
							<td>Random_1_Username</a></td>
							<td><font size='2'> R_1_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_1_age </font></td>
						</tr>
						<tr>
							<td><i>R_1_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_2_Username</a></td>
							<td><font size='2'> R_2_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_2_age </font></td>
						</tr>
						<tr>
							<td><i>R_2_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_3_Username</a></td>
							<td><font size='2'> R_3_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_3_age </font></td>
						</tr>
						<tr>
							<td><i>R_3_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_4_Username</a></td>
							<td><font size='2'> R_4_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_4_age </font></td>
						</tr>
						<tr>
							<td><i>R_4_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_5_Username</a></td>
							<td><font size='2'> R_5_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_5_age </font></td>
						</tr>
						<tr>
							<td><i>R_5_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_6_Username</a></td>
							<td><font size='2'> R_6_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_6_age </font></td>
						</tr>
						<tr>
							<td><i>R_6_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_7_Username</a></td>
							<td><font size='2'> R_7_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_7_age </font></td>
						</tr>
						<tr>
							<td><i>R_7_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_8_Username</a></td>
							<td><font size='2'> R_8_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_8_age </font></td>
						</tr>
						<tr>
							<td><i>R_8_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_9_Username</a></td>
							<td><font size='2'> R_9_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_9_age </font></td>
						</tr>
						<tr>
							<td><i>R_9_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
				<tr><td>
					<table>
						<tr>
							<td>Random_10_Username</a></td>
							<td><font size='2'> R_10_Full_Name </font> </td> 
							<td><font size='2' color='grey'> R_10_age </font></td>
						</tr>
						<tr>
							<td><i>R_10_recent_woot</i></td>
						</tr>
					</table>
				</td></tr>
			</table>
		</p>
	</td>
	<td></td>
</tr> 	"); 
?> 
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
