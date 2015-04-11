<doctype! html>
<html>
<head></head>
<body>
	<?php
	class User{
		public $username, $password, $id, $full_name, $age, $email;
		public $followers, $followees, $free_bit;
		// add privacy here?
	}
	// the following max values are hard coded into registration.html to not have to change it to .php
	$MIN_USER_LEN=8;	$MAX_USER_LEN=24+1; 
	$MIN_PWD_LEN=10;	$MAX_PWD_LEN=32+1; 
	$MAX_ID_LEN=10+1; // at most 10 digits for the # of users
	$MAX_FNM_LEN=25;	$MAX_LNM_LEN=25+1;	$MAX_EML_LEN=100+1;
	// age is hardcoded to 4, 3 digits + 1 pad
	$MAX_FLR_LEN=10+1;	$MAX_FLE_LEN=10+1; // at most 10 digits for the # of followees
	$MAX_ULINE_LEN=$MAX_USER_LEN+$MAX_PWD_LEN+$MAX_ID_LEN+$MAX_FNM_LEN+1+$MAX_LNM_LEN+4+$MAX_EML_LEN+$MAX_FLR_LEN+$MAX_FLE_LEN+1+1;
	$USERS_PER_FILE=10;		$FID_HDR_LEN=5;		$FLRS_PER_LINE=10;
	$MAX_FLR_LINE_LEN=($MAX_ID_LEN*$FLRS_PER_LINE)+1;
	$MY_DELIMITER=".";

function flush_user($file_handle, $user_obj, $offset){
	/*
	flush_user() writes the attributes of $user_obj one by one into the file $file_handle
	at the offset $offset. Each attribute is padded to meet its maximum field size
	so that each field, line, and user file are of a fixed, constant size.
	*/
	global $MAX_USER_LEN, $MAX_PWD_LEN, $MAX_ID_LEN, $MAX_FNM_LEN, $MAX_LNM_LEN, $MAX_EML_LEN, $MAX_FLR_LEN, $MAX_FLE_LEN;
	$u_file=$file_handle;
	fseek($file_handle, $offset, SEEK_SET);
	fwrite($u_file, str_pad($user_obj->username, $MAX_USER_LEN, "."));
	fwrite($u_file, str_pad($user_obj->password, $MAX_PWD_LEN, "."));
	fwrite($u_file, str_pad($user_obj->id, $MAX_ID_LEN, "."));
	fwrite($u_file, str_pad($user_obj->full_name, $MAX_FNM_LEN+$MAX_LNM_LEN+1, "."));
	fwrite($u_file, str_pad($user_obj->age, 4, "."));
	fwrite($u_file, str_pad($user_obj->email, $MAX_EML_LEN, "."));
	fwrite($u_file, str_pad($user_obj->followers, $MAX_FLR_LEN, "."));
	fwrite($u_file, str_pad($user_obj->followees, $MAX_FLE_LEN, "."));
	fwrite($u_file, "n");
		echo "It is being written that the user: $user_obj->username has $user_obj->followers followers and $user_obj->followees followees<br>";

}

function unflush_user($file_handle){
	/*
	unflush_user() reads the attributes of $user_obj one by one from the file $file_handle
	at the given file position. Each attribute's padding is removed upon reading from file.
	*/
	global $MAX_USER_LEN, $MAX_PWD_LEN, $MAX_ID_LEN, $MAX_FNM_LEN, $MAX_LNM_LEN, $MAX_EML_LEN, $MAX_FLR_LEN, $MAX_FLE_LEN;
	$user_base=$file_handle;
	fseek($user_base, -$MAX_USER_LEN, SEEK_CUR);
	$user_obj=new User;
	$user_obj->username=trim(fread($user_base, $MAX_USER_LEN), ".");
	$user_obj->password=trim(fread($user_base, $MAX_PWD_LEN), ".");
	$user_obj->id=trim(fread($user_base, $MAX_ID_LEN), ".");
	$user_obj->full_name=trim(fread($user_base, $MAX_FNM_LEN+$MAX_LNM_LEN+1), ".");
	$user_obj->age=trim(fread($user_base, 4), ".");
	$user_obj->email=trim(fread($user_base, $MAX_EML_LEN), ".");
	$user_obj->followers=trim(fread($user_base, $MAX_FLR_LEN), ".");
	$user_obj->followees=trim(fread($user_base, $MAX_FLE_LEN), ".");
	$user_obj->free_bit=fread($user_base, 1);
	fclose($user_base);
	//~ print_r($user_obj);
	return $user_obj;
	
}

