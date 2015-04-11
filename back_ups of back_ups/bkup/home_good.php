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
// // once logged in
// if (isset($_SESSION['count'])) { unset($_SESSION['count']); }

// to log out, close the session and go back to the main page
if($_POST['log_out_sub']){
	session_unset();
	session_destroy();
	header("Location: index.html");
}

// if not logged in, and try to access home.php
if(!isset($logged_in_user) &&  !isset($_GET['user']) ){ header("Location: login.php"); }
// if not logged in, accessing another user's page
if(isset($_GET['user'])){
	$other_user=new User;
	$other_user=read_user($_GET['user']);
	if(!isset($other_user->id)){ $user_dne=true; }
}
// once log in once, have the logged in username persist in top left
$user_obj=$_SESSION['user_obj'];
$logged_in_user=$user_obj->username;

if(isset($_POST['new_woot'])){
	// $_SESSION['new_woot']=$_POST['new_woot'];
}
if(isset($_POST['searching_for']) && !isset($_SESSION['searching_for']) ){
	// $_SESSION['searching_for']=$_POST['searching_for'];
}


function who(){ return $_GET['user']; }

function whose(){
	if(isset($_GET['user'])){ echo $_GET['user'] . "'s"; }
	else{ echo "Your"; }
}

function whoare(){
	if(isset($_GET['user'])){ echo $_GET['user'] . " is"; }
	else{ echo "You're"; }
}

function show_on_home($string){ if(!isset($_GET['user'])){ echo $string; } }

function hide_on_home($string){ if(isset($_GET['user'])){ echo $string; } }

?>

