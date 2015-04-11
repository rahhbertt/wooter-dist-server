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
	$MIN_USER_LEN=8;
	$MAX_USER_LEN=24+1; // for separator pad

	$MIN_PWD_LEN=10;
	$MAX_PWD_LEN=32+1; 

	$MAX_ID_LEN=10+1; // at most 10 digits for the # of users

	$MAX_FNM_LEN=25;
	$MAX_LNM_LEN=25+1;
	$MAX_EML_LEN=100+1;

// age is hardcoded to 4, 3 digits + 1 pad

	$MAX_FLR_LEN=10+1; // at most 10 digits for the # of followers
	$MAX_FLE_LEN=10+1; // at most 10 digits for the # of followees

	$MAX_ULINE_LEN=$MAX_USER_LEN+$MAX_PWD_LEN+$MAX_ID_LEN+$MAX_FNM_LEN+1+$MAX_LNM_LEN+4+$MAX_EML_LEN+$MAX_FLR_LEN+$MAX_FLE_LEN+1+1;
	$USERS_PER_FILE=10;
	$FID_HDR_LEN=5;

	$MY_DELIMITER=".";

	// each user field, delimited by a single char, a 1 char free_bit field, a delimiter, and a line end delimiter

	// verify that register.php works with the new GLOBALS set here for max length

	// when want to play around with line lengths and users per file, maybe use a batch file to call the php and fill it up
	// with garbage/random values for all of its fields, and set values small to like 2 users a file and 3 followers a line so can see it quickly scale

	// ALSO CHANGE LOG IN PHP SO THAT USERS DONT -HAVE- to be logged in to see tweets if the ?user= get exists
	// 	this way we have public tweets forever persisting

	/*
	register.php -> pass the fields to the next .php -> create a user object with those fields
	-> create a user ID (or find a free one) for that user -> write to that user ID's file and file_line
	*/

	// should be doing $user_obj=read_user($username), if(!isset($user_obj)) { error that user doesn't exist } else{ check password; }


	// before doing so should verify that user does not already exist


	function flush_user($file_handle, $user_obj, $offset){
		global $MAX_USER_LEN, $MAX_PWD_LEN, $MAX_ID_LEN, $MAX_FNM_LEN, $MAX_LNM_LEN, $MAX_EML_LEN, $MAX_FLE_LEN, $MAX_FLR_LEN, $FID_HDR_LEN;
		$u_file=$file_handle;
		fseek($file_handle, $offset);
		fwrite($u_file, str_pad($user_obj->username, $MAX_USER_LEN, "."));
		fwrite($u_file, str_pad($user_obj->password, $MAX_PWD_LEN, "."));
		fwrite($u_file, str_pad($user_obj->id, $MAX_ID_LEN, "."));
		fwrite($u_file, str_pad($user_obj->full_name, $MAX_FNM_LEN+$MAX_LNM_LEN+1, "."));
		fwrite($u_file, str_pad($user_obj->age, 4, "."));
		fwrite($u_file, str_pad($user_obj->email, $MAX_EML_LEN, "."));
		fwrite($u_file, str_pad($user_obj->followers, $MAX_FLR_LEN, "."));
		fwrite($u_file, str_pad($user_obj->followees, $MAX_FLE_LEN, "."));
		fwrite($u_file, "n");
	}
	/*
	create_user_id() finds and returns the first free user ID by scanning through all segments of the
	segmented list of free user IDs. 
	1. It does so by first checking the first integer in each file, which indicates the
	 # of free IDs, and only then doing a linear scan. The integer at the top of each 
	 segment saves the inevitable O(n) read of every full segment once segments start to fill up.
	2. If no free IDs exist in all segments of the list of free IDs, create_user_id() will
	 create a new segment for the list (as well as a corresponding user_base file to expand
	 the existing userbase) and give the user the first ID in that segment.

	Free ID List - 
	Index density:
	The free ID list is densely indexed to allow for O(1) user deletion, and overall simplicity.
	 Sparse indexing would allow for potentially faster look up of free IDs, but would take up
	 more space (using an entire string per user rathe than a single char), have O(n) deletion
	 and a more time and space intensive initializing of a new list segment.
	
	Overall Performance:
	It will take at most a full read of one list segment, plus the time taken
	 to read in the first int of every preceding free ID segment, to find a free ID. 
	 Otherwise a new segment will be created and the first ID available there will be returned.
	 Since segments are sufficiently large, and users are often deleted, this should not take
	 an excessive amount of file opening.
	*/
	// n for not free, f for free, d for deactivated
	// add a die to every fopen						


	// 1.a if it does not, create it
		//1.a.1 call create_file("users", i, 0)
		//1.a.2 assign user id to be i*max_users_per file
			// so if create file #2 (0 and 1 exist), 10k per file, user_id=20k
			// since users indexed from 0
		//1.a.3 call create_file("free_ids", i, 0)
			// seek to set the user_id % total_users_per_file to "y" for "yes this is active"
			// decrement digit at top to indicate a non-free ID
		//1.a.4 break out of the look over freeids_[i]
	// 1.b file does exist
		// 1.b.1 in this for loop going through all free_id[i] files
			 // read first special digit (digit\n) to check num_free_IDs
			 // if zero, continue to the next free_id file in the loop
			 // if non-zero, linearly scan with a for loop/counter for first free id
			 // if none are found, output an error?
			 // if found, assign this user that ID
			 // mark that ID bit using seek to "y"
	// return the user ID, should always succeed 

	 function create_new_user($user_obj){	
	 	global $MAX_USER_LEN, $MAX_PWD_LEN, $MAX_ID_LEN, $MAX_FNM_LEN, $MAX_LNM_LEN, $MAX_EML_LEN, 
	 	$MAX_FLE_LEN, $MAX_FLR_LEN, $FID_HDR_LEN, $USERS_PER_FILE, $MAX_ULINE_LEN;
	 	$file_number=0;
	 	$user_id=-1;
	 	while($file_number!=-1){
	 		$file_name="fids/fid_".$file_number.".txt";
	 		$current_offset=0;
	 	if(!file_exists($file_name)){ // if checked all free_ids list, and no free ids, make a new one
	 		$user_id=$file_number*$USERS_PER_FILE;
			//
	 		create_file("users", $file_number, 0);
	 		$u_file=fopen("users/u_".$file_number.".txt", "r+");
	 		$user_obj->id=$user_id;
	 		$user_obj->followers=0;
	 		$user_obj->followees=0;
	 		flush_user($u_file, $user_obj, 0);
	 		fclose($u_file);
	 		//
	 		create_file("free_ids", $file_number, 0);
	 		$fid_file=fopen($file_name, "r+");
	 		fseek($fid_file, ($FID_HDR_LEN+1) ); // goes to first user ID spot since just made the file
	 		fwrite($fid_file, "n"); 
			//
	 		return $user_id;
	 	}
	 	else { // for every free_id file that exists
	 		$fid_file=fopen($file_name, "r+");
	 		$header=(int)trim(fread($fid_file, $FID_HDR_LEN+1), ".");
	 		//~ echo "<br> header value" . $header."<br>";
	 				$current_id=0;
	 		if($header!=0){ // if it has any free ids, linearly scan for first one
	 			while(!feof($fid_file)){ 
	 				$current_bit=fread($fid_file, 2);
//~ CUT OUT THIS DEBUGGING LINE 
//~ output error if find no free idea somehow??
	 				//~ echo "<br> current bit: " . $current_bit ." <br>";
	 				if($current_bit=="f."){
	 					fseek($fid_file, -2, SEEK_CUR);
	 					fwrite($fid_file, "n.");
	 					fseek($fid_file, 0);
	 					$header=$header-1;
	 					fwrite($fid_file, str_pad($header, $FID_HDR_LEN, "."));
	 					fclose($fid_file);
	 					$u_file=fopen("users/u_".$file_number.".txt", "r+") or die("Something has gone wrong");
	 					$user_obj->id=$current_id+($USERS_PER_FILE*$file_number);
//~ CUT OUT THIS DEBUGGING LINE 
	 					//~ echo "<br> user id:" .$user_obj->id. "\t$MAX_ULINE_LEN <br>";
	 					$user_obj->followers=0;
	 					$user_obj->followees=0;
	 					flush_user($u_file, $user_obj, ($current_id)*$MAX_ULINE_LEN);
	 					fclose($u_file);
						//
	 					return true;
	 				}
	 				$current_id+=1;
	 			}
	 		}
			else { // there are no free ids, check next file
				$file_number+=1;
				continue;
			}
		}
	}
}
		// DOCUMENT THE CODE HERE
		// 1. for loop over every user file from the first one, checking username field
		// 1.a if(file exists)
		// 1.a.1 trim() the username field using built in php function? or write own?
		// 1.a.2 if a match, then check if( free_bit field is "n" ) 
		// 1.a.3 if a match, check if( free_bit field is "d")
			// then read in the user fields and construct a user obj. return that obj