function fids_decrease($line_num){
	/*
	fids_decrease() takes a line number, which corresponds to a user file u_$line_num, and decreases
	the number of free spots/ids (fids) in that file. If $line_num is larger than the # of lines,
	a new line is written of $USERS_PER_FILE-1.
	*/
	//~ echo "Decreasing fids at line:$line_num<br>";
	global $USERS_PER_FILE;
	$fids=fopen("fids.txt", "r+") or die("Could not open file: fids.txt");
	fseek($fids, $line_num*(strlen((string)$USERS_PER_FILE)+1) );
	$current_free=(int)(fread($fids, strlen((string)$USERS_PER_FILE)));
	// handles eof by just writing there, as it should
	if($current_free==0){ $current_free=$USERS_PER_FILE-1; }
	else{
		$current_free--;
		fseek($fids, -2, SEEK_CUR);
	}
	//~ echo "WHAT WE WILL WRIET: $current_free<br>";
	// if this requires a brand new user file, make a new line in fids
	$to_write=str_pad((string)$current_free, strlen((string)$USERS_PER_FILE), ".");
	$to_write=$to_write.PHP_EOL;
	//~ echo "our to write: $to_write t";
	fwrite($fids, $to_write);
	fclose($fids);
}

function create_new_user($user_obj){	
	/*
	create_new_user() takes in a user object assembled from the registration page and fully
	integrates that user into the system. This is done by finding it a free user ID and by
	writing the user's attributes to the first available spot in the user file base.
	* The user object is then returned for convenience.
	*/
	
	global $USERS_PER_FILE, $MAX_ULINE_LEN;
	//~ $file_number=0;
	$user_id=-1;
	$current_line=0;
	
	if(!file_exists("fids.txt")){ 
		
		create_file("fids", 0, 0); 
		//~ echo "creating fids file<br>";
		}
	else{
		$fids=fopen("fids.txt", "r+");
		$current_free=(int)trim(fread($fids,2),".");
			//~ echo "$current_free <- curr free, $current_line <- curr line<br>";
	
		while( $current_free == 0 ) { 
			//~ echo "z $current_free <- curr free, $current_line <- curr line<br>";

						if(!feof($fids)){ fseek($fids, 1, SEEK_CUR);  
							//~ echo "we know<br>";
							} // move to the next line
			else { break; }
			$current_line++;
			$current_free=(int)trim(fread($fids,2),".");

		}
			//~ echo "It $current_line is being written that the user: $user_obj->username has $user_obj->followers followers and $user_obj->followees followees<br>";

		fclose($fids);
	}
	
	// current_line is now which user file you want
	// if it doesn't exist, make it, and decrease fids there, which is handled
	//~ while($file_number!=-1){ 
	$file_name="users/u_".$current_line.".txt";
	$current_offset=0;
	if(!file_exists($file_name)){  // make the file, flush the user, update fids
		//~ echo "<br> where are we <br>";
		$user_id=$current_line*$USERS_PER_FILE;
		//~ echo "USER ID VALUE IS $user_id<br>";
		create_file("users", $current_line, 0);		
		$u_file=fopen($file_name, "r+") or die("Could not open file: ".$file_name);
		$user_obj->id=$user_id;
		$user_obj->followers=0;
		$user_obj->followees=0;
		flush_user($u_file, $user_obj, 0); // write the user and their attributes to the file
		fclose($u_file);
		//~ echo "About to decrease fids by ".(string)(int)($user_obj->id/$USERS_PER_FILE)." <br>";
		fids_decrease((int)($user_obj->id/$USERS_PER_FILE));
		return $user_obj;
	}
	else { // if the file exists, it has free space for new users
		$u_file=fopen($file_name, "r+") or die("Could not open file: ".$file_name);
		$current_id=0;
		$bit_pos=$MAX_ULINE_LEN-1-1;
		fseek($u_file, $bit_pos);
		
		while($current_id<100*$USERS_PER_FILE){ // scan until you find first free spot
			$current_bit=fread($u_file, 1);
			//~ echo "$current_bit = curent bit<br>";
			if($current_bit=="."){
				fseek($u_file, -($bit_pos+1));
				$user_obj->id=$current_id+($USERS_PER_FILE*$current_line);
				//~ echo "USER ID VALUE IS $user_obj->id<br>";
				$user_obj->followers=0;
				$user_obj->followees=0;
				flush_user($u_file, $user_obj, ($current_id)*$MAX_ULINE_LEN);
				fclose($u_file);
					//~ echo "About to decrease fids by ".(string)(int)($user_obj->id/$USERS_PER_FILE)." <br>";
				fids_decrease((int)($user_obj->id/$USERS_PER_FILE));
				return $user_obj;
			}
			fseek($u_file, $MAX_ULINE_LEN-1, SEEK_CUR);
			$current_id+=1;
		}
	}
	$file_number+=1;
}
//~ }

