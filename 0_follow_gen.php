<?php
include "user_mgmt.php";
$user_obj = new User;
// make 20 users, so that at least 20 exist if none so far
$FILE_PATH="/var/www/html/";

if(!file_exists( $FILE_PATH."fids/fids.txt" )){
	for($usernum=0; $usernum<20; $usernum++){
		$user_obj->username="test_acc".$usernum;
		$user_obj->password="test_psswd".$usernum;
		$user_obj->full_name="Test's_First_name Test's_Last_name_+_".$usernum;
		$user_obj->age=100*$usernum%199;
		$user_obj->email="test_email@".$usernum.".com";
		create_new_user($user_obj);
		echo PHP_EOL."made the following user".PHP_EOL;
		print_r($user_obj);
		echo PHP_EOL;
	}
}

$usernum=0; $offset=1;
for($usernum=0; $usernum<20; $usernum++){
	for($offset=10; $offset<25; $offset++){
		echo "User num: ".$usernum." Offset: ".($offset%20)."<br>";
		$user_obj=read_user("test_acc".$usernum);
		if( ($usernum+$offset)%20 == $offset%20) { $offset2=1;} 
		else { $offset2=0; } 
		$other_obj=read_user("test_acc".(($usernum+$offset+$offset2))%20); 
		echo "You are trying to get $user_obj->username to follow $other_obj->username.<br><br>"; 
		if(!empty($other_obj) && $user_obj->id!=$other_obj->id){
			follow($user_obj, $other_obj); 
		}
	}
}
?>