// usernames shouldnt be CASE SENSITIVE! make sure when input, you lowercase() all that stuff
// for login and for register

function read_user($username){
	global $MAX_USER_LEN, $MAX_PWD_LEN, $MAX_ID_LEN, $MAX_FNM_LEN, $MAX_LNM_LEN, $MAX_EML_LEN, $MAX_FLE_LEN, $MAX_FLR_LEN, $FID_HDR_LEN,
	$MAX_ULINE_LEN, $USERS_PER_FILE;
	$file_number=0;
	while($file_number>-1){
		//~ echo "file num " . $file_number . "<br>";
		$file_name="users/u_".$file_number.".txt";
		if(file_exists($file_name)){
			$user_base=fopen($file_name, 'r+') or die("Unable to open file: $file_name!");
			//~ echo "currentZ Zuname: $read_uname<br> current pos in file: ". ftell($user_base) . "<br>";
			while(ftell($user_base)<$USERS_PER_FILE*$MAX_ULINE_LEN){
				$read_uname=trim(fread($user_base, $MAX_USER_LEN), ".");
								//~ $file_number++;
				if($read_uname == $username){
					//~ echo "currentZ Zuname: $read_uname<br> current pos in file: ". ftell($user_base) . "<br>";
					fseek($user_base, $MAX_ULINE_LEN-$MAX_USER_LEN-2, SEEK_CUR);
								//~ echo "currentZ Zuname: $read_uname<br> current pos in file: ". ftell($user_base) . "<br>";
					$is_free=fread($user_base, 1);
					//~ echo "freedom aint cheap " . $is_free . "<br>";
					fseek($user_base, 1, SEEK_CUR); // to get past that EOL
					if($is_free == "d") {
					//  make this prettier
						echo "This user has deactivated their account."; 
						return;
					}
					else if($is_free == "n") {
						//~ echo "<br> final countdown <br>";
						$user_obj=new User;
						$user_obj->username=$read_uname;
						//~ echo "<br> curr pos1:" . ftell($user_base) . "<br>";
						fseek($user_base, -$MAX_ULINE_LEN+$MAX_USER_LEN, SEEK_CUR);
						//~ echo "<br> curr pos2:" . ftell($user_base) . "<br>";
						$user_obj->password=trim(fread($user_base, $MAX_PWD_LEN), ".");
						$user_obj->id=trim(fread($user_base, $MAX_ID_LEN), ".");
						$user_obj->full_name=trim(fread($user_base, $MAX_FNM_LEN+1+$MAX_LNM_LEN), ".");
						$user_obj->age=trim(fread($user_base, 4), ".");
						$user_obj->email=trim(fread($user_base, $MAX_EML_LEN), ".");
						$user_obj->followers=trim(fread($user_base, $MAX_FLR_LEN), ".");
						$user_obj->followees=trim(fread($user_base, $MAX_FLE_LEN), ".");
						$user_obj->free_bit="n";
						//~ print_r($user_obj);
						fclose($user_base);
						return $user_obj;
					}
					else if($is_free == "f"){
						echo "This user has deleted their account.";
						fclose($user_base);
						return; 
					}
				}
				else{
					//~ echo "current uname: $read_uname<br> current pos in file: ". ftell($user_base) . "<br>";

					fseek($user_base, $MAX_ULINE_LEN-$MAX_USER_LEN, SEEK_CUR);
				}
			}
			fclose($user_base);
		}
		else {
		// 1.b if(!file exists)
			// stop looking, user definitely does not exist. return empty object? how is a proper way to do so?
			return;
		}
		$file_number+=1;
	}
		// 1.a.3 if a match, check if( free_bit field is "d")
			//  if so, output the standard deactivated message, be this a registration look up or a search_user look up
			// or a bad link look up or a modified GET link lookup
			//  ALSO MAKE SURE THE GET method checks for specialhtmlcharacters, else can mod get for malicious code

}

