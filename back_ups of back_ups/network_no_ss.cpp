/*
  From Stevens Unix Network Programming, vol 1.
  Minor modifications by John Sterling
 */
#include <string>
#include <fstream>
#include <iostream>
#include <errno.h> // for checking  file existence
#include <sstream> // for concatenting ints and strings
#include <sys/stat.h> // for mkdir
#include <vector> // for vector<strings> for dirs to make in create_file

#include <stdio.h>       // perror, snprintf
#include <stdlib.h>      // exit
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <strings.h>     // bzero
#include <time.h>        // time, ctime
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons

#define	MAXLINE		4096	// max text line length
#define	BUFFSIZE	8192    // buffer size for reads and writes
#define  SA struct sockaddr
#define	LISTENQ		1024	// 2nd argument to listen()
#define PORT_NUM        13005 // how do I change this

using namespace std;

  struct Woot{ 
	  public:
    char message[100];
    char timestamp[22];
};

struct User{
	public:
    string username;
	string password;
    unsigned int id;
    string full_name;
    unsigned int age;
    string email;
    unsigned int followers;
    unsigned int followees;
    char free_bit;
    unsigned int num_woots;
};

//make all these values CONST?
    int MIN_USER_LEN=8;    int MAX_USER_LEN=24+1; 
    int MIN_PWD_LEN=10;    int MAX_PWD_LEN=32+1; 
    int MAX_ID_LEN=10+1; // at most 10 digits for the # of users
    int MAX_FNM_LEN=25;    int MAX_LNM_LEN=25+1;  int MAX_EML_LEN=100+1;
    int MAX_NUM_WOOTS=10; // at most 10 digits for the # of woots per user
    // age is hardcoded to 4, 3 digits + 1 pad
    int MAX_FLR_LEN=10+1;  int MAX_FLE_LEN=10+1; // at most 10 digits for the # of followees
    int MAX_ULINE_LEN=(24)+1+(32)+1+(10)+1+(25)+1+(25)+1+(4)+1+(100)+1+(10)+1+(10)+1+(10)+1;
    //int MAX_ULINE_LEN=MAX_USER_LEN+MAX_PWD_LEN+MAX_ID_LEN+MAX_FNM_LEN+1+MAX_LNM_LEN+4+MAX_EML_LEN+MAX_FLR_LEN+MAX_FLE_LEN+1+MAX_NUM_WOOTS+1;
    int USERS_PER_FILE=10;     int FID_HDR_LEN=5;     int FLRS_PER_LINE=10;
    int MAX_FLR_LINE=(10*10)+1;
    //int MAX_FLR_LINE_LEN=(MAX_ID_LEN*FLRS_PER_LINE)+1;
    char MY_DELIMITER=' ';
    int WOOTS_PER_LINE=10;     int MAX_WOOT_LEN=100;      int MAX_WOOT_TMSTP=22;
    int MAX_WOOT_LINE=(10)*(100+1+22+1);
    int MAX_WOOT_LINE_LEN=WOOTS_PER_LINE*(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
// is it oaky that all my file paths are hardcoded to /var/www/html? what if he doesnt run it in that
// I guess the directory where this code exists is the one where it makes things
// thats platform specific and tricky
// can I just document this and let that be?

// register.php neesd to not allow users to use spaces in their first name or last name
// use a dash or whatever you want, but if a space, an error
// otherwise storing users gets too tricky, have to do actually freads() instaed of <<
// -I- could change every ' ' to _ or soemthing and then on retrieval take that out
// but I'm too lazy, maybe at the very end


// get rid of prototypes and move functions here? or cleaner to keep down there
void function_tester();
void create_file(string file_type, string file_row, string file_column);
User* create_new_user(User* user_obj);
User* unflush_user(string file_name, int offset, bool unsure=false);
User* read_user(string username);
User* read_id(int id_num);
string already_following(User* home_user, int followee_id, char type='e');
string already_followed_by(User* other_user, int follower_id);
string first_free(User* user_obj, char type);

int main(int argc, char **argv) {
	// once set up functions and verify they all work right
	// then just have to change prototypes to fit what we can do over the network
	
	// but that doesnt change the function logic at all
	// now you just do a step at the start of the function
	// that converts the new parameter to the kind of parameter you need
	// and have the converted parameter have the name youw ere using before in the old
	// prototype, adn now its all good
	
	// SWAP OUT PARAMETER INTERFACE AND NO NEED TO CHANGE FUNCTIONS
	
	
	// check if num arguments is right
	// g++ -Wall -c -g "network.cpp"; g++ -o networks network.o; ./networks
	// create_file(argv[1], string(argv[2]), string(argv[3]));
	
	
	// keep php test_user and following functions
	// but have them call this over the network
	// thats the perfect way to verify
	
	
	function_tester();
	
	
	 //~ int	listenfd, connfd;  // Unix file descriptors. its just an int
    //~ struct sockaddr_in	servaddr;  // Note C use of struct
//~ 
//~ 
   //~ // need to decide format of what we're going to send
 //~ // minimal fixed # of bytes for command, then for operands from there, and a max total buffer size
//~ //    char buff[MAXLINE];
    //~ char cmd[6];
//~ 
    //~ // 1. Create the socket
    //~ if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        //~ perror("Unable to create a socket");
        //~ exit(1);
    //~ }
//~ 
    //~ // 2. Set up the sockaddr_in
//~ 
    //~ // zero it.  
    //~ // bzero(&servaddr, sizeof(servaddr)); // Note bzero is "deprecated".  Sigh.
    //~ memset(&servaddr, 0, sizeof(servaddr));
    //~ servaddr.sin_family      = AF_INET; // Specify the family
    //~ // use any network card present
    //~ servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //~ servaddr.sin_port        = htons(PORT_NUM);	// daytime server
//~ 
    //~ // 3. "Bind" that address object to our listening file descriptor
    //~ if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) == -1) {
        //~ perror("Unable to bind port");
        //~ exit(2);
    //~ }
//~ 
    //~ // 4. Tell the system that we are going to use this sockect for
    //~ //    listening and request a queue length
//~ 
    //~ // WHAT IS THIS QUEUE LENGTH
    //~ if (listen(listenfd, LISTENQ) == -1) {
        //~ perror("Unable to listen");
        //~ exit(3);
    //~ }
    //~ 
//~ 
    //~ for ( ; ; ) {
        //~ // 5. Block until someone connects.
        //~ //    We could provide a sockaddr if we wanted to know details of whom
        //~ //    we are talking to.
        //~ //    Last arg is where to put the size of the sockaddr if
        //~ //    we asked for one
     //~ fprintf(stderr, "Ready to connect.\n");
     //~ if ((connfd = accept(listenfd, (SA *) NULL, NULL)) == -1) {
        //~ perror("accept failed");
        //~ exit(4);
    //~ }
    //~ fprintf(stderr, "Connected\n");
    //~ int read_well=read(connfd, cmd, sizeof(cmd));
    //~ if(read_well==sizeof(cmd)){
//~ 
        //~ if(strncmp(cmd,"follow", sizeof(cmd))==0){ fprintf(stdout,"follow yes<\r\n\r\n");}
        //~ else { fprintf(stdout,":((\r\n\r\n"); }
 //~ //       snprintf(cmd, sizeof(cmd), "%.6s\r\n", "msgG");
            //~ write(connfd, cmd, strlen(cmd));
        //~ }
        //~ else{
        //~ // printf(sizeof(buff));
        //~ }
//~ 
//~ 
        //~ // We had a connection.  Do whatever our task is.
//~ //        ticks = time(NULL);
  //~ //      snprintf(buff, sizeof(buff), "%.24s\r\n", "testomg");
    //~ //    int len = strlen(buff);
//~ 
      //~ //  if (len != write(connfd, buff, strlen(buff))) {
        //~ //    perror("write to connection failed");
        //~ //}
//~ 
        //~ // 6. Close the connection with the current client and go back
        //~ //    for another.
        //~ close(connfd);
    //~ }
}

void function_tester(){
	User* usey = new User;
	usey->username="test_accy";
	usey->password="test_passwdy";
	usey->full_name="Test's_First_name Test's_Last_name";
	usey->age=25;
	usey->email="testemaily@emails.com";
	string original_name=usey->username;
	for(int i=0; i <40; i++){ 
		stringstream fullest_name;
		fullest_name << original_name << i;
		usey->username=fullest_name.str();
		fullest_name.str("");
		create_new_user(usey); 
	}
	delete usey;
	usey=NULL;
	
	User* usey2=unflush_user("/var/www/html/users/u_1.txt", 4*MAX_ULINE_LEN);
	// unflush does not error check, assumes youve already found the user and are just pulling out
	
	cout << endl << endl << "The results of unflushing u_1.txt at line # 4 are below: " << endl  
		<< usey2->username << "\t"  << usey2->password << "\t"  << usey2->id << "\t" 
		<< usey2->full_name << "\t"  << usey2->age << "\t"  << usey2->email << "\t" 
		<< usey2->followees << "\t"  << usey2->followers << "\t"  << usey2->free_bit << "\t" 
		<< usey2->num_woots << "\t" << endl;
	delete usey2;
	usey2=NULL;

	cout << endl << endl << "The results of read_user()ing are below: " << endl  	;
	User* usey3=read_user("test_accy12"); // always check for nulll when read_user()ing
	if(usey3 != NULL){
		cout << "The results of reading user test_accy10 are below: " << endl  
			<< usey3->username << "\t"  << usey3->password << "\t"  << usey3->id << "\t" 
			<< usey3->full_name << "\t"  << usey3->age << "\t"  << usey3->email << "\t" 
			<< usey3->followees << "\t"  << usey3->followers << "\t"  << usey3->free_bit << "\t" 
			<< usey3->num_woots << "\t" << endl;
		delete usey3;
		usey3=NULL;
	}
	else {
		cerr << "The user: test_acc10 could not be found" << endl;
	}
	
	cout << endl << endl << "The results of read_id()ing are below: " << endl;	
	for(int i=0; i < 50; i++){
		User* usey4=read_id(i); // always check for null when read_user()ing
		if(usey4 != NULL){
			cout << "The results of reading id # " << i << " are below: " << endl  
				<< usey4->username << "\t"  << usey4->password << "\t"  << usey4->id << "\t" 
				<< usey4->full_name << "\t"  << usey4->age << "\t"  << usey4->email << "\t" 
				<< usey4->followees << "\t"  << usey4->followers << "\t"  << usey4->free_bit << "\t" 
				<< usey4->num_woots << "\t" << endl;
			delete usey4;
			usey4=NULL;
		}
		else {
			cerr << "The user: test_accy" << i << " could not be found" << endl;
		}
	}
	// DELETE YOUR USER OBJECTS APPROPRIATELY OR MEMROY LEAKS AND FAIL
	// create destructors for User*? that needs pointer wrapper, hmm
	
	cout << endl << endl << "The results of already_following()ing are below: " << endl;  	
	User* usey5=read_user("test_accy16");
	if(usey5 != NULL){
		for(int fle_num=0; fle_num<30; fle_num++){
			stringstream capture;
			string returned;
			returned=already_following(usey5, fle_num);
			capture.str(returned);
			if( !(capture.str().empty()) ){ // something was in fact returned
				string file_name;
				int line_num, current_fle;
				capture >> file_name >> line_num >> current_fle;
				cout << "The followee of id # " << fle_num << " is in the segment file: " << file_name
					<< " at the line #: " << line_num << " (representing home user " << usey5->username
					<< ") and at follower id position # " << current_fle << endl << endl;
			}
			else { //
				cout << "The user test_accy16 is not following user id # " << fle_num << endl << endl;
			}
		}
		delete usey5;
		usey5=NULL;
	}
	
	cout << endl << endl << "The results of already_followed_by()ing are below: " << endl;  	
	User* usey6=read_user("test_accy13");
	if(usey6 != NULL){
		for(int fle_num=0; fle_num<30; fle_num++){
			stringstream capture;
			string returned;
			returned=already_followed_by(usey6, fle_num);
			capture.str(returned);
			if( !(capture.str().empty()) ){ // something was in fact returned
				string file_name;
				int line_num, current_fle;
				capture >> file_name >> line_num >> current_fle;
				cout << "The follower of id # " << fle_num << " is in the segment file: " << file_name
					<< " at the line #: " << line_num << " (representing home user " << usey6->username
					<< ") and at follower id position # " << current_fle << endl << endl;
			}
			else { //
				cout << "The user test_accy13 is not being followed by user id # " << fle_num << endl << endl;
			}
		}
		delete usey6;
		usey6=NULL;
	}
	
	cout << endl << endl << "The results of first_free()ing are below: " << endl;  
	for(int i=0; i<30; i++){
		User* usey7=read_id(i);
		stringstream capture;
		capture.str(first_free(usey7, 'e'));
		string file_name;
		int line_num=-1, current_fle=-1, current_column=-1;
		capture >> file_name >> line_num >> current_fle >> current_column;
		cout << "The first free followee spot for user: " << usey7->username << " is in file " 
			<< file_name << " at line #: " << line_num << " at followee spot #: " << current_fle
			<< " . This file is also confirmed as being column: " << current_column << endl << endl;
		capture.str("");
		capture.str(first_free(usey7, 'r'));
		file_name.clear();
		line_num=-1; current_fle=-1; current_column=-1;
		capture >> file_name >> line_num >> current_fle >> current_column;
		cout << "The first free follower spot for user: " << usey7->username << " is in file " 
			<< file_name << " at line #: " << line_num << " at follower spot #: " << current_fle
			<< " . This file is also confirmed as being column: " << current_column << endl << endl;				
		
	}
}

/*
 * If file exists, and file open or die code:
 * 
    int result=access(fids_path.c_str(), F_OK);
    if( (result < 0) && (errno == ENOENT) ) { // if file does not exist
		create_file("fids",0,0); 
	}
	else{
		ifstream fids;
		fids.open(fids_path.c_str());
		if(!fids){
			cout << "Error opening: " << fids_path << endl;
			exit(1);
		}
 
 * 
 * 
 * 
*/ 

// i COULD do all this as methods of the class
// but maybe at the end
// much easier interface then
// but really just flush, unflush, and the following functions
// lets leave flush adn unflush since they can bring nulls, and also unflush isnt a user method
// but following functions sure go for it

// peek actually moves the pointer
// MUST USE CLEAR() once hit end of file with seekg, cant evne move seekp until clear
// get rid of couts, those are just for me for debugging
    
 // first 4 
void flush_user(stringstream& file_name, User* user_obj, int offset){
	/*
	flush_user() writes the attributes of $user_obj one by one into the file $file_handle
	at the offset $offset. Each attribute is padded to meet its maximum field size
	so that each field, line, and user file are of a fixed, constant size.
	*/
	fstream user_file;
	user_file.open(file_name.str().c_str());
	if(!user_file){
		cout << "Error opening: " << file_name.str() << endl;
		exit(6);
	}
	user_file.seekp(offset, user_file.beg);
	stringstream buf;
	
	// probably could just overload operator<< so that it does this for me
	// outputs each of these to the given stream, can thus cout << user_obj
	// file << user_obj
	// file >> user_obj
	
	// sets a MINIMUM width to each field, be careful
	// set a fixed width stringstream to pad each field, then write to file
	buf.width(MAX_USER_LEN); 						buf << user_obj->username;		
	user_file << buf.str();							buf.str("");
	buf.width(MAX_PWD_LEN); 						buf << user_obj->password;		
	user_file << buf.str();							buf.str("");
	buf.width(MAX_ID_LEN); 							buf << user_obj->id;		
	user_file << buf.str();							buf.str("");
	buf.width(MAX_FNM_LEN+MAX_LNM_LEN+1); 			buf << user_obj->full_name;		
	user_file << buf.str();							buf.str("");
	buf.width(4); 									buf << user_obj->age;		
	user_file << buf.str();							buf.str("");
	buf.width(MAX_EML_LEN); 						buf << user_obj->email;		
	user_file << buf.str();							buf.str("");
	buf.width(MAX_FLR_LEN); 						buf << user_obj->followers;		
	user_file << buf.str();							buf.str("");
	buf.width(MAX_FLE_LEN); 						buf << user_obj->followees;		
	user_file << buf.str();							buf.str("");
	buf.width(1); 									buf << user_obj->free_bit;		
	user_file << buf.str();							buf.str("");
	buf.width(MAX_NUM_WOOTS); 						buf << user_obj->num_woots;		
	user_file << buf.str();							buf.str("");
	buf << endl; 
	user_file << buf.str();
	user_file.close();
}

void fids_decrease(int line_num){
	/*
	fids_decrease() takes a line number, which corresponds to a user file u_$line_num, and decreases
	the number of free spots/ids (fids) in that file. If $line_num is larger than the # of lines,
	a new line is written of $USERS_PER_FILE-1.
	*/
	string fids_path = "/var/www/html/fids/fids.txt";
	fstream fids;
	fids.open(fids_path.c_str());
	if(!fids){
		cout << "Error opening: " << fids_path << endl;
		exit(10);
	}
	
	// use line_num to loop via <<
	string current_free_ch;
	for(int i=0; i<line_num+1; i++){
		current_free_ch.clear();
		fids >> current_free_ch;	
	}
	
	// get amount to seek by, decrement current_free
	stringstream generic; // will be reused for ints then chars
	generic << USERS_PER_FILE-1;
	int num_digits=generic.str().length();
	int current_free=atoi(current_free_ch.c_str());
	if(current_free==0){ 
		current_free=USERS_PER_FILE-1; 
		fids.clear();
	}
	else{ current_free--; } 
	
	// seek where needed to write back current_free, do so
	fids.seekp(line_num*(num_digits+1), fids.beg);
	generic.str("");
	generic << current_free;
	fids << generic.str() << endl;
	fids.close();
}

// no string& since usually passing in literals
void create_file(string file_type, string file_row_s, string file_column_s){
	/*
	create_file() creates a file of the given $file_type and uses $file_row and $file_column
    to satisfy a naming convention. User files are one dimensional, from u_0.txt to u_n.txt.
    Followers, followees, and woot files use the row dimension to mirror the user_files,
    and have a column dimension to allow for a fixed # objects per file per user.
	*/
	int file_row=atoi(file_row_s.c_str());
	int file_column=atoi(file_column_s.c_str());
	stringstream file_name;
	string start_path="/var/www/html/";
	
	// check if any of the dirs needed dont exist, if so, make them
	vector<string> dirs;
	dirs.push_back("users");
	dirs.push_back("fids");
	dirs.push_back("flwrs");
	dirs.push_back("flwes");
	dirs.push_back("woots");
	for(size_t i=0; i < dirs.size(); i++){
		string test_path=dirs[i];	
		int result=access((start_path+test_path).c_str(), F_OK);
		if( (result < 0) && (errno == ENOENT) ) { // if directory does not exist
			cout << "The directory: " << start_path << test_path << " does not exist" << endl;
			mkdir((start_path+test_path).c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
			cout << "Have created dir! " << endl;
		}
		else{
			//cout << "The directory: " << start_path << test_path << " already exists" << endl;
		}
	}
	
	// choose file name format using the file_type parameter
	if(file_type=="users"){ file_name << "users/u_" << file_row << ".txt"; }
	else if(file_type=="fids") { file_name << "fids/fids.txt"; }
	else if(file_type=="followers") { file_name << "flwrs/fr_r" << file_row << "c" << file_column << ".txt"; }
	else if(file_type=="followees") { file_name << "flwes/fe_r" << file_row << "c" << file_column << ".txt"; }
	else if(file_type=="woots") { file_name << "woots/w_r" << file_row << "c" << file_column << ".txt"; }
	
	// if valid file type passed in, create the file with empty lines of that type
	int result=access((file_name.str()).c_str(), F_OK);
    if( (result == 0) ) { // if file does  exist
		cout << "Could not create: " << file_name.str() << " as it already exists." << endl;
		exit(8);
	}
	bool valid_type = (file_type=="users") || (file_type=="fids") || (file_type=="woots") || (file_type=="followers") || (file_type=="followees");
	if(valid_type){
		stringstream one_line;
		int num_lines=0;
		if(file_type=="users"){
			one_line << string(MAX_ULINE_LEN-1, MY_DELIMITER);
			num_lines=USERS_PER_FILE;
		}
		else if(file_type=="fids"){
			one_line << USERS_PER_FILE; // that many users slots are free when the file is made
			num_lines=1; // fids_decrease is called after making fids file/flushing user
		}
		else if(file_type=="followers" || file_type=="followees"){
			one_line << string(MAX_ID_LEN*FLRS_PER_LINE, MY_DELIMITER);
			num_lines=USERS_PER_FILE;
		}
		else if(file_type=="woots"){
			one_line << string(MAX_WOOT_LINE_LEN, MY_DELIMITER);
			num_lines=USERS_PER_FILE;			
		}
		one_line << endl;
		ofstream new_file;
		new_file.open(file_name.str().c_str());
		if(!new_file){
			cout << "Error opening: " << file_name.str() << endl;
			exit(9);
		}
		for(int lines=0; lines<num_lines; lines++){ new_file << one_line.str(); } // fill the file with empty line
		new_file.close();
	}
}

User* create_new_user(User* user_obj){    
    /*
    create_new_user() takes in a user object assembled from the registration page and fully
    integrates that user into the system. This is done by finding it a free user ID and by
    writing the user's attributes to the first available spot in the user file base.
    * The user object is then returned for convenience.
    */
    int current_line=0;
	string fids_path = "/var/www/html/fids/fids.txt";
    int result=access(fids_path.c_str(), F_OK);
    if( (result < 0) && (errno == ENOENT) ) { // if file does not exist
		create_file("fids","0","0"); 
	}
	else{ // if file does exist
		ifstream fids;
		fids.open(fids_path.c_str());
		if(!fids){
			cout << "Error opening: " << fids_path << endl;
			exit(5);
		}
		int num_free=0;
		fids >> num_free;
		while(num_free == 0){ // find the first user file # with free slots for new users
			if(!fids.eof()){ 
				current_line++;
				fids >> num_free;
			}
			else { break; }
		}
		fids.close();
	} // current line is now which user file you want

	
	// if the user file does not exist, make it, flush the user, update fids
	stringstream user_path;
	user_path << "users/u_" << current_line << ".txt";
	result=access(user_path.str().c_str(), F_OK);
    if( (result < 0) && (errno == ENOENT) ) { 
		int user_id=current_line*USERS_PER_FILE;
		stringstream current_line_str;
		current_line_str << current_line;
		create_file("users", current_line_str.str(), " ");		
		user_obj->id=user_id;
		user_obj->followers=0;
		user_obj->followees=0;
		user_obj->num_woots=0;
		user_obj->free_bit='n';
		flush_user(user_path, user_obj, 0); // 0 since this is a new file being made
		fids_decrease((user_obj->id/USERS_PER_FILE));
		return user_obj;
	}
	
	// if the file exists, it has free space for new users, since we checked the fids
	else { 
		fstream user_file;
		user_file.open(user_path.str().c_str());
		if(!user_file){
			cout << "Error opening: " << user_path.str() << endl;
			exit(7);
		}
		int current_id=0;
		char current_bit='\0';
		int bit_pos=MAX_ULINE_LEN-MAX_NUM_WOOTS-1-1;
		user_file.seekg(bit_pos, user_file.cur);
		
		 while(current_id<USERS_PER_FILE){ // scan until you find the first free spot
			current_bit='\0';
			user_file >> current_bit;
			//cout << "Read: " << current_bit << endl;
			if(current_bit=='\000'){  // fstream wont read whitespace
				user_obj->id=current_id+(USERS_PER_FILE*current_line);
				user_obj->followers=0;
				user_obj->followees=0;
				user_obj->num_woots=0;
				user_obj->free_bit='n';
				user_file.close();
				flush_user(user_path, user_obj, current_id*MAX_ULINE_LEN);								
				fids_decrease(user_obj->id / USERS_PER_FILE);
				return user_obj;
			}
			user_file.seekg(MAX_ULINE_LEN-1, user_file.cur);
			current_id+=1;
		}
	}
    return 0;
}

// next 4

// when calling unflush user, unlike in PHP now have to do
// fseek($user_base, -$MAX_USER_LEN, SEEK_CUR);
// before calculating offset to pass in

// unflush returns a heap user object, do NOT create User* = new User, and then call unflush
// THIS CREATES A USER ON THE HEAP, MAKE SURE TO HANDLE DELETING THIS WHEN DOEN
User* unflush_user(string file_name, int offset, bool unsure){	
	/*
	unflush_user() reads the attributes of $user_obj one by one from the file $file_handle
	at the given file position. Each attribute's padding is removed upon reading from file.

	It assumes a valid file_name and offset, does not error check for misuse.
	* The caller should use it properly.
	*/
	if(offset > MAX_ULINE_LEN*USERS_PER_FILE) { 
		cerr << "Error: tried to read line # " << offset/MAX_ULINE_LEN << " when only " << USERS_PER_FILE << " user lines per file " << endl;
		exit(10);// should this not crash? maybe just error, return null? big picture usage
	} // an entire CRASH is easy to debug, but horrible for the end user
	// or sicne this is a connection, can the exit get passed up somehow to php?
	// maybe should have an errorcode, pass that up  or set errno
	// and pass back a message that way

	ifstream user_base;
	user_base.open(file_name.c_str());
	if(!user_base && !unsure){
		cout << "Error opening: " << file_name << endl;
		exit(11);
	}
	else if(!user_base && unsure){ // if doing read_id and id might be arbitrary
		cout << "Error: the file " << file_name << " does not exist in the user base" << endl;
		return NULL; // don't crash the program because of invalid ID read attempt
	}
	user_base.seekg(offset, user_base.beg);
	stringstream buf;
	// when calling unflush user, unlike in PHP now have to do
	// fseek($user_base, -$MAX_USER_LEN, SEEK_CUR);
	// before calculating offset to pass in
	
	User *user_obj=new User;
	user_base >> user_obj->username >> user_obj->password >> user_obj->id;
	string first_name, last_name;
	user_base >> first_name >> last_name;
	user_obj->full_name=first_name + ' ' + last_name;
	user_base >> user_obj->age >> user_obj->email >> user_obj->followers 
	>> user_obj->followees >> user_obj->free_bit >> user_obj->num_woots;
	user_base.close();
	return user_obj;
}

// returns a nullptr if user couldnt be found
User* read_user(string username){
	/*
	read_user() takes in a username string, then finds it in the user file base and returns its user object.
	If the user does not exist in the user file base, a void return takes place.
	*/
	int file_number=0;
	while(file_number>-1){ // search every user file
		stringstream file_name;
		file_name << "/var/www/html" <<  "/users/u_" << file_number << ".txt";
		int result=access((file_name.str()).c_str(), F_OK);
		if( (result < 0) && (errno == ENOENT) ) { // if file does not exist
			cerr << "The file: " << file_name.str() << " does not exist" << endl;
			return NULL; // return nothing if not found
		}
		else { // file exists
			fstream user_base;
			user_base.open(file_name.str().c_str());
			if(!user_base){
				cerr << " The file: " << file_name.str() << " could not be opened." << endl;
				exit(12);
			}	
			while(user_base.tellg() != -1){
				string read_username;
				user_base >> read_username;
				cout << "Read the username: " << read_username << endl;
				if(read_username == username){ // if it matches what we want, build and return that user object
					user_base.seekg(-MAX_USER_LEN, user_base.cur);
					int offset=user_base.tellg();
					User* user_obj=unflush_user(file_name.str(), offset); // this could be null
					return user_obj;
				}				
				else { 
					user_base.seekg(MAX_ULINE_LEN-MAX_USER_LEN, user_base.cur); // if not a match, read next user
				}
			}
			user_base.close();
		}
		file_number++;
	}
	return NULL; // if inexplicably gets here
}

User* read_id(int id_num){ 
	/*
	read_id() takes in a username id #, then finds it in the user file base and returns its user object.
	If the user does not exist in the user file base, a void return takes place.
	This function allows for O(1) look up for followers/followees instead of an O(n) scan.
	*/
	stringstream file_name;
	file_name << "/var/www/html" << "/users/u_" << id_num/USERS_PER_FILE << ".txt";
	User* user_obj=unflush_user(file_name.str(), MAX_ULINE_LEN*(id_num % USERS_PER_FILE), true); // unsure if valid ID
	return user_obj; // could be null
}

// returns THREE things? how to implement hmm
string already_following(User* home_user, int followee_id, char type){
	/*
	already_following() takes in a logged in $home_user, the ID of the user they want to follow,
	and returns an array of the file name, file line #, and line position where that ID already
	exists in the user's list of followees. If the logged in user is not currently following that ID,
	then a void return takes place.
	*/
	int user_row=home_user->id/USERS_PER_FILE;
	int current_column=0;
	stringstream file_name;
	file_name << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
	unsigned int followees_read=0;
	int result=access(file_name.str().c_str(), F_OK);
	unsigned int total_people=type=='r'?home_user->followers:home_user->followees;
	while( !((result < 0)&&(errno==ENOENT)) && followees_read < total_people){
		ifstream following;
		following.open(file_name.str().c_str());
		int line_num=home_user->id % USERS_PER_FILE;
		following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		int flwrs_in_cur_file=FLRS_PER_LINE;
		string line;
		getline(following, line);
		stringstream sub_line(line);
		int last_token=-1;
		for(int current_fle=0; current_fle < flwrs_in_cur_file; current_fle++){ // flwrs follow is just a line of IDs separated by spaces
			int current_id;
			//cout << "in file: " << file_name.str() << endl;
			//line >> sub_line;
			sub_line >> current_id;
			if(current_id!=last_token){ // once hit last id in line, repeats the last token
				followees_read++;
				last_token=current_id;
			}
			else {
				//cout << "it works" << endl;
				 break; 
				 } // done with the line
			//cout << "Read the current id # : " << current_id << endl;
			if(current_id==followee_id) { 
				stringstream triplet;
				triplet << file_name.str() << ' ' << line_num << ' ' << current_fle;
				return triplet.str(); // yes by value, but what can you do
			}
			
			// have to read by a fixed amount because holes
			// so dont read 3 non holes, 7 of next guy's line and then move over
			// just do a getline
			
			// what if we create the following files so that a -1 at the end of each line
			//if(current_id=="") { cout << "That was an empty string" << endl; }
			// guess don't need this
			// need to see what happens when keep reading and base it on that
			// this is to handle removed followers leaving empty holes
			//if(current_id!="") { $followees_read++;  }
		}
		following.close();
		current_column++;
		file_name.str("");
		file_name << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt" ;
		result=access(file_name.str().c_str(), F_OK);
	}
	return ""; // should check on receiving end that not empty string
}
		
string already_followed_by(User* other_user, int follower_id){
	/*
	already_followed_by() takes in a target user $other_user, the ID of the logged in user wanting to follow them,
	and returns an array of the file name, file line #, and line position where that ID already
	exists in the target user's list of followers. If the target user is not currently being followed by the logged
	in user, then a void return takes place.
	*/
	return already_following(other_user, follower_id, 'r');
	
	//~ int user_row=other_user->id/USERS_PER_FILE;
	//~ int current_column=0;
	//~ stringstream file_name;
	//~ file_name << "/var/www/html" << "/flwrs/fr_r" << user_row << "c" << current_column << ".txt";
	//~ unsigned int followers_read=0;
	//~ int result=access(file_name.str().c_str(), F_OK);
	//~ while( !((result < 0)&&(errno==ENOENT)) && followers_read < other_user->followers){
		//~ ifstream following;
		//~ following.open(file_name.str().c_str());
		//~ int line_num=other_user->id % USERS_PER_FILE;
		//~ following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		//~ int flwrs_in_cur_file=FLRS_PER_LINE;
		//~ for(int current_flr=0; current_flr < flwrs_in_cur_file; current_flr++){ // flwrs follow is just a line of IDs separated by spaces
			//~ int current_id;
			//~ following >> current_id;
			//~ if(current_id==follower_id) { 
				//~ stringstream triplet;
				//~ triplet << file_name.str() << ' ' << line_num << ' ' << current_flr;
				//~ return triplet.str(); // yes by value, but what can you do
			//~ }
			//~ 
			//~ // need to see what happens when keep reading and base it on that
			//~ // this is to handle removed followers leaving empty holes
			//~ //if(current_id!="") { $followers_read++;  }
		//~ }
		//~ following.close();
		//~ current_column++;
		//~ file_name.str("");
		//~ file_name << "/var/www/html" << "/flwrs/fe_r" << user_row << "c" << current_column << ".txt" ;
		//~ result=access(file_name.str().c_str(), F_OK);
	//~ }
	//~ return ""; // should check on receiving end that not empty string
}

// since reusing holes so that it doesnt grow indefinitely in space
// with follow unfollow cycles
// have to do a seekg, since >> will skip over empty space
// and search linearly for first spot to find to insert
// seekg, getchar, and if ' ', back up full ID length, and insert there
string first_free(User* user_obj, char type){
	/*
	first_free() takes in a user and a char indicating either 'r' for "followers" or 'e' for "followees".
	It then returns the first location where a new follower/followee's ID can be recorded.
	This can indicate one of three cases: 1) a previous follower has unfollowed, leaving a gap,
	2) the user has no free gaps and a new file must be created, 3) or the file already
	exists but the user has simply not expanded into filling it.
	*/
	int user_row=user_obj->id/USERS_PER_FILE;
	int current_column=0;
	stringstream file_name;
	file_name << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
	int group=-1;
	if(type=='e'){ group=user_obj->followees; }
	else if(type=='r') { group=user_obj->followers; }
	else {
		cout << "An invalid type has been passed to first_free. Exiting" << endl;
		exit(13); // consider NOT crashing everything ever just over this, not that critical to logic
		// return a null or something, jesus
		// exit codes just for now for personal debugging
	}
	int followers_read=0;
	int line_num=user_obj->id%USERS_PER_FILE;
	
	int result=access(file_name.str().c_str(), F_OK);
	while( !((result < 0)&&(errno==ENOENT)) && followers_read < group){
		ifstream following;
		following.open(file_name.str().c_str());
		if(!following){
			cout << "Could not open file: " << file_name.str() << endl;
			exit(14);
		}

		following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		int flwes_in_cur_file=FLRS_PER_LINE;
		for(int current_fle=0; current_fle < flwes_in_cur_file; current_fle++){
			char * buffer = new char [MAX_ID_LEN];
			following.read(buffer,MAX_ID_LEN); // no need to check if read succeeded, limiting it
			stringstream current_id; // maybe do a getline and read that way?
			current_id.str(buffer);
			delete[] buffer; // no longer needed
			if(current_id.str().empty()){
				current_id.str("");
				current_id << file_name.str() << ' ' << line_num << ' ' << current_fle << ' ' << current_column;
				return current_id.str();				
			}
			else{ followers_read++; }
		}
		following.close();
		current_column++;
		file_name.str("");
		file_name << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
		result=access(file_name.str().c_str(), F_OK);
	}
	if( !((result < 0)&&(errno==ENOENT)) ) { // youve read the # of followers and found no spots
		stringstream current_id;
		current_id << file_name.str() << ' ' << line_num << ' ' << 0 << ' ' << current_column;
		return current_id.str(); // return the new file and spot to start filling
	}
	else{
		stringstream current_id;
		current_id << -1 << ' ' << line_num << ' ' << -1 << ' ' << current_column;
		return current_id.str();
		// return other information for outside functions ??
	}
}
	

void follow(User* home_user, User* other_user){}

void unfollow(User* home_user, User* other_user){}

vector<int> load_following(User* home_user, string following_type, int num){ 
	vector<int> v_i;
	return v_i; 
}

			
	// STORE " " instead of ., only first name and alst name ahs a space in between
	// now can use << to get things
    // woots cant be space delimited alone, since woots have spaces
	// can have every woot delimit with a \n, but then variable size woots and lines
	// have to getline through the whole thing
	// but you know what, thats okay
	// either do that if frustrated, or stick to padding . and trying to trim . somehow
	// may be easier just to have WOOT\nTIMESTAMP\n
	// but so ugly to have to read through ALL woots of file to ge thter
	// but then again youd have a database in real life and this is a small prject
	// so just do the damn woot\ntimestamp\n and call it an easier day
    
    // COMPROMSIE:
    // PAD each woot and timestamp to fit the right length, then \n
    // so when you write, you pad
    // when you read, you getline(), and possibly get the padding as well
    // but can figure that out
    // so can still have fixed seek() for quick finding/access, and << for easy file handling

 // when write woots to file, make sure a \0 or some sort of character sequence user cant generate is there
// maybe just do the \n after each woto		
void write_woot(User* home_user, string& woot, string& timestamp){}
void load_woots(User* home_user, int num_woots){}

// optional, left this in there because lazy
void load_woots_seq(User* home_user, int num_woots){}