function read_user($username){
	/*
	read_user() takes in a username string, then finds it in the user file base and returns it.
	If the user does not exist in the user file base, a void return takes place.
	*/
	global $USERS_PER_FILE, $MAX_ULINE_LEN, $MAX_USER_LEN;
	$file_number=0;
	while($file_number>-1){ // search every user file
		$file_name="users/u_".$file_number.".txt";
		if(file_exists($file_name)){
			//~ echo "test 1<br>";
			$user_base=fopen($file_name, 'r+') or die("Unable to open file:".$file_name);
			while(ftell($user_base)<$USERS_PER_FILE*$MAX_ULINE_LEN){
				//~ echo "test 2<br>";
				$read_uname=trim(fread($user_base, $MAX_USER_LEN), "."); // read in the username
				//~ echo "Read in: $read_uname versus our: $username<br>";
				if($read_uname == $username){ // if it matches what we want, build and return that user object
						$user_obj= new User;
						$user_obj=unflush_user($user_base);
						//~ echo "Returning $user_obj->username user<br>";
						return $user_obj;
				}
				else{
					fseek($user_base, $MAX_ULINE_LEN-$MAX_USER_LEN, SEEK_CUR); // if not a match, read next user
				}
			}
			fclose($user_base);
		}
		else { return; } // return nothing if not found
		$file_number+=1;
	}
}

function deactivate_user($user_id){
	// DOCUMENT CODE HERE
		//  if anybody tries to read you and it says "d", message that you deactivated

	// go to free_user[i] file based on UID mod, set that bit to "d". done
}

function create_file($file_type, $file_row, $file_column){
	/*
	create_file() creates a file of the given $file_type and uses $file_row and $file_column
    to satisfy a naming convention. User files are one dimensional, from u_0.txt to u_n.txt.
    Followers, followees, and woot files use the row dimension to mirror the user_files,
    and have a column dimension to allow for a fixed # objects per file per user.
	*/
	global $MAX_ULINE_LEN, $USERS_PER_FILE, $MY_DELIMITER, $FID_HDR_LEN, $MAX_ID_LEN, $FLRS_PER_LINE;
	$file_name="";
	
	if(!is_dir("users")) { mkdir("users"); }
	if(!is_dir("flwrs")) { mkdir("flwrs"); }
	if(!is_dir("flwes")) { mkdir("flwes"); }
	
	if($file_type=="users"){ $file_name="users/u_".$file_row.".txt"; }
	else if($file_type=="fids") { $file_name="fids.txt"; }
	else if($file_type=="woots") { $file_name="w_r".$file_row."c".$file_column.".txt"; }
	else if($file_type=="followers") { $file_name="flwrs/fr_r".$file_row."c".$file_column.".txt"; }
	else if($file_type=="followees") { $file_name="flwes/fe_r".$file_row."c".$file_column.".txt"; }
	if(file_exists($file_name)) { die("Could not create ".$file_name." as it already exists"); }
	
	$valid_type=( ($file_type=="users") || ($file_type=="fids") || ($file_type=="woots") || ($file_type=="followers") || ($file_type=="followees") );
	if($valid_type){
		if($file_type=="users"){
			$one_line=str_repeat($MY_DELIMITER, ($MAX_ULINE_LEN-1) ); // create an "empty" line of delimiters
			$num_lines=$USERS_PER_FILE;
		}
		else if($file_type=="fids"){
			$one_line=$USERS_PER_FILE; // that many users slots are free when the file is made
			$num_lines=1;
		}
		else if($file_type=="followers" || $file_type=="followees"){
			$one_line=str_repeat($MY_DELIMITER, ($MAX_ID_LEN*$FLRS_PER_LINE) );
			$num_lines=$USERS_PER_FILE;
		}
		$one_line=$one_line.PHP_EOL;
		$file_handle=fopen($file_name, "w") or die("Could not open file: ".$file_name);
		for($lines=0; $lines<$num_lines; $lines++){ fwrite($file_handle, $one_line); } // fill the file with empty line
		fclose($file_handle);
	}
}

