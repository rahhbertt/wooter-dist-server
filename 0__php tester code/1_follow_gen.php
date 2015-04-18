<?php
include "user_mgmt.php";
$user_obj = new User;
// make 20 users, so that at least 20 exist if none so far

if(!file_exists("fids/fids.txt")){
	for($usernum=20; $usernum>=0; $usernum--){
		$user_obj->username="test_acc".$usernum;
		$user_obj->password="test_psswd".$usernum;
		$user_obj->full_name="Test's_First_name Test's_Last_name_+_".$usernum;
		$user_obj->age=100*$usernum%199;
		$user_obj->email="test_email@".$usernum.".com";
		create_new_user($user_obj);
	}
}
else {
	$usernum=0; $offset=1;
	for($usernum=20; $usernum>=0; $usernum--){
		for($offset=10; $offset<35; $offset++){
			echo "User num: ".$usernum." Offset: ".($offset%21)."<br>";
			$user_obj=read_user("test_acc".$usernum);
			if( ($usernum+$offset)%21 == $offset%21) { $offset2=1;} 
			else { $offset2=0; } 
			$other_obj=read_user("test_acc".(($usernum+$offset+$offset2))%21); 
			echo "<br><br>You are trying to get $user_obj->username to follow $other_obj->username.<br>"; 
			if(!empty($other_obj) && $user_obj->id!=$other_obj->id){
				follow($user_obj, $other_obj); 
			}
		}
	}
}
?>
