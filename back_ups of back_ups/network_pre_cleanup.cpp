
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
#define PORT_NUM        13039 // change this as needed

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
// strip out all \n when taking in a woot via php
// easiest way out

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
    int MAX_FLR_LINE_LEN=(MAX_ID_LEN*FLRS_PER_LINE)+1;
    //int MAX_FLR_LINE_LEN=(MAX_ID_LEN*FLRS_PER_LINE)+1;
    char MY_DELIMITER=' ';
    int WOOTS_PER_LINE=10;     int MAX_WOOT_LEN=100;      int MAX_WOOT_TMSTP=22;
    int MAX_WOOT_LINE=(10)*(100+1+22+1);
    int MAX_WOOT_LINE_LEN=WOOTS_PER_LINE*(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
// is it oaky that all my file paths are hardcoded to /var/www/html? what if he doesnt run it in that
// I guess the directory where this code exists is the one where it makes things
// thats platform specific and tricky
// can I just document this and let that be?

//make a note MY_DELIMITER still hard coded, since use >> in some parts, a lot actually
// and that delimits on whitespace

// register.php neesd to not allow users to use spaces in their first name or last name
// use a dash or whatever you want, but if a space, an error
// otherwise storing users gets too tricky, have to do actually freads() instaed of <<
// -I- could change every ' ' to _ or soemthing and then on retrieval take that out
// but I'm too lazy, maybe at the very end


// get rid of prototypes and move functions here? or cleaner to keep down there
void net_connection(char** argv);
void function_tester(char cond='n');
void create_file(string file_type, string file_row, string file_column);
void flush_user(stringstream& file_name_ss, User* user_obj, int offset);
User* create_new_user(User* user_obj);
User* unflush_user(string file_name, int offset, bool unsure=false);
User* read_user(string username);
User* read_id(int id_num);
string already_following(User* home_user, int followee_id, char type='e');
string already_followed_by(User* other_user, int follower_id);
string first_free(User* user_obj, char type);
void follow(User* home_user, User* other_user);
void unfollow(User* home_user, User* other_user);
vector<int> load_following(User* home_user, string following_type, int num);
void write_woot(User* home_user, string woot, string timestamp);
vector<string>* load_woots_seq(User* home_user, int num_woots);
vector<string>* load_woots(User* home_user, int num_woots, char seq='n');

int main(int argc, char **argv) {	
	// check if num arguments is right
	// keep php test_user and following functions
	// but have them call this over the network
	// thats the perfect way to verify
	
	if(argv[2][0]=='y'){ function_tester(argv[2][0]);}
	
	// return vector of strings and send it back over the network
	net_connection(argv);
}
void reply(int connfd, string msg){
		stringstream reply_ss; // stringstream to send out multiple strings/ints/vars
		reply_ss /*<< "Sending the following:\t" */<< msg << endl; // NEED that endl
		cout << reply_ss.str() << endl; // local output
		string reply_s(reply_ss.str()); // make a string out of the stream
		char* reply_c=new char[reply_s.size()]; // allocate space for a char* out of the string
		reply_s.copy(reply_c, reply_s.size() ); // fill that char* with the string's contents
		write(connfd, reply_c, reply_s.size()); // write that char* with that size out to the network
		delete[] reply_c;
//~ 
		
	//~ stringstream reply_1_ss;
	//~ string received(msg);
	//~ cout << "Received the following:\t" << received << endl;
	//~ reply_1_ss << "Received the following:\t" << received << endl;
	//~ string reply_1(reply_1_ss.str());
	//~ char* receipt= new char[reply_1.size()]; // this makes sense
	//~ reply_1.copy(receipt, reply_1.size()); // is this bad somehow? new char but not length
	//~ write(connfd, receipt, reply_1.size() );
		//~ 
}

User* user_unexplode(stringstream& ss){
	string username, password, first_name, last_name, email;
    unsigned int age, id, followers, followees, num_woots;
    char free_bit;
    ss >> username >> password >> id;
    ss.clear();
    ss >> first_name >> last_name >> age;
    ss.clear();
    ss >> email >> followers;
    ss.clear();
    ss >> followees;
    ss.clear(); // after every read into int
    ss >> free_bit >> num_woots;
    //~ ss >> username >> password >> first_name >> last_name >> age >> email;
	User* newbie=new User;
	newbie->username=username;
	newbie->password=password;
	newbie->id=id;
	newbie->full_name=first_name +' '+ last_name;
	newbie->age=age;
	newbie->email=email;
	newbie->followers=followers;
	newbie->followees=followees;
	newbie->free_bit=free_bit;
	newbie->num_woots=num_woots;
	return newbie;
	// when registering, so must be set to dummy values, as they will be overwritten anyhow
	// id, followers, followees, free_bit, num_woots cannot be set
	
	//~ ss >> command >> username >> password >> id;
	//~ ss.clear();
	//~ ss >> first_name >> last_name >> age;
	//~ ss.clear();
	//~ ss >> email >> followers;
	//~ ss.clear()
	//~ ss >> followees;
	//~ ss.clear();
	//~ ss >> free_bit >> num_woots; 
	
    
	
	//~ string first_name, last_name;
	//~ user_base >> first_name >> last_name;
	//~ user_obj->full_name=first_name + ' ' + last_name;
	//~ 
	//~ 
	
	//return new User;
}

string user_explode(User* user_obj){
	stringstream buf_ss;
	buf_ss.width(MAX_USER_LEN); 						buf_ss << user_obj->username;		
	buf_ss.width(MAX_PWD_LEN); 							buf_ss << user_obj->password;		
	buf_ss.width(MAX_ID_LEN); 							buf_ss << user_obj->id;		
	buf_ss.width(MAX_FNM_LEN+MAX_LNM_LEN+1); 			buf_ss << user_obj->full_name;		
	buf_ss.width(4); 									buf_ss << user_obj->age;		
	buf_ss.width(MAX_EML_LEN); 							buf_ss << user_obj->email;		
	buf_ss.width(MAX_FLR_LEN); 							buf_ss << user_obj->followers;		
	buf_ss.width(MAX_FLE_LEN); 							buf_ss << user_obj->followees;		
	buf_ss.width(1); 									buf_ss << user_obj->free_bit;		
	buf_ss.width(MAX_NUM_WOOTS); 						buf_ss << user_obj->num_woots;		
	return buf_ss.str();
}

string cult_explode(vector<int> cult){
	stringstream buf_ss;
	for(size_t i=0; i<cult.size(); i++){
		buf_ss.width(MAX_ID_LEN);
		buf_ss << cult[i];
		buf_ss.clear();
//		buf_ss << " ";
	}
	return buf_ss.str();
}

string unexplode_woot(stringstream& ss){
	string big_line(ss.str());
	char* woot_c=new char[MAX_WOOT_LEN];
	big_line.copy(woot_c, MAX_WOOT_LEN, 40+MAX_ULINE_LEN); // hard coded command field size
	string to_return(woot_c);
	to_return.resize(MAX_WOOT_LEN);
	delete[] woot_c;
	woot_c=NULL;

	return to_return;
}

//~ string woots_explode(vector<string>* woots){
	//~ stringstream big_line;
	//~ for(size_t i=0; i<(*woots).size(); i++){
		//~ big_line << (*woots)[i]; // space delimiter already included between woots
	//~ }
	//~ return big_line.str();
//~ }

void reply_woots(int connfd, vector<string>* woots){
		stringstream reply_ss; // stringstream to send out multiple strings/ints/vars
		int num_packaged=0;
		int iterations=((*woots).size()/4)+((*woots).size()%4!=0?1:0);
		for(int i=0; i<iterations; i++){
			reply_ss.str("");
			for( size_t j=0; j<4 && num_packaged<(int)(*woots).size(); num_packaged++, j++){
				reply_ss << (*woots)[num_packaged]; // space delimiter already included between woots
				cout << "pulled out woot:\n\n\n" << (*woots)[num_packaged] << endl;
			}
			reply_ss << endl; // so php can know line has ended
			//~ reply_ss << string(560-4*(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1), ' '); /
			//reply_ss /*<< "Sending the following:\t" */<< large_line << endl; // NEED that endl
			cout << reply_ss.str() << endl; // local output
			string reply_s(reply_ss.str()); // make a string out of the stream
			char* reply_c=new char[reply_s.size()]; // allocate space for a char* out of the string
			reply_s.copy(reply_c, reply_s.size() ); // fill that char* with the string's contents
			write(connfd, reply_c, reply_s.size()); // write that char* with that size out to the network		
			delete[] reply_c;
		}
//~ 
}

string unexplode_ts(stringstream& ss){
	string big_line(ss.str());
	char* ts_c=new char[MAX_WOOT_TMSTP];
	big_line.copy(ts_c, MAX_WOOT_TMSTP, 40+MAX_ULINE_LEN+MAX_WOOT_LEN+1); // hard coded command field size
	string to_return(ts_c);
	to_return.resize(MAX_WOOT_TMSTP);
	delete[] ts_c;
	ts_c=NULL;

	return to_return;
}
			
void handle_php(int connfd, char* cmd, int cmd_size){
		string received(cmd);
		received.resize(cmd_size);
		cout << "Size of received cmd: " << received.size() << endl;
		//top 3 lines all optional, just for own debugging needs
		
		// make a stream out of the message, read in the comand token
		stringstream received_ss;
		received_ss.str(string(cmd));
		string received_str;
		received_ss >> received_str;
		
		// switch block based on passed in command
		if(received_str=="create_new_user"){
			User* newbie=user_unexplode(received_ss);
			if(newbie!=0){ 
				create_new_user(newbie);
				reply(connfd, user_explode(newbie) ); 
			}
			else{
				// send back ERROR: message
			}
			delete newbie;
			newbie=NULL;
			
			// at end have to replace every exit(code) command
			// with exit_net(code)
			// return;
			// that will send out the error code over the network, then return
			// a value that can be tested for at tha capture
			//~ string response("you got it, bub");
			//~ reply(connfd, response);
		}
		else if(received_str=="read_user"){
			string username;
			received_ss >> username;
			User * read_out=read_user(username);
			if(read_out!=NULL){ reply(connfd, user_explode(read_out)); }
			else{
				// send back ERROR: message
			}
			delete read_out;
			read_out=NULL;
		}
		else if(received_str=="flush_user"){
			User* flush=user_unexplode(received_ss);
			if(flush!=0){ 
				stringstream file_path_ss;
				file_path_ss << "/var/www/html" <<  "/users/u_" << flush->id/USERS_PER_FILE << ".txt";
				flush_user(file_path_ss, flush, (flush->id%USERS_PER_FILE)*(MAX_ULINE_LEN) );
				reply(connfd, user_explode(flush) ); // unnecessary network traffic, but why not
			}
			else{
				// send back ERROR: message
			}
			delete flush;
			flush=NULL;
			
		}
		else if(received_str=="read_id"){
			int id=-1;
			received_ss >> id;
			received_ss.clear();
			User* user_id=read_id(id);
			if(user_id!=NULL){ reply(connfd, user_explode(user_id)); }
			else{
				
			} // send back error message
		}
		else if(received_str=="follow"){
			User* home_user=user_unexplode(received_ss);
			User* other_user=user_unexplode(received_ss); // since by &, this should work
			if( home_user!=NULL && other_user != NULL){ follow(home_user, other_user); }
			else{ // no reply needed at all whatsoever
				//ERROR:
			}
			
		}
		else if(received_str=="unfollow"){
			User* home_user=user_unexplode(received_ss);
			User* other_user=user_unexplode(received_ss); // since by &, this should work
			if( home_user!=NULL && other_user != NULL){ unfollow(home_user, other_user); }
			else{ // no reply needed at all whatsoever
				//ERROR:
			}
			
		}
		else if(received_str=="load_following"){ // should note somewhere since 560 bytes
			// a real practical limit to load_following at one time
			User* home_user=user_unexplode(received_ss);
			string group_type;
			received_ss >> group_type;
			int num_ppl=-1;
			received_ss >> num_ppl;
			received_ss.clear();
			if(home_user!=NULL){ 
				vector<int> cult=load_following(home_user, group_type, num_ppl);
				reply(connfd, cult_explode(cult));
			}
			else{
				
			} // send back error message
			//real_load_following($home_user, "followees", 10, $argv[1]);
			// should check load following code to see how it handles a # greater than # of followers
		}
		else if(received_str=="write_woot"){
			User* home_user=user_unexplode(received_ss);
			string woot=unexplode_woot(received_ss);
			string timestamp=unexplode_ts(received_ss);
			if(home_user!=NULL){
				write_woot(home_user, woot, timestamp);
				reply(connfd, user_explode(home_user) );
				// have to increment num_woots
			} // no ACK
			else{
				//ERROR:
			}
			
		}
		else if(received_str=="load_woots"){
			User* wooter=user_unexplode(received_ss);
			int num_woots=-1;
			received_ss >> num_woots;
			received_ss.clear();
			if(wooter!=NULL){
				vector<string>* woots=load_woots(wooter, num_woots);
				reply_woots(connfd, woots);
				delete woots;
			}
			
		}
		else if(received_str=="load_woots_seq"){
			User* wooter=user_unexplode(received_ss);
			int num_woots=-1;
			received_ss >> num_woots;
			received_ss.clear();
			if(wooter!=NULL){
				vector<string>* woots=load_woots(wooter, num_woots);
				reply_woots(connfd, woots);
				delete woots;
			}
			
		}
		else if(received_str=="already_following"){
			User* home_user=user_unexplode(received_ss);
			int other_id=-1;
			received_ss >> other_id;
			received_ss.clear();
			if(home_user!=NULL){
				string triplet=already_following(home_user, other_id, 'e');
				if(!triplet.empty()){ reply(connfd, "YES"); }
				else { reply(connfd, "NO"); }
			}
		}
		//$request=format(format("already_following",40).user_explode($home_user)." ".$other_id, $SEND_SIZE );
		else { //invalid commands handled here
			reply(connfd, received);
		}
		// disable all these god awful cout statements, just comment them out
		// so can uncomment for the future
		// or make an exact copy with comments deleted, cleaner, just to submit?
		// for .zip for personal archive include the one with comments tho
		/*
	read_user()
	flush_user()
	now got register and login working
	
	follow()
	unfollow()
	load_following()
	read_id()
	write_woot()
	and that's everything public
	* */
}

void net_connection(char** argv){
	/*
	  From Stevens Unix Network Programming, vol 1.
	  Minor modifications by John Sterling
	 */
	 int	listenfd, connfd;  // Unix file descriptors. its just an int
    struct sockaddr_in	servaddr;  // Note C use of struct

// need to decide format of what we're going to send
// minimal fixed # of bytes for command, then for operands from there, and a max total buffer size
//    char buff[MAXLINE];
//~ 
    // 1. Create the socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Unable to create a socket");
        exit(1);
    }

    // 2. Set up the sockaddr_in

    // zero it.  
    // bzero(&servaddr, sizeof(servaddr)); // Note bzero is "deprecated".  Sigh.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET; // Specify the family
    // use any network card present
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    
    // just for testing, put PORT_NUM back as the arg
	stringstream port_ss;
	port_ss.str(argv[1]);
	int p_num=-1;
	port_ss >> p_num;
    servaddr.sin_port        = htons(p_num);	// daytime server

    // 3. "Bind" that address object to our listening file descriptor
    if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) == -1) {
        perror("Unable to bind port");
        exit(2);
    }

    // 4. Tell the system that we are going to use this sockect for
    //    listening and request a queue length

    // WHAT IS THIS QUEUE LENGTH
    if (listen(listenfd, LISTENQ) == -1) {
        perror("Unable to listen");
        exit(3);
    } // setting up how many bytes at a time listenfd can take maybe?
    

    for ( ; ; ) {
        // 5. Block until someone connects.
        //    We could provide a sockaddr if we wanted to know details of whom
        //    we are talking to.
        //    Last arg is where to put the size of the sockaddr if
        //    we asked for one
     fprintf(stderr, "Ready to connect.\n");
     if ((connfd = accept(listenfd, (SA *) NULL, NULL)) == -1) {
        perror("accept failed");
        exit(4);
    }
    fprintf(stderr, "Connected\n");
    string capture;
      //~ int read_well=read(connfd, &capture, 10);
      
      
    char cmd[560]; // make this bigger
    int read_well=read(connfd, cmd, sizeof(cmd));
    // read will block until it has exactly the number of bytes you wanted, or more
    // but the char * will only get that many bytes read out by future things
    
    // also third param, # bytes to read, need not be # bytes of array
    if(read_well==sizeof(cmd)){
		handle_php(connfd, cmd, 560);

	//~ 
		//~ stringstream reply_1_ss;
		//~ string received(cmd);
		//~ cout << "Received the following:\t" << received << endl;
		//~ reply_1_ss << "Received the following:\t" << received << endl;
		//~ string reply_1(reply_1_ss.str());
		//~ char* receipt= new char[reply_1.size()]; // this makes sense
		//~ reply_1.copy(receipt, reply_1.size()); // is this bad somehow? new char but not length
		//~ write(connfd, receipt, reply_1.size() );
		//~ 
		//~ 
		//~ stringstream output_ss;
		//~ output_ss << "Read the following: '" << string(cmd) << "'" << endl;
		//~ 
		
        //~ if(strncmp(cmd,"follow", sizeof(cmd))==0){ fprintf(stdout,"follow yes<\r\n\r\n");}
        //~ else { fprintf(stdout,":((\r\n\r\n"); }
 //       snprintf(cmd, sizeof(cmd), "%.6s\r\n", "msgG");
			//write(connfd, &capture, capture.size());
			
			//~ string cmd_s("longer string\r\n");
			//~ cout << "String form:" << cmd << endl;
			//~ 
			//~ char* test = new char[cmd_s.size()];
			//~ cmd_s.copy(test, cmd_s.size());
			//~ write(connfd, test, cmd_s.size() );
//~ //			write(connfd, test, sizeof(test));
			//~ delete[] test;
			//write(connfd, cmd, sizeof(cmd));
			//~ cout << " can I cout? " << endl;
			//write(connfd, output_ss.str().c_str(), output_ss.str().size() );
//			write(connfd, (char *)"testing", 6); 
            //write(connfd, output_ss.str().c_str(), output_ss.str().length());
        }
        //must have \r\n at the end of thing being written to network
        // as php is picking it up via getline, so one line at a time
        
        
        else{
        // printf(sizeof(buff));
        }


//~ 
	//~ // create function_net() interfaces to convert parameters to internal functions
	//~ // use .php test_user and test_follow functions to create things over the network
	//~ // perfect way to verify everythign
	//~ 
//~ 
//~ 
        // We had a connection.  Do whatever our task is.
//        ticks = time(NULL);
  //      snprintf(buff, sizeof(buff), "%.24s\r\n", "testomg");
    //    int len = strlen(buff);

      //  if (len != write(connfd, buff, strlen(buff))) {
        //    perror("write to connection failed");
        //}

        // 6. Close the connection with the current client and go back
        //    for another.
        close(connfd);
    }
    	//~ 
}