function follow($home_user, $other_user){
	/*
	follow() creates a "is following" relationship between the logged in $home_user and the target $other_user.
	A reciprocating "is being followed by" is created between the target user and logged in user.
	*/
	global $FLRS_PER_LINE, $MAX_FLR_LINE_LEN, $USERS_PER_FILE, $MAX_ID_LEN, $MAX_ULINE_LEN;	
	$followee_id=$other_user->id;
	$follower_id=$home_user->id;
	list ($fnm, $lnm, $flr_num) = already_following($home_user, $followee_id);
	if(!isset($flr_num)){
		echo "You have left the already following function for user: $home_user->username following user ID: $followee_id <br>";
		list ($file_name, $line_num, $col_num, $col_file_num)= first_free($home_user, "followees");		
		if($file_name==-1){ // file does not exist and no free spots found
			echo "No file was found with any free spots<br>";
			$row_val=(int)($home_user->id/$USERS_PER_FILE);
			$file_name="flwes/fe_r".$row_val."c".$col_file_num.".txt";
			create_file("followees", $row_val , $col_file_num ); 
			$col_num=0;
		} // otherwise you return a file name and col num, colfilenum in the filename
		echo "You have left the first free function with file: ".$file_name." line #: ".$line_num." col #: ".$col_num." col_file_#: ".$col_file_num."<br>";		
		$following=fopen($file_name, "r+") or die("Could not open file: ".$file_name);		
		fseek($following, ($line_num*$MAX_FLR_LINE_LEN)+($MAX_ID_LEN*$col_num));
		fwrite($following, $followee_id);
		fclose($following);

		$home_user->followees++;
		$u_file="users/u_".(int)($home_user->id/$USERS_PER_FILE).".txt";
		$user_base=fopen($u_file, "r+") or die("Could not open file: ".$u_file);
		flush_user($user_base, $home_user, ($home_user->id%$USERS_PER_FILE)*$MAX_ULINE_LEN);
		fclose($user_base);
	}
	list ($fnm, $lnm, $fle_num) = already_followed_by($other_user, $follower_id);
	if(!isset($fle_num)){
		echo "You have left the already followed by function for user: $other_user->username followed by user ID: $follower_id <br>";
		list ($file_name, $line_num, $col_num, $col_file_num)= first_free($other_user, "followers");		
		if($file_name==-1){ // file does not exist and no free spots found
			echo "No file was found with any free spots<br>";
			$row_val=(int)($other_user->id/$USERS_PER_FILE);
			$file_name="flwrs/fr_r".$row_val."c".$col_file_num.".txt";
			create_file("followers", $row_val , $col_file_num ); 
			$col_num=0;
		} // otherwise you return a file name and col num, colfilenum in the filename
		echo "You have left the first free function with file: ".$file_name." line #: ".$line_num." col #: ".$col_num." col_file_#: ".$col_file_num."<br>";		
		$following=fopen($file_name, "r+") or die("Could not open file: ".$file_name);		
		fseek($following, ($line_num*$MAX_FLR_LINE_LEN)+($MAX_ID_LEN*$col_num));
		fwrite($following, $follower_id);
		fclose($following);
		
		$other_user->followers++;
		$u_file="users/u_".(int)($other_user->id/$USERS_PER_FILE).".txt";
		$user_base=fopen($u_file, "r+") or die("Could not open file: ".$u_file);
		flush_user($user_base, $other_user, ($other_user->id%$USERS_PER_FILE)*$MAX_ULINE_LEN);
		fclose($user_base);
	 }
 }

