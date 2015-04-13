
<?php
include "user_mgmt.php";
$user_obj = new User;
for($usernum=19; $usernum>=0; $usernum--){
for($offset=1020; $offset<1050; $offset++){
		echo "User num: ".$usernum."Woot: Test woot + $usernum + $offset <br>";
		$user_obj=read_user("test_acc".$usernum);
		write_woot($user_obj, "Test woot + $usernum + $offset", date("Y-m-d -> G:i:s", time()) ); 
	}
}
?>
