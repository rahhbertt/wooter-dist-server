

// NOTE: for most purposes of multi-threading testing and simulation, the same PHP testing code
// was used as before, with modified instances of the code being reloaded simultaneously in a browser
// to simulate multiple clients simultaneously connecting and then being threaded as requests.
// The multi_threaded tester code below was done in chunks to force and examine scenarios
// of many, many threads acting on the same or similar file objects and ensure
// a lack of deadlocks or race conditions.

// CODE TESTING FUNCTIONS
void function_tester(char cond='n'){
	User* usey = new User;
	usey->username="test_accy";
	usey->password="test_passwdy";
	usey->full_name="Test's_First_name Test's_Last_name";
	usey->age=25;
	usey->email="testemaily@emails.com";
	string original_name=usey->username;
	if(cond=='y'){
		for(int i=0; i <40; i++){ 
			stringstream fullest_name_ss;
			fullest_name_ss << original_name << i;
			usey->username=fullest_name_ss.str();
			fullest_name_ss.str("");
			create_new_user(usey); 
		}
		delete usey;
		usey=NULL;
	}
	User* usey2=unflush_user(FILE_PATH+"/users/u_1.txt", 4*MAX_ULINE_LEN);
	// unflush does not error check, assumes youve already found the user and are just pulling out
	
	cerr << endl << endl << "The results of unflushing u_1.txt at line # 4 are below: " << endl  
	<< usey2->username << "\t"  << usey2->password << "\t"  << usey2->id << "\t" 
	<< usey2->full_name << "\t"  << usey2->age << "\t"  << usey2->email << "\t" 
	<< usey2->followees << "\t"  << usey2->followers << "\t"  << usey2->free_bit << "\t" 
	<< usey2->num_woots << "\t" << endl;
	delete usey2;
	usey2=NULL;

	cerr << endl << endl << "The results of read_user()ing are below: " << endl  	;
	User* usey3=read_user("test_accy12"); // always check for nulll when read_user()ing
	if(usey3 != NULL){
		cerr << "The results of reading user test_accy12 are below: " << endl  
		<< usey3->username << "\t"  << usey3->password << "\t"  << usey3->id << "\t" 
		<< usey3->full_name << "\t"  << usey3->age << "\t"  << usey3->email << "\t" 
		<< usey3->followees << "\t"  << usey3->followers << "\t"  << usey3->free_bit << "\t" 
		<< usey3->num_woots << "\t" << endl;
		delete usey3;
		usey3=NULL;
	}
	else { cerr << "The user: test_acc12 could not be found" << endl; }
	
	cerr << endl << endl << "The results of read_id()ing are below: " << endl;	
	for(int i=0; i < 50; i++){
		User* usey4=read_id(i); // always check for null when read_user()ing
		if(usey4 != NULL){
			cerr << "The results of reading id # " << i << " are below: " << endl  
			<< usey4->username << "\t"  << usey4->password << "\t"  << usey4->id << "\t" 
			<< usey4->full_name << "\t"  << usey4->age << "\t"  << usey4->email << "\t" 
			<< usey4->followees << "\t"  << usey4->followers << "\t"  << usey4->free_bit << "\t" 
			<< usey4->num_woots << "\t" << endl;
			delete usey4;
			usey4=NULL;
		}
		else { cerr << "The user: test_accy" << i << " could not be found" << endl; }
	}

	cerr << endl << endl << "The results of already_following()ing are below: " << endl;  	
	User* usey5=read_user("test_accy16");
	if(usey5 != NULL){
		for(int fle_num=0; fle_num<30; fle_num++){
			stringstream capture_ss;
			string returned;
			returned=already_following(usey5, fle_num);
			capture_ss.str(returned);
			if( !(capture_ss.str().empty()) ){ // something was in fact returned
				string file_name;
				int line_num, current_fle;
				capture_ss >> file_name >> line_num >> current_fle;
				cerr << "The followee of id # " << fle_num << " is in the segment file: " << file_name
				<< " at the line #: " << line_num << " (representing home user " << usey5->username << ") and at follower id position # " << current_fle << endl << endl;
			}
			else { cerr << "The user test_accy16 is not following user id # " << fle_num << endl << endl; }
		}
		delete usey5;
		usey5=NULL;
	}
	
	cerr << endl << endl << "The results of already_followed_by()ing are below: " << endl;  	
	User* usey6=read_user("test_accy13");
	if(usey6 != NULL){
		for(int fle_num=0; fle_num<30; fle_num++){
			stringstream capture_ss;
			string returned;
			returned=already_followed_by(usey6, fle_num);
			capture_ss.str(returned);
			if( !(capture_ss.str().empty()) ){ // something was in fact returned
				string file_name;
				int line_num, current_fle;
				capture_ss >> file_name >> line_num >> current_fle;
				cerr << "The follower of id # " << fle_num << " is in the segment file: " << file_name
				<< " at the line #: " << line_num << " (representing home user " << usey6->username << ") and at follower id position # " << current_fle << endl << endl;
			}
			else { cerr << "The user test_accy13 is not being followed by user id # " << fle_num << endl << endl; }
		}
		delete usey6;
		usey6=NULL;
	}
	
	cerr << endl << endl << "The results of follow()ing are below: " << endl;  
	for(int i=10; i<50; i++){
		User* usey8=read_id(i%40);
		for(int j=i+20; j<i+60; j++){
			User* usey9;
			if((i%40)==(j%40)){ usey9=read_id((j+1)%40); }
			else { usey9=read_id((j)%40); }
			if(usey8!=NULL && usey9!=NULL) {  follow(usey8, usey9); }
			delete usey9;
			usey9=NULL;
		}
		delete usey8;
		usey8=NULL;
	}

	// makes more sense to follow and then see first_free
	cerr << endl << endl << "The results of first_free()ing are below: " << endl;  
	for(int i=0; i<30; i++){
		User* usey7=read_id(i);
		stringstream capture_ss;
		string check=first_free(usey7, 'e');
		capture_ss.str(first_free(usey7, 'e'));
		string file_name=capture_ss.str();
		int line_num=-2, current_fle=-2, current_column=-2;
		capture_ss >> file_name >> line_num >> current_fle >> current_column;
		if(file_name!="-1"){		
			cerr << "FOLLOWEES:\t" << usey7->username << "\t" << file_name << "\tline#: " << line_num << "\tfle_pos: " << current_fle
			<< "\tclmn pos: " << current_column << endl << endl;
		}
		else{
			cerr << usey7->username << " must create a new file as they have : " << usey7->followees << " spots taken up "
			<< endl << "This is in fact a border value: " << usey7->followees << " % " << FLRS_PER_LINE << " = "
			<< usey7->followees%FLRS_PER_LINE << endl << endl;
		}
		
		stringstream capture_ss2;
		capture_ss2.str(first_free(usey7, 'r'));
		string debug(capture_ss.str());
		string file_name2;
		line_num=-2; current_fle=-2; current_column=-2;
		capture_ss2 >> file_name2 >> line_num >> current_fle >> current_column;
		if(file_name2!="-1"){		
			cerr << "FOLLOWERS:\t" << usey7->username << "\t" << file_name2 << "\tline#: " << line_num << "\tfle_pos: " << current_fle
			<< "\tclmn pos: " << current_column << endl << endl;
		}
		else { 
			cerr << usey7->username << " must create a new file as they have : " << usey7->followers << " spots taken up "
			<< endl << "This is in fact a border value: " << usey7->followers << " % " << FLRS_PER_LINE << " = "
			<< usey7->followers%FLRS_PER_LINE << endl << endl;
		}

		cerr << endl << endl;
		delete usey7;
		usey7=NULL;
	}
	
	cerr << endl << endl << "The results of unfollow()ing are below: " << endl;  
	for(int i=0; i<40; i++){
		User* usey10=read_id(i%40);
		for(int j=i+1; j<i+41; j+=5){
			User* usey11;
			if((i%40)==(j%40)){ usey11=read_id((j+1)%40); }
			else { usey11=read_id((j)%40); }
			if(usey10!=NULL && usey11!=NULL) { 
				cerr << "User: " << usey10->username << " is unfollowing user: " << usey11->username << endl;
				unfollow(usey10, usey11);
			}
			delete usey11;
			usey11=NULL;
		}
		delete usey10;
		usey10=NULL;
	}
	
	cerr << endl << endl << "The results of first_free()ing after unfollow()ing are below: " << endl;  
	for(int i=0; i<30; i++){
		User* usey7_1=read_id(i);
		stringstream capture_ss;
		string check=first_free(usey7_1, 'e');
		capture_ss.str(first_free(usey7_1, 'e'));
		string file_name=capture_ss.str();
		int line_num=-2, current_fle=-2, current_column=-2;
		capture_ss >> file_name >> line_num >> current_fle >> current_column;
		if(file_name!="-1"){		
			cerr << "FOLLOWEES:\t" << usey7_1->username << "\t" << file_name << "\tline#: " << line_num << "\tfle_pos: " << current_fle
			<< "\tclmn pos: " << current_column << endl << endl;
		}
		else{
			cerr << usey7_1->username << " must create a new file as they have : " << usey7_1->followees << " spots taken up "
			<< endl << "This is in fact a border value: " << usey7_1->followees << " % " << FLRS_PER_LINE << " = "
			<< usey7_1->followees%FLRS_PER_LINE << endl << endl;
		}
	}

	cerr << endl << endl << "The results of follow()ing some after unfollow()ing are below: " << endl;  
	for(int i=0; i<40; i++){
		User* usey7_3=read_id(i%40);
		for(int j=i+1; j<i+41; j+=10){
			User* usey7_4;
			if((i%40)==(j%40)){ usey7_4=read_id((j+1)%40); }
			else { usey7_4=read_id((j)%40); }
			if(usey7_3!=NULL && usey7_4!=NULL) { 
				cerr << "User: " << usey7_3->username << " is following user: " << usey7_4->username << endl;
				follow(usey7_3, usey7_4);
			}
			delete usey7_4;
			usey7_4=NULL;
		}
		delete usey7_3;
		usey7_3=NULL;
	}
	
	
	cerr << endl << endl << "The results of first_free()ing after unfollow()ing and refollowing are below: " << endl;  
	for(int i=0; i<30; i++){
		User* usey7_5=read_id(i);
		stringstream capture_ss;
		string check=first_free(usey7_5, 'e');
		capture_ss.str(first_free(usey7_5, 'e'));
		string file_name=capture_ss.str();
		int line_num=-2, current_fle=-2, current_column=-2;
		capture_ss >> file_name >> line_num >> current_fle >> current_column;
		if(file_name!="-1"){		
			cerr << "FOLLOWEES:\t" << usey7_5->username << "\t" << file_name << "\tline#: " << line_num << "\tfle_pos: " << current_fle
			<< "\tclmn pos: " << current_column << endl << endl;
		}
		else{
			cerr << usey7_5->username << " must create a new file as they have : " << usey7_5->followees << " spots taken up "
			<< endl << "This is in fact a border value: " << usey7_5->followees << " % " << FLRS_PER_LINE << " = "
			<< usey7_5->followees%FLRS_PER_LINE << endl << endl;
		}
	}

	cerr << endl << endl << "The results of load_following()ing are below: " << endl;  
	for(int i=0; i<40; i++){
		User* usey12=read_id(i%40);
		cerr << "Selected user: " << usey12->username << " of id: " << usey12->id << " and type followees: "<< endl;
		vector<int> following=load_following(usey12, "followees", usey12->followees);
		for(size_t i=0; i< following.size(); i++) { cerr << following[i] << "\t"; }
			following.clear();

		cerr << endl << "Now showing followers: " << endl;
		following=load_following(usey12, "followers", usey12->followers);
		for(size_t i=0; i< following.size(); i++) { cerr << following[i] << "\t"; }
			following.clear();
		cerr << endl << endl;
		delete usey12;
		usey12=NULL;
	}
	
	
	cerr << endl << endl << "Testing invalid arguments to load_following(): " << endl;
	User* usey13=read_id(rand()%40);
	cerr << "Selected user: " << usey13->username << " of id: " << usey13->id << " and type followees: followees-5" << endl;
	vector<int> following=load_following(usey13, "followees", (usey13->followees)-5);
	for(size_t i=0; i< following.size(); i++) { cerr << following[i] << "\t"; }
		following.clear();

	cerr << endl << "Now showing followers: followers+5 " << endl;
	following=load_following(usey13, "followers", usey13->followers+5);
	for(size_t i=0; i< following.size(); i++) { cerr << following[i] << "\t"; }
		following.clear();

	cerr << endl << "Now showing followees: -5" << endl;
	following=load_following(usey13, "followees", -5);
	for(size_t i=0; i< following.size(); i++) { cerr << following[i] << "\t"; }
		following.clear();	

	cerr << endl << "Now showing followers: -5" << endl;
	following=load_following(usey13, "followers", -5);
	for(size_t i=0; i< following.size(); i++) { cerr << following[i] << "\t"; }
		following.clear();

	delete usey13;
	usey13=NULL;
	
	if(cond=='y'){
		cerr << endl << endl << "The results of write_woot()ing are below: " << endl;  
		for(int i=0; i<40; i++){
			User* usey14=read_id(i%40);
			for( int j=0; j<40; j++) {
				stringstream woot_ss;
				woot_ss << "This is a test woot #: " << j << " of user id: " << usey14->id << " plus the random value " << rand()/100000;
				stringstream time_stamp_ss;
				time_stamp_ss << "ts: " << j << " a #### " << rand()/100000;
				write_woot(usey14, woot_ss.str(), time_stamp_ss.str());
			}
			delete usey14;
			usey14=NULL;
		}
	}
	cerr << endl << endl << "The results of load_woot_seq()ing are below: " << endl;
	for( int i=0; i<40; i++) {
		User* usey15=read_id(i);
		vector<string>* woots=load_woots_seq(usey15, usey15->num_woots);
		cerr << "User #: " << usey15->id << endl;
		for(size_t j=0; j< woots->size(); j++){ cerr << "Woot #: " << j << endl << (*woots)[j] << endl; }
			cerr << endl << endl << endl;
		delete woots;
		woots=NULL;
		
		delete usey15;
		usey15=NULL;
	}

	cerr << endl << endl << "The results of load_woot()ing are below: " << endl;
	for( int i=0; i<40; i++) {
		User* usey16=read_id(i);
		vector<string>* woots=load_woots(usey16, usey16->num_woots);
		cerr << "User #: " << usey16->id << endl;
		for(size_t j=0; j < woots->size(); j++){ cerr << "Woot #: " << j << endl << (*woots)[j] << endl; }
			cerr << endl << endl << endl;
		delete woots;
		woots=NULL;
		
		delete usey16;
		usey16=NULL;
	}
	
	
	cerr << endl << endl << "Testing invalid arguments to load_woots(): " << endl;
	User* usey17=read_id(rand()%40);
	cerr << "Selected user: " << usey17->username << " of id: " << usey17->id << " and type followees: woots-5" << endl;
	vector<string>* woots=load_woots(usey17, usey17->num_woots-5);
	for(size_t j=0; j < woots->size(); j++){ cerr << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cerr << endl << endl << endl;
	woots->clear();
	
	cerr << endl << "Now showing woots: woots+5 " << endl;
	woots=load_woots(usey17, usey17->num_woots+5);
	for(size_t j=0; j < woots->size(); j++){ cerr << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cerr << endl << endl << endl;
	woots->clear();
	

	cerr << endl << "Now showing woots: -5" << endl;
	woots=load_woots(usey17, -5);
	for(size_t j=0; j < woots->size(); j++){ cerr << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cerr << endl << endl << endl;
	woots->clear();


	delete usey17;
	usey17=NULL;
}

void test_follow(char type){
	vector<thread> join_back2;
	int num_flw=45;
	if(type=='u') { num_flw=10; }
	for(int i=0; i<45; i++){
		User* usey8=read_id(i%40); // the read_id() calls result in memory leaks.
		for(int j=i+0; j<i+num_flw; j++){
			User* usey9; // this tester code was quick and dirty and will be improved for the future
			if((i%40)==(j%40)){ usey9=read_id((j+1)%40); }
			else { usey9=read_id((j)%40); }
			if(usey8!=NULL && usey9!=NULL) {  
				
				if(type=='f'){
					join_back2.push_back(thread([usey8, usey9] { follow(usey8, usey9); } ));
				}
				else if(type=='u'){
					join_back2.push_back(thread([usey8, usey9] { unfollow(usey8, usey9); } ));
				}
			}
		}
	}
	for(size_t i=0; i<join_back2.size(); i++){
		join_back2[i].join();
		cout << "flw_joined #" << i << endl;
	}
}

void test_woot(){
	vector<thread> join_back;
	for(int i=0; i<40; i++){
		User* usey14=read_id(i%40); // memory leaks here as well. this tester code will be improved in the future
		for( int j=0; j<40; j++) {
			stringstream woot_ss;
			woot_ss << "This is a test woot #: " << j << " of user id: " << usey14->id << " plus the random value " << rand()/100000;
			stringstream time_stamp_ss;
			time_stamp_ss << "ts: " << j << " a #### " << rand()/100000;
			// by value
			join_back.push_back(thread( write_woot, usey14, woot_ss.str(), time_stamp_ss.str() ) );
		}
	}
	for(size_t i=0; i<join_back.size(); i++){
		join_back[i].join();
		cout << "woot_joined #" << i << endl;
	}
}

void multi_threaded_tester(char cond='n'){	
	vector<User*> clean_up;
	vector<thread> join_back;
	User* usey = nullptr;
	
	// create users for first time
	if(cond=='y'){
		for(int i=0; i <40; i++){ 				
			usey=new User;
			usey->username="test_accy";
			usey->password="test_passwdy";
			usey->full_name="Test's_First_name Test's_Last_name";
			usey->age=311;
			usey->email="testemaily@emails.com";
			string original_name=usey->username;
			clean_up.push_back(usey);
			
			stringstream fullest_name_ss;
			fullest_name_ss << original_name << i;
			fullest_name_ss.clear();
			usey->username=fullest_name_ss.str();
			fullest_name_ss.str("");

			User* cpy=usey;
			join_back.push_back(thread([cpy] { create_new_user(cpy); } ) );
		}
	}	
	for(size_t i=0; i<join_back.size(); i++){ // join these threads so following commands have users to act on
		join_back[i].join();
		cout << "create__joined #" << i << endl;
	}
	for(size_t i=0; i<clean_up.size(); i++){ // clean up heap memory
		delete clean_up[i];
		clean_up[i]=nullptr;
	}

	test_follow('f');
	test_woot();	// main thread can't leave until follow, woots are done
	
	vector<thread> join_back2;
	for(int i=0; i <40; i++){ 				
		string original_name="test_accy";		
		stringstream fullest_name_ss;
		fullest_name_ss << original_name << i;
		fullest_name_ss.clear();
		original_name=fullest_name_ss.str();
		fullest_name_ss.str("");

		// heap memory being lost here excessively; tester code will be improved in the future
		join_back2.push_back(thread([i] { cout << "Read_cult: " << i << "\t" << cult_explode( load_following(read_id(i),"followees", 20) ) << endl; }) );	
		join_back2.push_back(thread( [i] { cout << "Alr_flw: " << i << "\t" << already_following(read_id(i), (i+1)%21, 'e') << endl; } ) );
		join_back2.push_back(thread( [i] { vector<string>* woots=load_woots(read_id(i), 20); for(size_t j=0; j<woots->size(); j++) { cout << "read_woot: user:" << i << j << "\t " << (*woots)[j] << endl; } } ) );
	}
	for(size_t i=0; i<join_back2.size(); i++){
		join_back2[i].join();
		cout << "end_joined #" << i << endl;
	}
	this_thread::sleep_for(chrono::seconds(5));
	cout << endl << " ending mt_tester " << endl;
}