function first_free($user_obj, $follow_type){
	/*
	first_free() takes in a user and a string indicating either "followers" or "followees".
	It then returns the first location where a new follower/followee's ID can be recorded.
	This can indicate one of three cases: 1) a previous follower has unfollowed, leaving a gap,
	2) the user has no free gaps and a new file must be created, 3) or the file already
	exists but the user has simply not expanded into filling it.
	*/
	global $USERS_PER_FILE, $FLRS_PER_LINE, $MY_DELIMITER, $MAX_ID_LEN;
	echo "<br><br>You are in the first_free function for user: $user_obj->username<br>";
	$user_row=(int)($user_obj->id/$USERS_PER_FILE);
	$current_column=0;
	if($follow_type=="followees") { $type="e"; }
	else if($follow_type=="followers"){ $type="r"; }

	$file_name="flw".$type."s/f".$type."_r".$user_row."c".$current_column.".txt";
	echo "You are about to check if the file: $file_name exists.<br>";
	echo "The number of followe".$type."s is: ";
	if($follow_type=="followees") { $group=$user_obj->followees; }
	else if ($follow_type=="followers") { $group=$user_obj->followers; }
	echo $group."<br>";
	$followees_read=0;
	$line_num=$user_obj->id%$USERS_PER_FILE;
	while( file_exists($file_name) && $followees_read < $group){
		$following=fopen($file_name, "r") or die("Could not open file ".$file_name);
		fseek($following, $line_num*(1+$FLRS_PER_LINE*$MAX_ID_LEN));
		$flwes_in_cur_file=$FLRS_PER_LINE; 
		echo "There are $flwes_in_cur_file followe".$type."s in the current file<br>";
		echo "You have read $followees_read followe".$type."s<br>";		
		for($current_fle=0; $current_fle<$flwes_in_cur_file; $current_fle++){
			echo "You are currently checking column ". $current_fle . " <br>";		
			$current_id=trim(fread($following, $MAX_ID_LEN), $MY_DELIMITER);
			if($current_id=="") { 
				echo "Column ".($current_fle)." (value: $current_id) WAS a match an empty spot <br>";
				return array($file_name, $line_num, $current_fle, $current_column); 
			}
			echo "Column ".($current_fle)." (value: $current_id) was not a match an empty spot <br>";
			if($current_id!="") { $followees_read++; }
		}
		echo "You have exited the for loop as you have hit the # of flw".$type."s_in_cur_file:$flwes_in_cur_file<br>";
		fclose($following);
		$current_column++;
		$file_name="flw".$type."s/f".$type."_r".$user_row."c".$current_column.".txt";
	}
	if(file_exists($file_name)) { return array($file_name, $line_num, 0, $current_column); }
	else { return array(-1,$line_num,-1,$current_column); }
}

function already_following($home_user, $followee_id){
	/*
	already_following() takes in a logged in $home_user, the ID of the user they want to follow,
	and returns an array of the file name, file line #, and line position where that ID already
	exists in the user's list of followees. If the logged in user is not currently following that ID,
	then a void return takes place.
	*/
	global $USERS_PER_FILE, $FLRS_PER_LINE, $MY_DELIMITER, $MAX_ID_LEN;
	echo "<br><br>You are in the already following function for user: $home_user->username following user ID: $followee_id <br>";
	$user_row=(int)($home_user->id/$USERS_PER_FILE);
	$current_column=0;
	$file_name="flwes/fe_r".$user_row."c".$current_column.".txt";
	echo "You are about to check if the file: $file_name exists.<br>";
	echo "The number of followees is: " . $home_user->followees ."<br>";
	$followees_read=0;
	while( file_exists($file_name) && $followees_read < $home_user->followees){
		$following=fopen($file_name, "r");
		$line_num=$home_user->id%$USERS_PER_FILE;
		fseek($following, $line_num*(1+$FLRS_PER_LINE*$MAX_ID_LEN));
		$flwes_in_cur_file=$FLRS_PER_LINE; 
		echo "There are $flwes_in_cur_file followers in the current file<br>";
		echo "You have read $followees_read followers<br>";		
		for($current_fle=0; $current_fle<$flwes_in_cur_file; $current_fle++){
			echo "You are currently checking column ". $current_fle . " <br>";		
			$current_id=trim(fread($following, $MAX_ID_LEN), $MY_DELIMITER);
			if(($current_id)==(string)($followee_id)) { 
				echo "Column ".($current_fle)." (followee's id: $current_id) WAS a match to followee id: $followee_id <br>";
				return array($file_name, $line_num, $current_fle); 
			}
			echo "Column ".($current_fle)." (followee's id: $current_id) was not a match to followee id: $followee_id <br>";
			if($current_id!="") { $followees_read++; }
		}
		echo "You have exited the for loop as you have hit the # of flwes_in_cur_file:$flwes_in_cur_file<br>";
		fclose($following);
		$current_column++;
		$file_name="flwes/fe_r".$user_row."c".$current_column.".txt";
	}
	return;
}

