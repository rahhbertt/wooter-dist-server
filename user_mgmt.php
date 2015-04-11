<doctype! html>
<html>
<head></head>
<body>

<?php
class Woot{ public $message, $timestamp; }
class User{
	public $username, $password, $id, $full_name, $age, $email;
	public $followers, $followees, $free_bit, $num_woots;
}
$PORT_NUM=13013;
$SEND_SIZE=560;
$TIME_OUT=500;

// the following max values are hard coded into registration.html to not have to change it to .php
$MIN_USER_LEN=8;	$MAX_USER_LEN=24+1; 
$MIN_PWD_LEN=10;	$MAX_PWD_LEN=32+1; 
$MAX_ID_LEN=10+1; // at most 10 digits for the # of users
$MAX_FNM_LEN=25;	$MAX_LNM_LEN=25+1;	$MAX_EML_LEN=100+1;
$MAX_NUM_WOOTS=10; // at most 10 digits for the # of woots per user
// age is hardcoded to 4, 3 digits + 1 pad
$MAX_FLR_LEN=10+1;	$MAX_FLE_LEN=10+1; // at most 10 digits for the # of followees
$MAX_ULINE_LEN=$MAX_USER_LEN+$MAX_PWD_LEN+$MAX_ID_LEN+$MAX_FNM_LEN+1+$MAX_LNM_LEN+4+$MAX_EML_LEN+$MAX_FLR_LEN+$MAX_FLE_LEN+1+$MAX_NUM_WOOTS+1;
$USERS_PER_FILE=10;		$FID_HDR_LEN=5;		$FLRS_PER_LINE=10;
$MAX_FLR_LINE_LEN=($MAX_ID_LEN*$FLRS_PER_LINE)+1;
$MY_DELIMITER=".";
$WOOTS_PER_LINE=10;		$MAX_WOOT_LEN=100;		$MAX_WOOT_TMSTP=22;
$MAX_WOOT_LINE_LEN=$WOOTS_PER_LINE*($MAX_WOOT_LEN+1+$MAX_WOOT_TMSTP+1);

// interfaces to client functions
function create_new_user($user_obj){	
	/*
	create_new_user() takes in a user object assembled from the registration page and fully
	integrates that user into the system. This is done by finding it a free user ID and by
	writing the user's attributes to the first available spot in the user file base.
	* The user object is then returned for convenience.
	*/
	global $PORT_NUM;
	return real_create_new_user($user_obj, $PORT_NUM);
}

function flush_user($user_obj){
	/*
	flush_user() writes the attributes of $user_obj one by one into the file $file_handle
	at the offset $offset. Each attribute is padded to meet its maximum field size
	so that each field, line, and user file are of a fixed, constant size.
	*/
	global $PORT_NUM;
	return real_flush_user($user_obj, $PORT_NUM);
}

function read_user($username){
	/*
	read_user() takes in a username string, then finds it in the user file base and returns its user object.
	If the user does not exist in the user file base, a void return takes place.
	*/
	global $PORT_NUM;
	return real_read_user($username, $PORT_NUM);
}

function read_id($id_num){
	/*
	read_id() takes in a username id #, then finds it in the user file base and returns its user object.
	If the user does not exist in the user file base, a void return takes place.
	This function allows for O(1) look up for followers/followees instead of an O(n) scan.
	*/
	global $PORT_NUM;
	return real_read_id($id_num, $PORT_NUM);
}

function follow($home_user, $other_user){
	/*
	follow() creates a "is following" relationship between the logged in $home_user and the target $other_user.
	A reciprocating "is being followed by" is created between the target user and logged in user.
	*/
	global $PORT_NUM;
	return real_follow($home_user, $other_user, $PORT_NUM);
}

