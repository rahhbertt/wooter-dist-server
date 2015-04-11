#!/usr/bin/php

<?php
	include "user_mgmt.php";
  
	// create a dummy user to send over
	$user_obj=new User;
	$user_obj->username="user_name_net";
	$user_obj->password="pass_wd_net";
	$user_obj->full_name="Full_User_Name And_Last_Name";
	$user_obj->age="133";
	$user_obj->email="test_email@emails.emails";

	create_new_user($user_obj);
	read_user($user_obj->username);
	$user_obj->username="user_name_net_no_more";
	flush_user($user_obj);
	$other_user=new User;
	$other_user=read_id(rand()%40);
	$third_user=new User;
	$third_user=read_id((99+ 6));
	$fourth_user=read_id(rand()%40);
	if(isset($third_user)) {
		follow($other_user, $third_user);
		unfollow($other_user, $third_user);
	}
	
	$id_list_fe=load_following($other_user, "followees", 10); 
	echo PHP_EOL;
	print_r($id_list_fe);
	$fe_cult=array();
	if(!empty($id_list_fe)){
		for($index=0; $index<10;$index++){
			$temp_usr=read_id($id_list_fe[$index]);
			array_push($fe_cult, $temp_usr);
		}
	}
	echo PHP_EOL;
	print_r($fe_cult);
	
	echo "\n\nInvalid user below:\n\n";
	print_r($third_user);
	$fr_cult=array();
	if(isset($third_user->username)){
		$id_list_fr=load_following($third_user, "followers", 12); 
		echo PHP_EOL;
		print_r($id_list_fr);
		if(!empty($id_list_fr)){
			for($index=0; $index<10;$index++){
				$temp_usr=read_id($id_list_fr[$index]);
				array_push($fr_cult, $temp_usr);
			}
		}
		echo PHP_EOL;
		print_r($fr_cult);
	}
	else{
		echo "\n\nAn empty user has been passed to read from\n";
	}	
	$woot="This is a network tested woot message. no new lines allowed";
	$timestamp="1973_77_22_33";
	$other_user=write_woot($other_user, $woot, $timestamp);

	$fe_woots=array();
	for($index=0; $index<10; $index++){ 
		if(!empty($fe_cult)){ 
			array_push($fe_woots, load_woots($fe_cult[$index], 1) ); 
		}
	}	
	for($index=0; $index<10; $index++){
		echo"\n\nUser:".($fe_cult[$index]->username)." name: ".($fe_cult[$index]->full_name);
		echo "\nage: ".($fe_cult[$index]->age)." most recent woot: ".$fe_woots[$index][0]->message;
		echo "\n most recent woot timestamp:".$fe_woots[$index][0]->timestamp."";
	}
	
	$fr_woots=array();
	for($index=0; $index<10; $index++){ 
		if(!empty($fr_cult)){ 
			array_push($fr_woots, load_woots($fr_cult[$index], 1) ); 
		}
	}	
	
	if(isset($third_user->username)){
		for($index=0; $index<10; $index++){
			echo"\n\nUser:".($fr_cult[$index]->username)." name: ".($fr_cult[$index]->full_name);
			echo "\nage: ".($fr_cult[$index]->age)." most recent woot: ".$fr_woots[$index][0]->message;
			echo "\n most recent woot timestamp:".$fr_woots[$index][0]->timestamp."";
		}
	}
	else{
		echo "\n\nAn empty user has been passed to read from\n";
	}
	
	$woots_2=load_woots($other_user, $other_user->num_woots +1);
	print_r($woots_2);
	// now load woots many


?>
