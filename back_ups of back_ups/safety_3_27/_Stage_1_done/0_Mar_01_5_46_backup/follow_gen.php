<?php
include "user_mgmt.php";
$user_obj = new User;
	// make 20 users, so that at least 20 exist if none so far
if(!file_exists("users/u_0.txt")){
	for($usernum=0; $usernum<21; $usernum++){
		$user_obj->username="test_acc".$usernum;
		$user_obj->password="test_psswd".$usernum;
		$user_obj->full_name="test_name".$usernum;
		$user_obj->age=$usernum%199;
		$user_obj->email="test_email".$usernum;
		
		create_new_user($user_obj);
	}
}
	// make sure a user can't follow themselves?
	//~ for($usernum=0; $usernum<21; $usernum++){
//~ for($offset=1; $offset<21; $offset++){
		//~ $user_obj=read_user("test_acc".$usernum);
		//~ $other_obj=read_user("test_acc".(($usernum+$offset)%21) );
		//~ follow($other_obj, $user_obj);
	//~ }
//~ }
		$user_obj=read_user("test_acc19");
		$other_obj=read_user("test_acc19");
		unfollow($other_obj, $user_obj);
?>