function unfollow($home_user, $other_user){
	/*
	unfollow() takes in a logged in $home_user and a target $other_user and destroys any "is following"
	relationship from the first to the second. It also destroys any reciprocating "is being followed by"
	relationship from the second to the first.
	An "lost follower" leaves an empty ID field for that portion of the file, which can then be recycled
	by future follow() attempts using first_free to find said empty field.
	* This is to prevent subsequent follow/unfollows from arbitrarily growing the # of files tracking this information.
	*/	
	global $PORT_NUM;
	return real_unfollow($home_user, $other_user, $PORT_NUM);
}

function load_following($home_user, $string, $num){
	/* 
	load_following() takes in the logged in user object, the kind of following (followers 
	or followees), the # to read in, and then returns an array of those user IDs.
	*/
global $PORT_NUM;
return real_load_following($home_user, $string, $num, $PORT_NUM);
}

function write_woot($home_user, $woot, $timestamp){
	/*
	write_woot() takes in the logged in user object, the woot to write, and the timestamp
	associated with that woot and writes this to the woot file base.
	*/
	global $PORT_NUM;
	return real_write_woot($home_user, $woot, $timestamp, $PORT_NUM);
}

function load_woots($home_user, $num_woots){
	/*
	load_woots() takes in the logged in user object, the number of woots to load, and returns
	an array of that many woots starting from the most recent one.
	*/
	global $PORT_NUM;
	return real_load_woots($home_user, $num_woots, $PORT_NUM, "n"); 
}

function load_woots_seq($home_user, $num_woots){
	/*
	load_woots_seq() takes in the logged in user object, the number of woots to load, and returns
	an array of that many woots starting from the least recent one.
	*/
	global $PORT_NUM;
	return real_load_woots($home_user, $num_woots, $PORT_NUM, "y"); 
}

function already_following($home_user, $other_id){
	global $PORT_NUM;
	return real_already_following($home_user, $other_id, $PORT_NUM);
}

// internal functions that handle communicating with network server
function format($some_str, $length){
	while(strlen($some_str)< $length){ $some_str=$some_str.' '; }
	return $some_str;
}

function user_explode($user_obj){
	global $MAX_USER_LEN, $MAX_PWD_LEN, $MAX_ID_LEN, $MAX_FNM_LEN, $MAX_LNM_LEN, $MAX_EML_LEN, $MAX_FLR_LEN, $MAX_FLE_LEN, $MAX_NUM_WOOTS, $MAX_USER_LEN;
	$exploded="";
	if(isset($user_obj)) { // if an empty user, just send over an empty string
		$exploded=$exploded.str_pad($user_obj->username, $MAX_USER_LEN, " ");
		$exploded=$exploded.str_pad($user_obj->password, $MAX_PWD_LEN, " ");
		$exploded=$exploded.str_pad($user_obj->id, $MAX_ID_LEN, " ");
		$exploded=$exploded.str_pad($user_obj->full_name, $MAX_FNM_LEN+$MAX_LNM_LEN+1, " ");
		$exploded=$exploded.str_pad($user_obj->age, 4, " ");
		$exploded=$exploded.str_pad($user_obj->email, $MAX_EML_LEN, " ");
		$exploded=$exploded.str_pad($user_obj->followers, $MAX_FLR_LEN, " ");
		$exploded=$exploded.str_pad($user_obj->followees, $MAX_FLE_LEN, " ");
		$exploded=$exploded.$user_obj->free_bit;
		$exploded=$exploded.str_pad($user_obj->num_woots, $MAX_NUM_WOOTS, " ");	
	}
	else { $exploded=str_pad(" ", $MAX_USER_LEN); }
	return $exploded;	
}

