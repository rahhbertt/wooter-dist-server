<doctype! html>
<html>
<head>
	<title> See all your followers & followees </title>
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
<body>
</body>
<?php
include "user_mgmt.php";
session_start();
$home_user=$_SESSION['user_obj'];
$other_user=$_SESSION['other_user'];
echo "<p class=\"tcenter\">Viewing All:</p>";
echo "<table width=\"100%\" id=\"entire_page\" class=\"tcenter tcolor\" align=\"center\"><tr><td>";

if(!isset($_POST['woots'])){
	if(isset($_POST['followers'])){
		if(!isset($_SESSION['user'])){ 
			$id_list=load_following($home_user, "followers", $home_user->followers); 
			$limit=$home_user->followers;
		}
		else { 
			$id_list=load_following($other_user, "followers", $other_user->followers); 
			$limit=$other_user->followers;
		}
		echo "$limit in total:<br>";
		$cult=array();
		for($index=0; $index<$limit;$index++){
			$temp_usr=read_id($id_list[$index]);
			array_push($cult, $temp_usr);
		}
	}
	else if(isset($_POST['followees'])){
		if(!isset($_SESSION['user'])){ 
			$id_list=load_following($home_user, "followees", $home_user->followees); 
			$limit=$home_user->followees;
		}
		else { 
			$id_list=load_following($other_user, "followees", $other_user->followees); 
			$limit=$other_user->followees;
		}
		echo "$limit in total:<br>";
		$cult=array();
		for($index=0; $index<$limit;$index++){
			$temp_usr=read_id($id_list[$index]);
			array_push($cult, $temp_usr);
		}
	}
	foreach($cult as &$user){
		echo "<a href='home.php?user=".$user->username."'>".$user->username."</a></td></tr><tr><td>";
	}
}
else {
	$woot_list=array();
	if(!isset($_SESSION['user'])){ $woot_list=load_woots($home_user, $home_user->num_woots); }
	else { $woot_list=load_woots($other_user, $other_user->num_woots); }
		//~ echo "$home_user->num_woots many woots<br> b$other_user->num_woots num woots also";
	foreach($woot_list as &$woot_obj){
		echo $woot_obj->message."</td><td>".$woot_obj->timestamp."</td></tr><tr><td>";
	}
}
echo "<p>Please click here to <a href=\"#\" onclick=\"history.go(-1)\">go back</a>.";
echo "</td></tr></table>";
?>
</body>
</html>
