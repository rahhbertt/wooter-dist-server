<?php
include "user_mgmt.php";
$user_obj = new User;
for($usernum=0; $usernum<20; $usernum++){
for($offset=0; $offset<20; $offset++){
		echo "User num: ".$usernum."<br>";
		$user_obj=read_user("test_acc".$usernum);
		write_woot($user_obj, "Test woot + $usernum + $offset", date("Y-m-d -> G:i:s", time()) ); 
	}
}
?>
