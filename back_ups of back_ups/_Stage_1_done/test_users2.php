#!/usr/bin/php
<?php
	include "user_mgmt.php";
	$user_obj = new User;
	for($usernum=0; $usernum<200; $usernum++){
		$user_obj->username="test_acc".$usernum;
		$user_obj->password="test_psswd".$usernum;
		$user_obj->full_name="test_name".$usernum;
		$user_obj->age=$usernum%199;
		$user_obj->email="test_email".$usernum;
		
		create_new_user($user_obj);
	}
?>