function user_unexplode($user_obj, $user_str){
	global $MAX_USER_LEN, $MAX_PWD_LEN, $MAX_ID_LEN, $MAX_FNM_LEN, $MAX_LNM_LEN, $MAX_EML_LEN, $MAX_FLR_LEN, $MAX_FLE_LEN, $MAX_NUM_WOOTS;
	$offset=0;
	$user_obj->username=trim(substr($user_str, $offset, $MAX_USER_LEN));
	if(empty($user_obj->username)) { // if no user sent over, stop reading/unserializing here
		unset($user_obj); 
		return;
	}
	$offset+=$MAX_USER_LEN;
	$user_obj->password=trim(substr($user_str, $offset, $MAX_PWD_LEN));
	$offset+=$MAX_PWD_LEN;
	$user_obj->id=trim(substr($user_str, $offset, $MAX_ID_LEN) );
	$offset+=$MAX_ID_LEN;
	$user_obj->full_name=trim(substr($user_str, $offset, $MAX_FNM_LEN + $MAX_LNM_LEN +1));
	$offset+=$MAX_FNM_LEN+$MAX_LNM_LEN+1;
	$user_obj->age=trim(substr($user_str, $offset, 4));
	$offset+=4;
	$user_obj->email=trim(substr($user_str, $offset, $MAX_EML_LEN));
	$offset+=$MAX_EML_LEN;
	$user_obj->followers=trim(substr($user_str, $offset, $MAX_FLR_LEN) ); 
	$offset+=$MAX_FLR_LEN;
	$user_obj->followees=trim(substr($user_str, $offset, $MAX_FLE_LEN) ); 
	$offset+=$MAX_FLE_LEN;
	$user_obj->free_bit=trim(substr($user_str, $offset, 1) );
	$offset++;
	$user_obj->num_woots=trim(substr($user_str, $offset, $MAX_NUM_WOOTS) ); 
	return $user_obj;
}

function real_create_new_user($user_obj, $port){
	global $SEND_SIZE;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$request=format(format("create_new_user ",40).user_explode($user_obj), $SEND_SIZE );
	fwrite($fp, $request);
	$line=fgets($fp);
	$user_obj=user_unexplode($user_obj, $line);
	fclose($fp);
}

function real_read_user($user_name, $port){
	global $SEND_SIZE;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$request=format(format("read_user",40).$user_name, $SEND_SIZE );
	fwrite($fp, $request);
	$line=fgets($fp);
	$user_obj=new User;
	$user_obj=user_unexplode($user_obj, $line);
	fclose($fp);
	return $user_obj;
}

function real_flush_user($user_obj, $port) {
	global $SEND_SIZE;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$request=format(format("flush_user",40).user_explode($user_obj), $SEND_SIZE );
	fwrite($fp, $request);
	$line=fgets($fp);
	fclose($fp);
	
	$user_obj=new User;
	$user_obj=user_unexplode($user_obj, $line);
	return $user_obj; // potentially unnecessary
}

function real_read_id($id, $port){
	global $SEND_SIZE;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$request=format(format("read_id",40).$id, $SEND_SIZE );
	fwrite($fp, $request);
	$line=fgets($fp);
	fclose($fp);
	
	$user_obj=new User;
	$user_obj=user_unexplode($user_obj, $line);
	return $user_obj;
}

function real_follow($home_user, $other_user, $port){
	global $SEND_SIZE;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$request=format(format("follow",40).user_explode($home_user)." ".user_explode($other_user), $SEND_SIZE );
	fwrite($fp, $request);
	fclose($fp);
}

function real_unfollow($home_user, $other_user, $port){
	global $SEND_SIZE;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$request=format(format("unfollow",40).user_explode($home_user)." ".user_explode($other_user), $SEND_SIZE );
	fwrite($fp, $request);
	fclose($fp);
	
}