void function_tester(char cond){
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
		cout << "The results of reading user test_accy12 are below: " << endl  
			<< usey3->username << "\t"  << usey3->password << "\t"  << usey3->id << "\t" 
			<< usey3->full_name << "\t"  << usey3->age << "\t"  << usey3->email << "\t" 
			<< usey3->followees << "\t"  << usey3->followers << "\t"  << usey3->free_bit << "\t" 
			<< usey3->num_woots << "\t" << endl;
		delete usey3;
		usey3=NULL;
	}
	else {
		cerr << "The user: test_acc12 could not be found" << endl;
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
			stringstream capture_ss;
			string returned;
			returned=already_following(usey5, fle_num);
			capture_ss.str(returned);
			if( !(capture_ss.str().empty()) ){ // something was in fact returned
				string file_name;
				int line_num, current_fle;
				capture_ss >> file_name >> line_num >> current_fle;
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
			stringstream capture_ss;
			string returned;
			returned=already_followed_by(usey6, fle_num);
			capture_ss.str(returned);
			if( !(capture_ss.str().empty()) ){ // something was in fact returned
				string file_name;
				int line_num, current_fle;
				capture_ss >> file_name >> line_num >> current_fle;
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
	
	cout << endl << endl << "The results of follow()ing are below: " << endl;  
	for(int i=10; i<50; i++){
		User* usey8=read_id(i%40);
		for(int j=i+20; j<i+60; j++){
			User* usey9;
			if((i%40)==(j%40)){ usey9=read_id((j+1)%40); }
			else { usey9=read_id((j)%40); }
			if(usey8!=NULL && usey9!=NULL) { 
				follow(usey8, usey9);
			}
			delete usey9;
			usey9=NULL;
		}
		delete usey8;
		usey8=NULL;
	}

// makes more sense to follow and then see first_free

	cout << endl << endl << "The results of first_free()ing are below: " << endl;  
	for(int i=0; i<30; i++){
		User* usey7=read_id(i);
		stringstream capture_ss;
		string check=first_free(usey7, 'e');
		capture_ss.str(first_free(usey7, 'e'));
		string file_name=capture_ss.str();
		int line_num=-2, current_fle=-2, current_column=-2;
		capture_ss >> file_name >> line_num >> current_fle >> current_column;
		if(file_name!="-1"){		
			cout << "FOLLOWEES:\t" << usey7->username << "\t" << file_name << "\tline#: " << line_num << "\tfle_pos: " << current_fle
				 << "\tclmn pos: " << current_column << endl << endl;
		}
		else{
			cout << usey7->username << " must create a new file as they have : " << usey7->followees << " spots taken up "
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
			cout << "FOLLOWERS:\t" << usey7->username << "\t" << file_name2 << "\tline#: " << line_num << "\tfle_pos: " << current_fle
				 << "\tclmn pos: " << current_column << endl << endl;
		}
		else { 
			cout << usey7->username << " must create a new file as they have : " << usey7->followers << " spots taken up "
				<< endl << "This is in fact a border value: " << usey7->followers << " % " << FLRS_PER_LINE << " = "
				<< usey7->followers%FLRS_PER_LINE << endl << endl;
		}

		cout << endl << endl;
		delete usey7;
		usey7=NULL;
	}

	
		cout << endl << endl << "The results of unfollow()ing are below: " << endl;  
		for(int i=0; i<40; i++){
			User* usey10=read_id(i%40);
			for(int j=i+1; j<i+41; j+=5){
				User* usey11;
				if((i%40)==(j%40)){ usey11=read_id((j+1)%40); }
				else { usey11=read_id((j)%40); }
				if(usey10!=NULL && usey11!=NULL) { 
					cout << "User: " << usey10->username << " is unfollowing user: " << usey11->username << endl;
					unfollow(usey10, usey11);
				}
				delete usey11;
				usey11=NULL;
			}
			delete usey10;
			usey10=NULL;
		}
	
		cout << endl << endl << "The results of first_free()ing after unfollow()ing are below: " << endl;  
		for(int i=0; i<30; i++){
			User* usey7_1=read_id(i);
			stringstream capture_ss;
			string check=first_free(usey7_1, 'e');
			capture_ss.str(first_free(usey7_1, 'e'));
			string file_name=capture_ss.str();
			int line_num=-2, current_fle=-2, current_column=-2;
			capture_ss >> file_name >> line_num >> current_fle >> current_column;
			if(file_name!="-1"){		
				cout << "FOLLOWEES:\t" << usey7_1->username << "\t" << file_name << "\tline#: " << line_num << "\tfle_pos: " << current_fle
					 << "\tclmn pos: " << current_column << endl << endl;
			}
			else{
				cout << usey7_1->username << " must create a new file as they have : " << usey7_1->followees << " spots taken up "
					<< endl << "This is in fact a border value: " << usey7_1->followees << " % " << FLRS_PER_LINE << " = "
					<< usey7_1->followees%FLRS_PER_LINE << endl << endl;
			}
		}
		
		cout << endl << endl << "The results of follow()ing some after unfollow()ing are below: " << endl;  
		for(int i=0; i<40; i++){
			User* usey7_3=read_id(i%40);
			for(int j=i+1; j<i+41; j+=10){
				User* usey7_4;
				if((i%40)==(j%40)){ usey7_4=read_id((j+1)%40); }
				else { usey7_4=read_id((j)%40); }
				if(usey7_3!=NULL && usey7_4!=NULL) { 
					cout << "User: " << usey7_3->username << " is following user: " << usey7_4->username << endl;
					follow(usey7_3, usey7_4);
				}
				delete usey7_4;
				usey7_4=NULL;
			}
			delete usey7_3;
			usey7_3=NULL;
		}
	
	
		cout << endl << endl << "The results of first_free()ing after unfollow()ing and refollowing are below: " << endl;  
		for(int i=0; i<30; i++){
			User* usey7_5=read_id(i);
			stringstream capture_ss;
			string check=first_free(usey7_5, 'e');
			capture_ss.str(first_free(usey7_5, 'e'));
			string file_name=capture_ss.str();
			int line_num=-2, current_fle=-2, current_column=-2;
			capture_ss >> file_name >> line_num >> current_fle >> current_column;
			if(file_name!="-1"){		
				cout << "FOLLOWEES:\t" << usey7_5->username << "\t" << file_name << "\tline#: " << line_num << "\tfle_pos: " << current_fle
					 << "\tclmn pos: " << current_column << endl << endl;
			}
			else{
				cout << usey7_5->username << " must create a new file as they have : " << usey7_5->followees << " spots taken up "
					<< endl << "This is in fact a border value: " << usey7_5->followees << " % " << FLRS_PER_LINE << " = "
					<< usey7_5->followees%FLRS_PER_LINE << endl << endl;
			}
		}
		
		cout << endl << endl << "The results of load_following()ing are below: " << endl;  
		for(int i=0; i<40; i++){
			User* usey12=read_id(i%40);
			cout << "Selected user: " << usey12->username << " of id: " << usey12->id << " and type followees: "<< endl;
			vector<int> following=load_following(usey12, "followees", usey12->followees);
			for(size_t i=0; i< following.size(); i++) { cout << following[i] << "\t"; }
			following.clear();
			
			cout << endl << "Now showing followers: " << endl;
			following=load_following(usey12, "followers", usey12->followers);
			for(size_t i=0; i< following.size(); i++) { cout << following[i] << "\t"; }
			following.clear();
			cout << endl << endl;
			delete usey12;
			usey12=NULL;
		}
	
	
		cout << endl << endl << "Testing invalid arguments to load_following(): " << endl;
			User* usey13=read_id(rand()%40);
			cout << "Selected user: " << usey13->username << " of id: " << usey13->id << " and type followees: followees-5" << endl;
			vector<int> following=load_following(usey13, "followees", (usey13->followees)-5);
			for(size_t i=0; i< following.size(); i++) { cout << following[i] << "\t"; }
			following.clear();
		
			cout << endl << "Now showing followers: followers+5 " << endl;
			following=load_following(usey13, "followers", usey13->followers+5);
			for(size_t i=0; i< following.size(); i++) { cout << following[i] << "\t"; }
			following.clear();

			cout << endl << "Now showing followees: -5" << endl;
			following=load_following(usey13, "followees", -5);
			for(size_t i=0; i< following.size(); i++) { cout << following[i] << "\t"; }
			following.clear();	
			
			cout << endl << "Now showing followers: -5" << endl;
			following=load_following(usey13, "followers", -5);
			for(size_t i=0; i< following.size(); i++) { cout << following[i] << "\t"; }
			following.clear();
			
			delete usey13;
			usey13=NULL;
	
	
	if(cond=='y'){
		cout << endl << endl << "The results of write_woot()ing are below: " << endl;  
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
	cout << endl << endl << "The results of load_woot_seq()ing are below: " << endl;
	for( int i=0; i<40; i++) {
		User* usey15=read_id(i);
		vector<string>* woots=load_woots_seq(usey15, usey15->num_woots);
		cout << "User #: " << usey15->id << endl;
		for(size_t j=0; j< woots->size(); j++){ cout << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cout << endl << endl << endl;
		delete woots;
		woots=NULL;
		
		delete usey15;
		usey15=NULL;
	}
		
	cout << endl << endl << "The results of load_woot()ing are below: " << endl;
	for( int i=0; i<40; i++) {
		User* usey16=read_id(i);
		vector<string>* woots=load_woots(usey16, usey16->num_woots);
		cout << "User #: " << usey16->id << endl;
		for(size_t j=0; j < woots->size(); j++){ cout << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cout << endl << endl << endl;
		delete woots;
		woots=NULL;
		
		delete usey16;
		usey16=NULL;
	}
	
	
		cout << endl << endl << "Testing invalid arguments to load_woots(): " << endl;
		User* usey17=read_id(rand()%40);
		cout << "Selected user: " << usey17->username << " of id: " << usey17->id << " and type followees: woots-5" << endl;
		vector<string>* woots=load_woots(usey17, usey17->num_woots-5);
		for(size_t j=0; j < woots->size(); j++){ cout << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cout << endl << endl << endl;
		woots->clear();
	
		cout << endl << "Now showing woots: woots+5 " << endl;
		woots=load_woots(usey17, usey17->num_woots+5);
		for(size_t j=0; j < woots->size(); j++){ cout << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cout << endl << endl << endl;
		woots->clear();
	

		cout << endl << "Now showing woots: -5" << endl;
		woots=load_woots(usey17, -5);
		for(size_t j=0; j < woots->size(); j++){ cout << "Woot #: " << j << endl << (*woots)[j] << endl; }
		cout << endl << endl << endl;
		woots->clear();

		
		delete usey17;
		usey17=NULL;
	
	
}
	
	
	
// now test first_free once have unfollow making holes

// then loadfollowing, woot mgmt

// then figure out network code

// figure out message passing for what ops to do/what to return

// and then use .php tester files to test over the network from scratch
// and building up like now


//~ }
	
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
void flush_user(stringstream& file_name_ss, User* user_obj, int offset){
	/*
	flush_user() writes the attributes of $user_obj one by one into the file $file_handle
	at the offset $offset. Each attribute is padded to meet its maximum field size
	so that each field, line, and user file are of a fixed, constant size.
	*/
	fstream user_file;
	user_file.open(file_name_ss.str().c_str());
	if(!user_file){
		cout << "Error opening: " << file_name_ss.str() << endl;
		exit(6);
	}
	user_file.seekp(offset, user_file.beg);
	stringstream buf_ss;
	
	// probably could just overload operator<< so that it does this for me
	// outputs each of these to the given stream, can thus cout << user_obj
	// file << user_obj
	// file >> user_obj
	
	// sets a MINIMUM width to each field, be careful
	// set a fixed width stringstream to pad each field, then write to file
	buf_ss.width(MAX_USER_LEN); 						buf_ss << user_obj->username;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(MAX_PWD_LEN); 						buf_ss << user_obj->password;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(MAX_ID_LEN); 							buf_ss << user_obj->id;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(MAX_FNM_LEN+MAX_LNM_LEN+1); 			buf_ss << user_obj->full_name;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(4); 									buf_ss << user_obj->age;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(MAX_EML_LEN); 						buf_ss << user_obj->email;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(MAX_FLR_LEN); 						buf_ss << user_obj->followers;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(MAX_FLE_LEN); 						buf_ss << user_obj->followees;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(1); 									buf_ss << user_obj->free_bit;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss.width(MAX_NUM_WOOTS); 						buf_ss << user_obj->num_woots;		
	user_file << buf_ss.str();							buf_ss.str("");
	buf_ss << endl; 
	user_file << buf_ss.str();
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
	stringstream generic_ss; // will be reused for ints then chars
	generic_ss << USERS_PER_FILE-1;
	int num_digits=generic_ss.str().length();
	int current_free=atoi(current_free_ch.c_str());
	if(current_free==0){ 
		current_free=USERS_PER_FILE-1; 
		fids.clear();
	}
	else{ current_free--; } 
	
	// seek where needed to write back current_free, do so
	fids.seekp(line_num*(num_digits+1), fids.beg);
	generic_ss.str("");
	generic_ss << current_free;
	fids << generic_ss.str() << endl;
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
	stringstream file_name_ss;
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
	if(file_type=="users"){ file_name_ss << "users/u_" << file_row << ".txt"; }
	else if(file_type=="fids") { file_name_ss << "fids/fids.txt"; }
	else if(file_type=="followers") { file_name_ss << "flwrs/fr_r" << file_row << "c" << file_column << ".txt"; }
	else if(file_type=="followees") { file_name_ss << "flwes/fe_r" << file_row << "c" << file_column << ".txt"; }
	else if(file_type=="woots") { file_name_ss << "woots/w_r" << file_row << "c" << file_column << ".txt"; }
	
	// if valid file type passed in, create the file with empty lines of that type
	int result=access((file_name_ss.str()).c_str(), F_OK);
    if( (result == 0) ) { // if file does  exist
		cout << "Could not create: " << file_name_ss.str() << " as it already exists." << endl;
		exit(8);
	}
	bool valid_type = (file_type=="users") || (file_type=="fids") || (file_type=="woots") || (file_type=="followers") || (file_type=="followees");
	if(valid_type){
		stringstream one_line_ss;
		int num_lines=0;
		if(file_type=="users"){
			one_line_ss << string(MAX_ULINE_LEN-1, MY_DELIMITER);
			num_lines=USERS_PER_FILE;
		}
		else if(file_type=="fids"){
			one_line_ss << USERS_PER_FILE; // that many users slots are free when the file is made
			num_lines=1; // fids_decrease is called after making fids file/flushing user
		}
		else if(file_type=="followers" || file_type=="followees"){
			one_line_ss << string(MAX_ID_LEN*FLRS_PER_LINE, MY_DELIMITER);
			num_lines=USERS_PER_FILE;
		}
		else if(file_type=="woots"){
			one_line_ss << string(MAX_WOOT_LINE_LEN, MY_DELIMITER);
			num_lines=USERS_PER_FILE;			
		}
		one_line_ss << endl;
		ofstream new_file;
		new_file.open(file_name_ss.str().c_str());
		if(!new_file){
			cout << "Error opening: " << file_name_ss.str() << endl;
			exit(9);
		}
		for(int lines=0; lines<num_lines; lines++){ new_file << one_line_ss.str(); } // fill the file with empty line
		new_file.close();
	}
}

User* create_new_user(User* user_obj){    
    /*
    create_new_user() takes in a user object assembled from the registration page and fully
    integrates that user into the system. This is done by finding it a free user ID and by
    writing the user's attributes to the first available spot in the user file base.
    * The user object is then returned for convenience.
    * This function assumes that read_user() has been checked first before calling,
    * and does not further check if the user is in the database already.
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
	stringstream user_path_ss;
	user_path_ss << "/var/www/html" <<  "/users/u_" << current_line << ".txt";
	result=access(user_path_ss.str().c_str(), F_OK);
    if( (result < 0) && (errno == ENOENT) ) { 
		int user_id=current_line*USERS_PER_FILE;
		stringstream current_line_str_ss;
		current_line_str_ss << current_line;
		create_file("users", current_line_str_ss.str(), " ");		
		user_obj->id=user_id;
		user_obj->followers=0;
		user_obj->followees=0;
		user_obj->num_woots=0;
		user_obj->free_bit='n';
		flush_user(user_path_ss, user_obj, 0); // 0 since this is a new file being made
		fids_decrease((user_obj->id/USERS_PER_FILE));
		return user_obj;
	}
	
	// if the file exists, it has free space for new users, since we checked the fids
	else { 
		fstream user_file;
		user_file.open(user_path_ss.str().c_str());
		if(!user_file){
			cout << "Error opening: " << user_path_ss.str() << endl;
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
				flush_user(user_path_ss, user_obj, current_id*MAX_ULINE_LEN);								
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

	int result=access(file_name.c_str(), F_OK);
	if( (result<0)&&(errno=ENOENT) ){
		return NULL; // user's file does not exist, user does not exist
	}
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
	// stringstream buf_ss;
	// when calling unflush user, unlike in PHP now have to do
	// fseek($user_base, -$MAX_USER_LEN, SEEK_CUR);
	// before calculating offset to pass in
	
	User* user_obj=new User;
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
		stringstream file_name_ss;
		file_name_ss << "/var/www/html" <<  "/users/u_" << file_number << ".txt";
		int result=access((file_name_ss.str()).c_str(), F_OK);
		if( (result < 0) && (errno == ENOENT) ) { // if file does not exist
			cerr << "The file: " << file_name_ss.str() << " does not exist" << endl;
			return NULL; // return nothing if not found
		}
		else { // file exists
			fstream user_base;
			user_base.open(file_name_ss.str().c_str());
			if(!user_base){
				cerr << " The file: " << file_name_ss.str() << " could not be opened." << endl;
				exit(12);
			}	
			while(user_base.tellg() != -1){
				string read_username;
				user_base >> read_username;
				cout << "Read the username: " << read_username << endl;
				if(read_username == username){ // if it matches what we want, build and return that user object
					user_base.seekg(-MAX_USER_LEN, user_base.cur);
					int offset=user_base.tellg();
					User* user_obj=unflush_user(file_name_ss.str(), offset); // this could be null
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
	stringstream file_name_ss;
	file_name_ss << "/var/www/html" << "/users/u_" << id_num/USERS_PER_FILE << ".txt";
	return unflush_user(file_name_ss.str(), MAX_ULINE_LEN*(id_num % USERS_PER_FILE), true); // unsure if valid ID
	// could be null
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
	stringstream file_name_ss;
	file_name_ss << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
	unsigned int followees_read=0;
	int result=access(file_name_ss.str().c_str(), F_OK);
	unsigned int total_people=type=='r'?home_user->followers:home_user->followees;
	while( !((result < 0)&&(errno==ENOENT)) && followees_read < total_people){
		ifstream following;
		following.open(file_name_ss.str().c_str());
		int line_num=home_user->id % USERS_PER_FILE;
		following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		int flwrs_in_cur_file=FLRS_PER_LINE;
		string line;
		//getline(following, line);
		stringstream sub_line_ss;
		int last_token=-1;
		for(int current_fle=0; current_fle < flwrs_in_cur_file; current_fle++){ // flwrs follow is just a line of IDs separated by spaces
			int current_id=-1;
			//cout << "in file: " << file_name_ss.str() << endl;
			//line >> sub_line;
			char * buffer = new char [MAX_ID_LEN];
			following.read(buffer,MAX_ID_LEN); // no need to check if read succeeded, limiting it
			string temp(buffer);
			temp.resize(MAX_ID_LEN);
			sub_line_ss.str(temp);
			string empty(MAX_ID_LEN, MY_DELIMITER);
			delete[] buffer;
			buffer=NULL;
			
			if(sub_line_ss.str() != empty){		
				sub_line_ss.clear(); // after every read into int
				sub_line_ss >> current_id; // only read in an id if its not an empty line
					string debug(sub_line_ss.str());
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
					stringstream triplet_ss;
					triplet_ss << file_name_ss.str() << ' ' << line_num << ' ' << current_fle;
					return triplet_ss.str(); // yes by value, but what can you do
				}
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
		file_name_ss.str("");
		file_name_ss << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt" ;
		result=access(file_name_ss.str().c_str(), F_OK);
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
	//~ int result=access(file_name_ss.str().c_str(), F_OK);
	//~ while( !((result < 0)&&(errno==ENOENT)) && followers_read < other_user->followers){
		//~ ifstream following;
		//~ following.open(file_name_ss.str().c_str());
		//~ int line_num=other_user->id % USERS_PER_FILE;
		//~ following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		//~ int flwrs_in_cur_file=FLRS_PER_LINE;
		//~ for(int current_flr=0; current_flr < flwrs_in_cur_file; current_flr++){ // flwrs follow is just a line of IDs separated by spaces
			//~ int current_id;
			//~ following >> current_id;
			//~ if(current_id==follower_id) { 
				//~ stringstream triplet;
				//~ triplet << file_name_ss.str() << ' ' << line_num << ' ' << current_flr;
				//~ return triplet_ss.str(); // yes by value, but what can you do
			//~ }
			//~ 
			//~ // need to see what happens when keep reading and base it on that
			//~ // this is to handle removed followers leaving empty holes
			//~ //if(current_id!="") { $followers_read++;  }
		//~ }
		//~ following.close();
		//~ current_column++;
		//~ file_name_ss.str("");
		//~ file_name << "/var/www/html" << "/flwrs/fe_r" << user_row << "c" << current_column << ".txt" ;
		//~ result=access(file_name_ss.str().c_str(), F_OK);
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
	stringstream file_name_ss;
	file_name_ss << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
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
	
	int result=access(file_name_ss.str().c_str(), F_OK);
	while( !((result < 0)&&(errno==ENOENT)) && followers_read < group){ // a gap has been found
		ifstream following;
		following.open(file_name_ss.str().c_str());
		if(!following){
			cout << "Could not open file: " << file_name_ss.str() << endl;
			exit(14);
		}

		following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		int flwes_in_cur_file=FLRS_PER_LINE;
		// no && (followers_read < group) since want to finish line and get free spot here
		for(int current_fle=0; (current_fle < flwes_in_cur_file); current_fle++){
			char * buffer = new char [MAX_ID_LEN];
			following.read(buffer,MAX_ID_LEN); // no need to check if read succeeded, limiting it
			stringstream current_id_ss; // maybe do a getline and read that way?
			string cut_junk(buffer);
			cut_junk.resize(MAX_ID_LEN); // shouldnt pick up any more than that
			current_id_ss.str(cut_junk);
			//current_id_ss.str(buffer);
			int num=-1;
			string temp(current_id_ss.str());
			//cout << "ss str " << current_id_ss.str() << "temp: " << temp << " atoi: " << atoi(temp.c_str()) 
			//	<< " empty: " << temp.empty()<< endl;
			
			current_id_ss >> num;

			delete[] buffer;
			buffer=NULL;
			 // no longer needed
			string empty(MAX_ID_LEN, MY_DELIMITER);
			if(current_id_ss.str() == empty){
			//if(!num){
				current_id_ss.str("");
				current_id_ss.clear(); // something to do with outptuting to a number
				current_id_ss << file_name_ss.str();
				current_id_ss << " ";
				current_id_ss << line_num;
				current_id_ss << " ";
				current_id_ss << current_fle;
				current_id_ss << " " << current_column;
				string debug(current_id_ss.str());
				return current_id_ss.str();				
			}
			else{
				//cout << "This is the non empty current id str: " << current_id_ss.str() << endl;
				 followers_read++; }
		}
		following.close();
		current_column++;
		file_name_ss.str("");
		file_name_ss << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
		result=access(file_name_ss.str().c_str(), F_OK);
	}
	if( !((result < 0)&&(errno==ENOENT)) ) { // file exists
		stringstream current_id_ss;
		current_id_ss << file_name_ss.str() << ' ' << line_num << ' ' << 0 << ' ' << current_column;
		return current_id_ss.str(); // return the new file and spot to start filling
	}
	else{ // file doesn't exist, user must make it
		stringstream current_id_ss;
		current_id_ss << -1 << ' ' << line_num << ' ' << -1 << ' ' << current_column;
		return current_id_ss.str();
		// return other information for outside functions ??
	}
}
	
void gen_follow_func(User* home_user, User* other_user, char op, char group){
	// DOCUMENT HERE

	if( !(op=='f' || op=='u') || !(group=='1' || group=='2') ){
		cout << "Invalid options passed to gen_follow_func: " << op << "," << group << endl;
		exit(16);
	} // if invalid options passed, exit
	
	int followee_id=other_user->id;
	int follower_id=home_user->id;
	stringstream capture_ss;
	
	// catch the already_following or already_followed_by relationship
	string returned;
	if(group=='1'){ returned=already_following(home_user, followee_id); }
	else if(group=='2') { returned=already_followed_by(other_user, follower_id); }
	capture_ss.str(returned);
	string fnm;
	int lnm=-2, flx_num=-2;
	if( !(capture_ss.str().empty()) ){ 
		capture_ss >> fnm >> lnm >> flx_num;
	}
	
	// if the relationship doesn't exist, and you want to make it
	// or the relationship does exist, and you want to destroy it
	if( ((flx_num==-2)&&(op=='f')) || ((flx_num!=-2)&&(op=='u')) ){
		string file_name(fnm); // if unfollow(), you want the file where the relationship is
		// if follow, line below sets this to the proper file name
		int line_num=lnm, col_num=-2, col_file_num=-2;
		// if unfollow(), you want the line of the relationship
		// if follow, lines below set this appropriately
		
		// if follow is called, find first free spot for either relationship and set col_num to that
		// create a new file if necessary
		if(op=='f'){ 
			stringstream capture_ss_2;
			if(group=='1'){ capture_ss_2.str(first_free(home_user, 'e')); }
			else if(group=='2') { capture_ss_2.str(first_free(other_user, 'r')); }
			capture_ss_2 >> file_name >> line_num >> col_num >> col_file_num;
			
			// file does not exist yet and no free spots found; must make new file
			if(atoi(file_name.c_str())==-1){ 
				int row_val=-2;
				char type;
				if(group=='1'){ 
					row_val=home_user->id/USERS_PER_FILE; 
					type='e';
				}
				else if(group=='2') { 
					row_val=other_user->id/USERS_PER_FILE; 
					type='r';
				}
				
				// choose the name based on if home_user following other_user or other_user being followed by home_user
				stringstream file_name_ss;
				file_name_ss << "/var/www/html" <<  "/flw" << type << "s/f" << type << "_r" << row_val << "c" << col_file_num << ".txt";
				file_name=file_name_ss.str();  

				// here only because defined create_file() to take strings
				stringstream conv;
				conv << row_val << " " << col_file_num;
				string rv, cfn;
				conv >> rv >> cfn;
				// create the necessary file
				string file_type;
				if(group=='1') { file_type="followees"; }
				if(group=='2') { file_type="followers"; }
				create_file(file_type, rv , cfn );  
				col_num=0;
			} 
		} 
		
		fstream following; 
		following.open(file_name.c_str());
		if(!following){
			cerr << " The file: " << file_name << " could not be opened." << endl;
			exit(12);
		}	
		
		// go to either the first free spot, or the spot you want to remove
		int decision=-1;
		if(op=='f'){ decision=col_num; } // follow1 or follow2 decided above already
		else if(op=='u'){ decision=flx_num; } //unfollow1 or unfollow2 decided above by call to already_following or already_followed_by
		following.seekp(line_num*MAX_FLR_LINE_LEN + (MAX_ID_LEN*decision), following.beg);

		// write the ID you want to the file, or wipe it out if unfollowing
		stringstream buf_ss; 
		buf_ss.width(MAX_ID_LEN);
		if(op=='f' && group=='1'){ buf_ss << followee_id; } // follow1
		else if(op=='f' && group=='2') { buf_ss << follower_id; } // follow2
		else if(op=='u'){ buf_ss << string(MAX_ID_LEN, MY_DELIMITER);} //unfollow
		string temp(buf_ss.str());
		following << buf_ss.str();
		following.close();
		
		// update user follower/followee information
		if(op=='f' && group=='1'){ home_user->followees++; } // follow1
		else if(op=='f' && group=='2'){ other_user->followers++; } // follow2
		else if(op=='u' && group=='1'){ home_user->followees--; } // unfollow1
		else if(op=='u' && group=='2'){ other_user->followers--; } // unfollow2
		
		// select the user who has been updated, and update them in the user file base		
		int fnum=-2, lnum=-2;
		User* to_flush=NULL;
		if(group=='1'){ 
			fnum=home_user->id/USERS_PER_FILE; 
			lnum=(home_user->id % USERS_PER_FILE)*MAX_ULINE_LEN;
			to_flush=home_user;
		}
		else if(group=='2') { 
			fnum=other_user->id/USERS_PER_FILE; 
			lnum=(other_user->id % USERS_PER_FILE)*MAX_ULINE_LEN;
			to_flush=other_user;
		}
		if(to_flush!=NULL) { // just to be safe
			stringstream file_name_2_ss;
			file_name_2_ss << "/var/www/html" << "/users/u_" << fnum << ".txt";
			flush_user(file_name_2_ss, to_flush, lnum);
		}
	}
}

void follow(User* home_user, User* other_user){
	/*
	follow() creates a "is following" relationship between the logged in $home_user and the target $other_user.
	A reciprocating "is being followed by" is created between the target user and logged in user.
	*/
	gen_follow_func(home_user, other_user, 'f', '1');
	gen_follow_func(home_user, other_user,  'f', '2');
}	

void unfollow(User* home_user, User* other_user){
	/*
	unfollow() takes in a logged in $home_user and a target $other_user and destroys any "is following"
	relationship from the first to the second. It also destroys any reciprocating "is being followed by"
	relationship from the second to the first.
	A "lost follower" leaves an empty ID field for that portion of the file, which can then be recycled
	by future follow() attempts using first_free to find said empty field.
	* This is to prevent subsequent follow/unfollows from arbitrarily growing the # of files tracking this information.
	*/	
	gen_follow_func(home_user, other_user, 'u', '1');
	gen_follow_func(home_user, other_user, 'u', '2');
}

vector<int> load_following(User* home_user, string following_type, int num){ 
/* 
	load_following() takes in the logged in user object, the kind of following (followers 
	or followees), the # to read in, and then returns an array of those user IDs.
	*/
	char type;
	if(following_type=="followees") { type='e'; }
	else if(following_type=="followers") { type='r'; }
	int user_row=home_user->id/USERS_PER_FILE;
	int current_column=0;
	stringstream file_name_ss;
	file_name_ss << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c"
		<< current_column << ".txt";
	int followers_read=0;
	vector<int> cult;
	
	int result=access(file_name_ss.str().c_str(), F_OK);
	while( !((result < 0)&&(errno==ENOENT)) && followers_read < num){
		ifstream following;
		following.open(file_name_ss.str().c_str());
		int line_num=home_user->id%USERS_PER_FILE;
		following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		int flwrs_in_cur_file=FLRS_PER_LINE; 
		
		// once read all the followers to load, even if this line not done, quit
		for(int current_flr=0; (current_flr < flwrs_in_cur_file) && (followers_read < num); current_flr++){
			int current_id=-1;
			stringstream line_ss;
			
			char * buffer = new char [MAX_ID_LEN];
			following.read(buffer,MAX_ID_LEN); // no need to check if read succeeded, limiting it
			string temp(buffer);
			temp.resize(MAX_ID_LEN); // why it doesn't follow this I dont know
			line_ss.str(temp);
			string empty(MAX_ID_LEN, MY_DELIMITER);
			delete[] buffer;
			buffer=NULL;
			
			if(line_ss.str() != empty){		
				line_ss.clear(); // after every read into int
				line_ss >> current_id; // only read in an id if its not an empty line
			}
			if(current_id!=-1) {
				followers_read++;
				cult.push_back(current_id);
			}
			
		}
		following.close();
		current_column++;		
		file_name_ss.str("");
		file_name_ss << "/var/www/html" << "/flw" << type << "s/f" << type << "_r" << user_row << "c"
					 << current_column << ".txt";
		result=access(file_name_ss.str().c_str(), F_OK);
		
	}
	return cult; // by value, yes
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
void write_woot(User* home_user, string woot, string timestamp){
	// no getline, no >>, just do a read of the length per woot
	
	/*
	write_woot() takes in the logged in user object, the woot to write, and the timestamp
	associated with that woot and writes this to the woot file base.
	*/
	stringstream woot_ss;
	woot_ss.width(MAX_WOOT_LEN);
	woot_ss << woot;
	stringstream timestamp_ss;
	
	timestamp_ss.width(MAX_WOOT_TMSTP);
	timestamp_ss << timestamp;
	int row_val= home_user->id/USERS_PER_FILE;
	int col_val=home_user->num_woots/WOOTS_PER_LINE;
	// you go straight to the woot file to write to, no need to scan
	stringstream file_name_ss;
	file_name_ss << "/var/www/html" << "/woots/w_r" << row_val << "c" << col_val << ".txt";
	
	int result=access(file_name_ss.str().c_str(), F_OK);
	while( !((result<0)&&(errno==ENOENT)) ){
		// read woot by woot for empty spot, and write it and return
		fstream woot_file;
		woot_file.open(file_name_ss.str().c_str());
		if(!woot_file){
			cout << "Could not open file: " << file_name_ss.str() << endl;
			exit(18);
		}
		woot_file.seekg( (home_user->id%USERS_PER_FILE)*(1+MAX_WOOT_LINE_LEN) );
		// go to user's line there
		for(int index=0; index<WOOTS_PER_LINE; index++){
			char* buffer = new char[MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1];
			woot_file.read(buffer, MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			string cut_junk(buffer);
			cut_junk.resize(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			
			stringstream read_woot_ss;
			read_woot_ss.str(cut_junk);
			string empty(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1, MY_DELIMITER);
			if(read_woot_ss.str() == empty) {
				int current_pos=woot_file.tellg();
				
				woot_file.seekp(current_pos -(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1), woot_file.beg);
				woot_file << woot_ss.str() << ' ';
				woot_file << timestamp_ss.str() << ' ';
				woot_file.close();
				
				home_user->num_woots++;
				file_name_ss.str("");
				file_name_ss.clear();
				file_name_ss << "/var/www/html" << "/users/u_" << home_user->id/USERS_PER_FILE << ".txt";
				flush_user(file_name_ss, home_user, (home_user->id%USERS_PER_FILE)*MAX_ULINE_LEN );
				return;
			} // if a woot exits, just read the next woot
		}
		woot_file.close();
		col_val++;
		file_name_ss.str("");
		file_name_ss << "/var/www/html" << "/woots/w_r" << row_val << "c" << col_val << ".txt";
		result=access(file_name_ss.str().c_str(), F_OK);
	} // if exit while loop, no empty spot exists, make a new file
	
	stringstream conv;
	conv << row_val << " " << col_val;
	string rv, clv;
	conv >> rv >> clv;
	create_file("woots", rv, clv);
	
	fstream woot_file;
	woot_file.open(file_name_ss.str().c_str());
	if(!woot_file){
		cout << "Could not open file: " << file_name_ss.str() << endl;
		exit(18);
	}
	woot_file.seekp( (home_user->id%USERS_PER_FILE)*(1+MAX_WOOT_LINE_LEN)); // for EOL
	woot_file << woot_ss.str() << ' ';
	woot_file << timestamp_ss.str() << ' ';
	woot_file.close();
	
	home_user->num_woots++;
	file_name_ss.str("");
	file_name_ss.clear();
	file_name_ss << "/var/www/html" << "/users/u_" << home_user->id/USERS_PER_FILE << ".txt";
	flush_user(file_name_ss, home_user, (home_user->id%USERS_PER_FILE)*MAX_ULINE_LEN);

}

// leave these two as separate since too much code to mod
vector<string>* load_woots_seq(User* home_user, int num_woots){
	/*
	load_woots_seq() takes in the logged in user object, the number of woots to load, and returns
	an array of that many woots starting from the least recent one.
	*/
	return load_woots(home_user, num_woots, 'y');
	//~ int row_val=home_user->id/USERS_PER_FILE;
	//~ int col_val=0;//((home_user->num_woots -1)/WOOTS_PER_LINE);
	//~ stringstream file_name_ss;
	//~ file_name_ss << "/var/www/html" << "/woots/w_r" << row_val << "c" << col_val << ".txt";
	//~ int woots_read=0;
	//~ vector<string>* woots=new vector<string>;
	//~ 
	//~ int result=access(file_name_ss.str().c_str(), F_OK);
	//~ while( !( (result<0) && (errno=ENOENT) ) && woots_read < num_woots ){
		//~ // read woot by woot for empty spot, and write it and return
		//~ ifstream woot_file;
		//~ woot_file.open(file_name_ss.str().c_str());
		//~ if(!woot_file){
			//~ cin >> woots_read;
			//~ cout << "Could not open file: " << file_name_ss.str() << endl;
			//~ exit(18);
		//~ }
		//~ woot_file.seekg((home_user->id%USERS_PER_FILE)*(1+MAX_WOOT_LINE_LEN) );
		//~ // go to user's line there
		//~ for(int index=0; index < WOOTS_PER_LINE && woots_read < num_woots; index++){
			//~ char* buffer = new char[MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1];
			//~ woot_file.read(buffer, MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			//~ string cut_junk(buffer);
			//~ cut_junk.resize(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			//~ 
			//~ stringstream read_woot_ss;
			//~ read_woot_ss.str(cut_junk);
			//~ string empty(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1, MY_DELIMITER);
			//~ if(read_woot_ss.str() != empty) { 
				//~ woots_read++;
				//~ woots->push_back(read_woot_ss.str()); 
			//~ }
			//~ else { break; } // don't load empty woots, they passed in the wrong # of woots then
		//~ }	
		//~ woot_file.close();
		//~ col_val++;
		//~ file_name_ss.str("");
		//~ file_name_ss << "/var/www/html" << "/woots/w_r" << row_val << "c" << col_val << ".txt";
		//~ result=access(file_name_ss.str().c_str(), F_OK);
	//~ }
	//~ return woots;
}

// optional, left this in there because lazy
vector<string>* load_woots(User* home_user, int num_woots, char seq){
	/*
	load_woots() takes in the logged in user object, the number of woots to load, and returns
	an array of that many woots starting from the most recent one.
	*/
	int row_val=home_user->id/USERS_PER_FILE;
	int col_val=-1;
	if(seq=='y') { col_val=0; }
	else if(seq=='n') { col_val=((home_user->num_woots -1)/WOOTS_PER_LINE); } // change 1
	// int col_val=0;//((home_user->num_woots -1)/WOOTS_PER_LINE);

	stringstream file_name_ss;
	file_name_ss << "/var/www/html" << "/woots/w_r" << row_val << "c" << col_val << ".txt";
	int woots_read=0;
	vector<string>* woots=new vector<string>;	
	
	int result=access(file_name_ss.str().c_str(), F_OK);
	while( !( (result<0) && (errno=ENOENT) ) && woots_read < num_woots ){
		// read woot by woot for empty spot, and write it and return
		ifstream woot_file;
		woot_file.open(file_name_ss.str().c_str());
		if(!woot_file){
			cout << "Could not open file: " << file_name_ss.str() << endl;
			exit(18);
		}
		int next_line=-1;
		if(seq=='y') { next_line=0; }
		else if(seq=='n') { next_line=1; }
		woot_file.seekg( (next_line+home_user->id%USERS_PER_FILE)*(1+MAX_WOOT_LINE_LEN)-1 ); //new line character one time
		// woot_file.seekg((home_user->id%USERS_PER_FILE)*(1+MAX_WOOT_LINE_LEN) );
		
		// goes to the line AFTER yours, so you can read the timestamp and the last woot in first
		if(seq=='n'){
			if( woot_file.tellg()==-1){
				woot_file.clear();
				woot_file.seekg(0, woot_file.end);
			}
			woot_file.seekg(-(MAX_WOOT_TMSTP+1+MAX_WOOT_LEN+1), woot_file.cur);
		}
		// could eb used in both? no, only for laest recent
		// this is not here for load seq
		
		// go to user's line there
		 // +1 since trailing delimiters
			// not here if load_woot_seq
		for(int index=0; index < WOOTS_PER_LINE && woots_read < num_woots; index++){			
			char* buffer = new char[MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1];
			woot_file.read(buffer, MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			string cut_junk(buffer);
			cut_junk.resize(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			
			stringstream read_woot_ss;
			read_woot_ss.str(cut_junk);
			string empty(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1, MY_DELIMITER);
			// don't load any empty woots, but if 1 woot in entire line and working backwards
			// keep working until find a woot
			if(seq=='n'){
				woot_file.seekg(-(2)*(MAX_WOOT_TMSTP+1+MAX_WOOT_LEN+1), woot_file.cur); // +1 since trailing delimiters
			}
			if(read_woot_ss.str() != empty) { 
				woots_read++;
				woots->push_back(read_woot_ss.str()); 
			}
			else { continue; }
			delete[] buffer;
			// .read advanced the file position pointer, so unadvance it, then move back
			// this line simply not here if load_woot_seq
			
		}	
		woot_file.close();
		if(seq=='y') { col_val++; }
		else if(seq=='n') { col_val--; }
		//col_val--; // change 2
		//col_val++;
		file_name_ss.str("");
		file_name_ss << "/var/www/html" << "/woots/w_r" << row_val << "c" << col_val << ".txt";
		result=access(file_name_ss.str().c_str(), F_OK);
	}
	return woots;

}


	
//~ void unfollow1(User* home_user, User* other_user){
	//~ int followee_id=other_user->id;
	//~ 
	//~ stringstream capture_ss;
	//~ string returned;
	//~ returned=already_followed_by(other_user, follower_id);
	//~ capture_ss.str(returned);
	//~ string fnm;
	//~ int lnm=-2, fle_num=-2;
	//~ if( !(capture_ss.str().empty()) ){ // something was in fact returned
		//~ capture_ss >> fnm >> lnm >> fle_num;
	//~ }	
	//~ if(fle_num==-2){
		//~ 
	//~ list ($fnm, $lnm, $fle_num) = already_following($home_user, $followee_id);
	//~ if(isset($fle_num)){
		//~ $following=fopen($fnm, "r+") or die("Could not open file:".$fnm);
		//~ fseek($following, ($lnm*(1+$FLRS_PER_LINE*$MAX_ID_LEN)) + ($MAX_ID_LEN*$fle_num));
		//~ fwrite($following, str_repeat($MY_DELIMITER, $MAX_ID_LEN)); // remove them from your followees
		//~ fclose($following);
//~ 
		//~ $home_user->followees--; // decrease your number of followees
		//~ $u_file="users/u_".(int)($home_user->id/$USERS_PER_FILE).".txt";
		//~ $user_base=fopen($u_file, "r+") or die("Could not open file: ".$u_file);
		//~ flush_user($user_base, $home_user, ($home_user->id%$USERS_PER_FILE)*$MAX_ULINE_LEN);
		//~ fclose($user_base);
	//~ }
//~ }

//~ void unfollow2(User* home_user, User* other_user){
	//~ int follower_id=home_user->id;
	//~ list ($fnm_2, $lnm_2, $flr_num) = already_followed_by($other_user, $follower_id);
	//~ if(isset($flr_num)){ // if that user is already being followed by you
		//~ $following=fopen($fnm_2, "r+") or die("Could not open file: $fnm");
		//~ fseek($following, ($lnm_2*(1+$FLRS_PER_LINE*$MAX_ID_LEN)) + ($MAX_ID_LEN*$flr_num));
		//~ fwrite($following, str_repeat($MY_DELIMITER, $MAX_ID_LEN)); // remove yourself from their followers
		//~ fclose($following);
		//~ 
		//~ $other_user->followers--; // decrease their number of followers
		//~ $u_file="users/u_".(int)($other_user->id/$USERS_PER_FILE).".txt";
		//~ $user_base=fopen($u_file, "r+") or die("Could not open file: ".$u_file);
		//~ flush_user($user_base, $other_user, ($other_user->id%$USERS_PER_FILE)*$MAX_ULINE_LEN);
		//~ fclose($user_base);
	//~ }
//~ }
//~ 

//~ 
//~ void follow2(User* home_user, User* other_user){
	//~ int follower_id=home_user->id;
	//~ 
	//~ stringstream capture_ss;
	//~ string returned;
	//~ returned=already_followed_by(other_user, follower_id);
	//~ capture_ss.str(returned);
	//~ string fnm;
	//~ int lnm=-2, fle_num=-2;
	//~ if( !(capture_ss.str().empty()) ){ // something was in fact returned
		//~ capture_ss >> fnm >> lnm >> fle_num;
	//~ }	
	//~ if(fle_num==-2){	
		//~ stringstream capture_ss_2;
		//~ capture_ss_2.str(first_free(other_user, 'r'));
		//~ string file_name;
		//~ int line_num=-2, col_num=-2, col_file_num=-2;
		//~ capture_ss_2 >> file_name >> line_num >> col_num >> col_file_num;
		//~ if(atoi(file_name.c_str())==-1){ // file does not exist and no free spots found
			//~ int row_val=other_user->id/USERS_PER_FILE;
			//~ stringstream file_name_ss;
			//~ file_name_ss << "/var/www/html" <<  "/flwrs/fr_r" << row_val << "c" << col_file_num << ".txt";
			//~ stringstream conv;
			//~ conv << row_val << " " << col_file_num;
			//~ string rv, cfn;
			//~ conv >> rv >> cfn;
			//~ create_file("followers", rv , cfn ); 
			//~ col_num=0;
			//~ file_name=file_name_ss.str();
		//~ } // otherwise you return a file name and col num, colfilenum in the filename
		//~ 
		//~ 
		//~ //int result=access(file_name.c_str(), F_OK);// just open the file if it doesn't exist
		//~ fstream following;
		//~ following.open(file_name.c_str());
		//~ if(!following){
			//~ cerr << " The file: " << file_name << " could not be opened." << endl;
			//~ exit(12);
		//~ }	
		//~ 
		//~ following.seekp(line_num*MAX_FLR_LINE_LEN + (MAX_ID_LEN*col_num) );
		//~ stringstream buf_ss;
		//~ 
		//~ buf_ss.width(MAX_ID_LEN);
		//~ buf_ss << follower_id;		
		//~ string temp(buf_ss.str());
		//~ following << buf_ss.str();
		//~ following.close();
		//~ 
		//~ other_user->followers++; // update this value and flush it to the user base
		//~ 
		//~ stringstream file_name_2_ss;
		//~ file_name_2_ss << "/var/www/html" << "/users/u_" << other_user->id/USERS_PER_FILE << ".txt";
		//~ flush_user(file_name_2_ss, other_user, (other_user->id % USERS_PER_FILE)*MAX_ULINE_LEN);
	//~ }
	//~ 
//~ }