function write_user($user_object){
		// DOCUMENT THE CODE HERE


		// 1. take the (int division)of the $user_object->id to detemrine which user_base file to open
			// if user is a multiple of users_per_file, they are in line 0, not their USER ID line!
		// 1.b check that this file exists, else report an error
		// 2. use seek to write to the appropriate line_length * (user_id % users_per_file)
		// 2.b for each field, write padded($field, $field_size, $delimiter)
		// 2.c then write a "n" (for not free) as a 1 char field so when searching up a user can know
			// if deactivated, deleted, or active, without opening up the free IDs file
		// 2.d then write the line_delim_char
		// close the file
}

function delete_user($user_id){
		// DOCUMENT CODE HERE
		// this choice is made by the user in the session so have the UID here already

		// go to free_user[i] file based on UID mod, set that bit to "n". done
}

function deactivate_user($user_id){
	// DOCUMENT CODE HERE
		//  if anybody tries to read you and it says "d", message that you deactivated

	// go to free_user[i] file based on UID mod, set that bit to "d". done
}

function create_file($file_type, $file_row, $file_column){
	/*
	create_file() creates a file of the type specified for the purposes needed in this project,
	namely for users, woots, free_ids, or followers/followees, with a corresponding naming convention.
	
	Matrix File Naming -
	For woots, followers, and followees the naming convention requires a row and a column
	due to their nature of
	EXPLAIN REST HERE
	for followers and followees can get n^2, since n users, first follows n-1, seocnd n-2 ... really big
	really fast

	split up intro matrix so that have fixed length lines and can seek line base don user id quickly
	so one user with 100k followers isnt a monolithic file/linear scan to next new line character

	*/

	// 0. set up the global vars for whats described here
	// 1. check argument passed
		// if "users", set local function vars to global vars for user files
			// set rest of code to ignore COLUMNS if user file OR free_ids file
		// if "woots", set local vars to global woot var
		// if "free_ids", set local vars to free_id vars defined here
			// set rest of code to ignore COLUMNS if user file OR free_ids file
		// if "followers||followees", set vars to globals
			// then test if (followers) set file naming convention to X
			// if (followees) set file namign convention to Y
	global $MAX_ULINE_LEN, $USERS_PER_FILE, $MY_DELIMITER, $FID_HDR_LEN;
	$file_name="";
	if(!is_dir("users")){ mkdir("users"); }
	if(!is_dir("fids")) { mkdir("fids"); }

	if($file_type=="users"){ $file_name="users/u_".$file_row.".txt"; }
	else if($file_type="free_ids") { $file_name="fids/fid_".$file_row.".txt"; }
	else if($file_type="woots") { $file_name="w_r".$file_row."c".$file_column.".txt"; }
	else if($file_type="followers") { $file_name="fr_r".$file_row."c".$file_column.".txt"; }
	else if($file_type="followees") { $file_name="fe_r".$file_row."c".$file_column.".txt"; }

	if(file_exists($file_name)) { die("Something has gone terribly wrong."); }

	// check if the file exists already, if so, output an error
	// if not, define your variables for line length, lines per file, delimiter character
	// whether to write an int at the top with a \n or not
	// then write delim_char length_of_line*lines_per_file in one go
	//  then write every length_of_line'th multiple in bytes the LINE_DELIM char
	// free ID list segments have INTEGER\ny\n\y\y\n\d\n\y\y etc
	if($file_type=="users"){
		//~ echo "what file : " . $file_name . PHP_EOL;
		$one_line=str_repeat($MY_DELIMITER, ($MAX_ULINE_LEN-1) );
		$one_line=$one_line . PHP_EOL;
		$file_handle=fopen($file_name, "w");
		for($lines=0; $lines<$USERS_PER_FILE; $lines++){
			fwrite($file_handle, $one_line);
		}
		fclose($file_handle);
	}
	else if($file_type=="free_ids") {
		$header=str_pad((string)($USERS_PER_FILE-1), $FID_HDR_LEN, $MY_DELIMITER);
		// somehow check if this includes a null char at the en	d
		$header=$header.PHP_EOL;
		$file_handle=fopen($file_name, "w");
		fwrite($file_handle, $header);
		$one_line=str_repeat("f.", $USERS_PER_FILE); // y.n.d.y.n.n.y.y.y.n.n, etc
		fwrite($file_handle, $one_line);
		fclose($file_handle);
	}
/*
$MIN_USER_LEN=8;
$MAX_USER_LEN=24;

$MIN_PWD_LEN=10;
$MAX_PWD_LEN=32;

$MAX_ID_LEN=10; // at most 10 digits for the # of users

$MAX_FNM_LEN=25;
$MAX_LNM_LEN=25;
$MAX_EML_LEN=100;

$MAX_FLR_LEN=10; // at most 10 digits for the # of followers
$MAX_FLE_LEN=10; // at most 10 digits for the # of followees

$MAX_ULINE_LEN=$MAX_USER_LEN+1+$MAX_PWD_LEN+1+$MAX_ID_LEN+1+$MAX_FNM_LEN+1+$MAX_LNM_LEN+1+$MAX_EML_LEN+1+$MAX_FLR_LEN+1+$MAX_FLE_LEN+1+1+2;
$USERS_PER_FILE=10;


*/

	// DUE TO MANY FILES, SHOULD HAVE SEPARATE DIRECTORIES FOR user_base (u files + free_ids)
	//  followers_base, followees_base
	// also ask sterling if free_ids file separate, or just use that field in users anyhow and put integer at top of eahc users file

	// free_ids_i
	// u_i.txt no need to pad i and k for file sorting, since dont know how many will have and use direct #
	// w_ri_ck.txt so every X woots for every user same file
	// fr_ri_ck.txt for follower, fe_ri_ck for followee
	// thus creating a full file of "empty" lines, just delimiters, so we can have idempotent writes
		// and also fixed length lines for seek in O(1) time
// so fixed length fields, fixed length lines, fixed length files, variable # of files
	// fixed length files so file doesn't become so large that loading it is very slow

	// close the file
}


	//  AFTER USERS, IMPLEMENT FUNCTIONALITY FOR FOLLOWERS, FOLLOWEES, FOR WOOTS FILES
	//  then implement actually FOLLOWING and being followed by users and showing that on the page

	// need the # of followers so when lazy delete/deactivate, and not overwrite old followers with delimiters
		// can know when to stop reading, rather than going on to no end until empty fields
		 // in which case HAVE to overwrite deleted/deactivated, and that gets messy


	// FOR FOLLOWERS/FOLLOWEES for now implement a SEE ALL page, that also says 1-17 or 20-36 shown
		// in future, change this so that it pages, and can set it to view 5 users, 10 users, 20, 50
		//  and go through in pages. which will require dynamic button sizes, or reloading the button values
		//  with every submit? will this be too much? maybe

	// AFTER FOLLOWERS, FOLLOWEES, WOOTS, which are all very similar and fairly straightforward
		// finish documenting all the code





?>
</body>
</html>