<body>

	<!-- The Overarching Table -->

	<table width="100%" id="entire_page">
		<tr><td>
			<table width="100%" border="0">
				<tr>
					<td>
						<form action="home.php" method="post" align="left">
							<?php echo "<a class='button' href='home.php'> $logged_in_user </a>&nbsp"; ?>
							<input class="button"  type="submit" value="Log out" name="log_out_sub">
						</form>
					</td>
					<td>
						<form method="post" align="right">
							<input type="text" name="searching_for" placeholder="Search users here..">
						</form>

					</td>
				</tr>
			</table>
		</td></tr>

		<!-- The Followers/Followees/Woots/Users Table -->

		<tr><td>
			<table width="100%" border="0">
				<tr> 
					<td></td>
					<td align="center">
						<p class="tcenter">  
							<font class="tcolor" size="6"> <-Wooter 1.1-> </font>   
						</p><p>
						
						<?php
						hide_on_home("<table style='border-collapse: collapse' cellpadding='10'>
							<tr style='border:1px solid #000'><td>
								<a href='home.php?user=" . $_GET['user'] . "'> <font size='5'>" . $_GET['user']
									. " </font> </a> </td></tr>
									<tr><td><form method='post'>");
							if(isset($_GET['user'])){
								if(!isset($_POST['follow'])){
									echo "<input type='submit' class='button' name='follow' value='Follow' style='width:100%'>" ; 
								}
								else {
									echo "<input type='submit' class='button' name='unfollow' value='Unfollow' style='width:100%'>" ; 
								}
							}
							hide_on_home("</form></td></tr></table>");
							
							show_on_home("
								<font class='tcolor'>
									Welcome to Your Wooter Home Page.
								</font>");
							// if(isset($_SESSION['searching_for'])){
							// 	echo "<br><em><font class='tcolor'> Your search request for " . $_SESSION['searching_for'] . " has been duly noted.</font></em>";	
							// }
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
						<table>
							<tr><td>
								<p class="tcenter"> 
									<font class="tbcolor">	
										<?php whose(); ?> Followers: 
									</font>
									<table border="1" class="tbcolor" align="center">
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_1">Follower_1_Username</a></td>
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
													<td><a href="home.php?user=some_user_2">Follower_2_Username</a></td>
													<td><font size="2"> F_2_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_2_age </font></td>
												</tr>
												<tr>
													<td><i>F_2_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_3">Follower_3_Username</a></td>
													<td><font size="2"> F_3_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_3_age </font></td>
												</tr>
												<tr>
													<td><i>F_3_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_4">Follower_4_Username</a></td>
													<td><font size="2"> F_4_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_4_age </font></td>
												</tr>
												<tr>
													<td><i>F_4_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_5">Follower_5_Username</a></td>
													<td><font size="2"> F_5_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_5_age </font></td>
												</tr>
												<tr>
													<td><i>F_5_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_6">Follower_6_Username</a></td>
													<td><font size="2"> F_6_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_6_age </font></td>
												</tr>
												<tr>
													<td><i>F_6_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_7">Follower_7_Username</a></td>
													<td><font size="2"> F_7_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_7_age </font></td>
												</tr>
												<tr>
													<td><i>F_7_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>	
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_8">Follower_8_Username</a></td>
													<td><font size="2"> F_8_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_8_age </font></td>
												</tr>
												<tr>
													<td><i>F_8_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>	
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_9">Follower_9_Username</a></td>
													<td><font size="2"> F_9_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_9_age </font></td>
												</tr>
												<tr>
													<td><i>F_9_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>	
										<tr><td>
											<table>
												<tr>
													<td><a href="home.php?user=some_user_10">Follower_10_Username</a></td>
													<td><font size="2"> F_10_Full_Name </font> </td> 
													<td><font size="2" color="grey"> F_10_age </font></td>
												</tr>
												<tr>
													<td><i>F_10_recent_woot</i></td>
												</tr>
											</table>
										</td></tr>	
									</table>

									<tr><td align="right">
										<font class="tcolor">
											<a href="home.php">See all..</a>
										</font>
									</td></tr>
								</td></tr>
							</table>

						</p>
					</td>

					<!-- Your Recent Woots -->
					<!-- sub-table to center both WOOT HERE and YOUR WOOTS -->
					<td>
						<table align="center">
							<tr><td>
								<table align="center" border="0" height="600">
									<?php
									show_on_home("<tr>
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
											<p class="tcenter"> 
												<font class="tbcolor">
													<?php whose(); ?> Recent Woots: 
												</font>
												<table border="1" class="tbcolor"align="center">
													<tr>
														<td><i>1_recent_woot</i></td>
														<td><font size="2" color="grey"> 1_time_stamp </font></td>
													</tr>
													<tr>
														<td><i>2_recent_woot</i></td>
														<td> <font size="2" color="grey"> 2_time_stamp</font> </td>
													</tr>
													<tr>
														<td><i>3_recent_woot</i></td>
														<td><font size="2" color="grey"> 3_time_stamp </font></td>
													</tr>
													<tr>
														<td><i>4_recent_woot</i></td>
														<td> <font size="2" color="grey"> 4_time_stamp</font> </td>
													</tr>
													<tr>
														<td><i>5_recent_woot</i></td>
														<td><font size="2" color="grey"> 5_time_stamp </font></td>
													</tr>
													<tr>
														<td><i>6_recent_woot</i></td>
														<td> <font size="2" color="grey"> 6_time_stamp</font> </td>
													</tr>
													<tr>
														<td><i>7_recent_woot</i></td>
														<td><font size="2" color="grey"> 7_time_stamp </font></td>
													</tr>
													<tr>
														<td><i>8_recent_woot</i></td>
														<td> <font size="2" color="grey"> 8_time_stamp</font> </td>
													</tr>
													<tr>
														<td><i>9_recent_woot</i></td>
														<td><font size="2" color="grey"> 9_time_stamp </font></td>
													</tr>
													<tr>
														<td><i>10_recent_woot</i></td>
														<td> <font size="2" color="grey"> 10_time_stamp</font> </td>
													</tr>


												</table>

												<font class="tcolor" align="right">
													<a href="home.php">See all..</a>
												</font>

											</p>
										</td>
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
									<p class="tcenter"> 
										<font class ="tbcolor"> Users <?php whoare(); ?> Following: </font>
										<table border="1" class="tbcolor"align="center">
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_1">Followee_1_Username</a></td>
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
														<td><a href="home.php?user=some_user_2">Followee_2_Username</a></td>
														<td><font size="2"> F_2_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_2_age </font></td>
													</tr>
													<tr>
														<td><i>F_2_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_3">Followee_3_Username</a></td>
														<td><font size="2"> F_3_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_3_age </font></td>
													</tr>
													<tr>
														<td><i>F_3_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_4">Followee_4_Username</a></td>
														<td><font size="2"> F_4_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_4_age </font></td>
													</tr>
													<tr>
														<td><i>F_4_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_5">Followee_5_Username</a></td>
														<td><font size="2"> F_5_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_5_age </font></td>
													</tr>
													<tr>
														<td><i>F_5_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_6">Followee_6_Username</a></td>
														<td><font size="2"> F_6_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_6_age </font></td>
													</tr>
													<tr>
														<td><i>F_6_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_7">Followee_7_Username</a></td>
														<td><font size="2"> F_7_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_7_age </font></td>
													</tr>
													<tr>
														<td><i>F_7_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>	
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_8">Followee_8_Username</a></td>
														<td><font size="2"> F_8_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_8_age </font></td>
													</tr>
													<tr>
														<td><i>F_8_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>	
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_9">Followee_9_Username</a></td>
														<td><font size="2"> F_9_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_9_age </font></td>
													</tr>
													<tr>
														<td><i>F_9_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>	
											<tr><td>
												<table>
													<tr>
														<td><a href="home.php?user=some_user_10">Followee_10_Username</a></td>
														<td><font size="2"> F_10_Full_Name </font> </td> 
														<td><font size="2" color="grey"> F_10_age </font></td>
													</tr>
													<tr>
														<td><i>F_10_recent_woot</i></td>
													</tr>
												</table>
											</td></tr>	
										</table>
									</td>
								</tr>
								<tr><td align="right">
									<font class="tcolor"> <a href="home.php"> See all.. </a></font>
								</td></tr>
							</table>
						</p>
					</td>
				</tr>
				<?php  
				show_on_home("<tr>
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