function already_followed_by($other_user, $follower_id){
	/*
	already_followed_by() takes in a target user $other_user, the ID of the logged in user wanting to follow them,
	and returns an array of the file name, file line #, and line position where that ID already
	exists in the target user's list of followers. If the target user is not currently being followed by the logged
	in user, then a void return takes place.
	*/
	global $USERS_PER_FILE, $FLRS_PER_LINE, $MY_DELIMITER, $MAX_ID_LEN;
	echo "<br><br>You are in the already followed function for user: $other_user->username followed by user ID: $follower_id<br>";
	$user_row=(int)($other_user->id/$USERS_PER_FILE);
	$current_column=0;
	$file_name="flwrs/fr_r".$user_row."c".$current_column.".txt";
	echo "You are about to check if the file: $file_name exists.<br>";
	echo "The number of followers is: " . $other_user->followers."<br>";
	$followers_read=0;
	while( file_exists($file_name) && $followers_read < $other_user->followers){
		$following=fopen($file_name, "r");
		$line_num=$other_user->id%$USERS_PER_FILE;
		fseek($following, $line_num*(1+$FLRS_PER_LINE*$MAX_ID_LEN));
		echo "You are about to enter the for loop for file: $file_name.<br>";
		$flwrs_in_cur_file=$FLRS_PER_LINE; 
		echo "There are $flwrs_in_cur_file followers in the current file<br>";
		echo "You have read $followers_read followers<br>";
		for($current_flr=0; $current_flr<$flwrs_in_cur_file; $current_flr++){
			echo "You are currently checking column ". $current_flr . " <br>";
			$current_id=trim(fread($following, $MAX_ID_LEN), $MY_DELIMITER);
			if(($current_id)==(string)($follower_id)) { 
				echo "Column ".($current_flr)." (follower's id: $current_id) WAS a match to follower id: $follower_id <br>";
				return array($file_name, $line_num, $current_flr); 				
			}
			echo "Column ".($current_flr)." (follower's id: $current_id) was not a match to follower id: $follower_id <br>";
			if($current_id!="") { $followers_read++; }
		}
		echo "You have exited the for loop as you have hit the # of flwrs_in_cur_file:$flwrs_in_cur_file<br>";
		fclose($following);
		$current_column++;
		$file_name="flwrs/fr_r".$user_row."c".$current_column.".txt";

	}
	return;
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
	global $FLRS_PER_LINE, $MAX_FLR_LINE_LEN, $USERS_PER_FILE, $MAX_ID_LEN, $MAX_ULINE_LEN, $MY_DELIMITER;	
	$followee_id=$other_user->id;
	$follower_id=$home_user->id;
	list ($fnm, $lnm, $fle_num) = already_following($home_user, $followee_id);
	if(isset($fle_num)){
		echo "Currently in unfollow's already following with flenum: $fle_num <br>";
		$following=fopen($fnm, "r+") or die("Could not open file:".$fnm);
		fseek($following, ($lnm*(1+$FLRS_PER_LINE*$MAX_ID_LEN)) + ($MAX_ID_LEN*$fle_num));
		fwrite($following, str_repeat($MY_DELIMITER, $MAX_ID_LEN)); // remove them from your followees
		fclose($following);

		$home_user->followees--; // decrease your number of followees
		$u_file="users/u_".(int)($home_user->id/$USERS_PER_FILE).".txt";
		$user_base=fopen($u_file, "r+") or die("Could not open file: ".$u_file);
		flush_user($user_base, $home_user, ($home_user->id%$USERS_PER_FILE)*$MAX_ULINE_LEN);
		fclose($user_base);
	}
	list ($fnm_2, $lnm_2, $flr_num) = already_followed_by($other_user, $follower_id);
	if(isset($flr_num)){ // if that user is already being followed by you
		$following=fopen($fnm_2, "r+") or die("Could not open file: $fnm");
		fseek($following, ($lnm_2*(1+$FLRS_PER_LINE*$MAX_ID_LEN)) + ($MAX_ID_LEN*$flr_num));
		fwrite($following, str_repeat($MY_DELIMITER, $MAX_ID_LEN)); // remove yourself from their followers
		fclose($following);
		
		$other_user->followers--; // decrease their number of followers
		$u_file="users/u_".(int)($other_user->id/$USERS_PER_FILE).".txt";
		$user_base=fopen($u_file, "r+") or die("Could not open file: ".$u_file);
		flush_user($user_base, $other_user, ($other_user->id%$USERS_PER_FILE)*$MAX_ULINE_LEN);
		fclose($user_base);
	}

}

?>



</body>
</html>
