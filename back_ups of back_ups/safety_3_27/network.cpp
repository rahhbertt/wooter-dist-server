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


// get rid of prototypes and move functions here? or cleaner to keep down there
void create_file(string file_type, string file_row, string file_column);
User* create_new_user(User* user_obj);

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
	create_file(argv[1], string(argv[2]), string(argv[3]));
	User* usey = new User;
	usey->username="testaccy";
	usey->email="testemaily@emails.com";
	usey->password="testpasswdy";
	
	usey->age=25;
	usey->full_name="testfully";
	create_new_user(usey);
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
			one_line << USERS_PER_FILE-1; // that many users slots are free when the file is made
			num_lines=1;
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
void unflush_user(string file_name){}

User* read_user(string username){ return new User;}

User* read_id(int id_num){ return new User;}

// returns THREE things? how to implement hmm
void already_following(User* home_user, int followee_id){}
	
void already_followed_by(User* other_user, int followee_id){}

// returns THREE things? how to implement hmm
void first_free(User* user_obj, string follow_type){}
	
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