function real_load_following($home_user, $group_type, $num_ppl, $port){
	global $SEND_SIZE, $MAX_ID_LEN;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$ids_per_msg=(int)($SEND_SIZE/($MAX_ID_LEN+1));
	$request=format(format("load_following",40).user_explode($home_user)." ".$group_type." ".$num_ppl, $SEND_SIZE );
	fwrite($fp, $request); 
	$cult=array();
	$ids_received=0;
	while($line=fgets($fp)){ 
		$offset=0;
		for($current_id=0; $current_id<$ids_per_msg && $ids_received<$num_ppl; $current_id++, $ids_received++){
			$read_id=trim(substr($line, $offset, $MAX_ID_LEN));
			if($read_id=="") { break; }
			$offset+=$MAX_ID_LEN+1; // a space between it and the next one
			array_push($cult, $read_id);
		}
	}
	fclose($fp);	
	return $cult;
}

function cult_unexplode($line, $num_ppl){
	global $MAX_ID_LEN;
	$cult=array();
	$offset=0; // does not do any safety checking on $num_ppl
	for($index=0; $index<$num_ppl;$index++){
		$current_id=trim(substr($line, $offset, $MAX_ID_LEN));
		if(!empty(trim(substr($line, $offset, $MAX_ID_LEN)))){ array_push($cult, $current_id);	}
		$offset+=$MAX_ID_LEN;	
	}
	return $cult;
}

function real_write_woot($other_user, $woot, $timestamp, $port){
	global $SEND_SIZE, $MAX_WOOT_LEN, $MAX_WOOT_TMSTP;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$ready_woot=trim(preg_replace('/\s+/', ' ', $woot));	
	if(!empty($ready_woot)) { 
		$ready_woot=str_pad($ready_woot, $MAX_WOOT_LEN, " ");
		$request=format(format("write_woot",40).user_explode($other_user)." ".$ready_woot." ".str_pad($timestamp, $MAX_WOOT_TMSTP, " "), $SEND_SIZE );
		fwrite($fp, $request); 
		$line=fgets($fp);
		fclose($fp);
		$other_user=user_unexplode($other_user, $line);
	}
	return $other_user; // to update num_woots
}

function real_load_woots($other_user, $num_woots, $port, $seq){ 
	global $SEND_SIZE, $MAX_WOOT_LEN, $MAX_WOOT_TMSTP;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$woots_per_msg=(int)($SEND_SIZE/($MAX_WOOT_LEN+$MAX_WOOT_TMPSTP+1));
	if($seq=="y"){ $request=format(format("load_woots_seq",40).user_explode($other_user)." ".$num_woots, $SEND_SIZE ); }
	else { $request=format(format("load_woots",40).user_explode($other_user)." ".$num_woots, $SEND_SIZE );}
	fwrite($fp, $request); // assumes it works, that it must work
	$woots=array();
	$woots_received=0;
	while($line = fgets($fp)) {	
		$offset=0;
		for($current_woot=0; $current_woot<$woots_per_msg && $woots_received<$num_woots; $current_woot++, $woots_received++){ // 560 char, 124 char woots, 4 woots per line
			$read_woot=new Woot;
			$read_woot->message=trim(substr($line, $offset, $MAX_WOOT_LEN));
			if(empty($read_woot->message)) { break; }
			$offset+=$MAX_WOOT_LEN+1; // a space between it and the timestamp
			$read_woot->timestamp=trim(substr($line, $offset, $MAX_WOOT_TMSTP+1));
			$offset+=$MAX_WOOT_TMSTP+1; // a space between it and the next one
			array_push($woots, $read_woot);
		}
	}
	fclose($fp);
	return $woots;
}

function real_already_following($home_user, $other_id, $port){
	global $SEND_SIZE;
	$fp = stream_socket_client('localhost:'.$port, $errno, $errstr, $TIME_OUT);
	if (!$fp) {
		echo $errstr;
		exit(1);
	}
	$request=format(format("already_following",40).user_explode($home_user)." ".$other_id, $SEND_SIZE );
	fwrite($fp, $request);
	$line=fgets($fp);
	$answer=trim(substr($line, 0, 3));
	if( $answer != "YES"){ unset($answer); } // hardcoded YES or NO
	fclose($fp);
	return $answer;
	
}

?>



</body>
</html>
