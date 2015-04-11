<?php
include "user_mgmt.php";
$user_obj = new User;
	//~ // make 20 users, so that at least 20 exist if none so far

if(!file_exists("fids/fids.txt")){
	for($usernum=0; $usernum<20; $usernum++){
		$user_obj->username="test_acc".$usernum;
		$user_obj->password="test_psswd".$usernum;
		$user_obj->full_name="Test's_First_name Test's_Last_name + ".$usernum;
		$user_obj->age=100*$usernum%199;
		$user_obj->email="test_email@".$usernum.".com";
			echo "made it here<br>";
		create_new_user($user_obj);
		echo "It is being written that the user: $user_obj->username has $user_obj->followers followers and $user_obj->followees followees<br>";

	}
}
$usernum=0; $offset=1;
		//~ $user_obj=read_user("test_acc".$usernum);
		//~ $other_obj=read_user("test_acc".(($usernum+$offset)%21) );
		//~ follow($user_obj, $other_obj);
	//~ // make sure a user can't follow themselves?
for($usernum=0; $usernum<20; $usernum++){
	for($offset=10; $offset<35; $offset++){
		echo "User num: ".$usernum." Offset: ".($offset%20)."<br>";
		$user_obj=read_user("test_acc".$usernum);
		$other_obj=read_user("test_acc".(($usernum+$offset))%20);
		echo "<br><br>You are trying to get $user_obj->username to follow $other_obj->username.<br>"; 
		if(!empty($other_obj) && $user_obj->id!=$other_obj->id){
			follow($user_obj, $other_obj); 
		}
	}
}

	//~ for($usernum=0; $usernum<21; $usernum++){
//~ for($offset=19; $offset<21; $offset++){
		//~ $user_obj=read_user("test_acc".$usernum);
		//~ $other_obj=read_user("test_acc".(($usernum+$offset)%21) );
		//~ follow($user_obj, $other_obj);
	//~ }
//~ }
// following after following same no problem
//following after unfollowing is a problem
// but reunfollowing after that is no problem

?>
