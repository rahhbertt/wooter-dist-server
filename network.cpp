
#include <string>
#include <fstream>
#include <iostream>
#include <errno.h> // access, ENOENT
#include <sstream> // for concatenating ints and strings
#include <sys/stat.h> // for mkdir
#include <vector>

#include <thread> // for stage 3
#include <mutex>
#include <chrono>
#include <condition_variable>

#include <stdio.h>       // perror, snprintf
#include <stdlib.h>      // exit
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <strings.h>     // bzero
#include <time.h>        // time, ctime
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons

#define	MAXLINE		4096
#define	BUFFSIZE	8192
#define  SA struct sockaddr
#define	LISTENQ		1024
#define PORT_NUM    13092

using namespace std;
mutex globy; // currently testing this at handle php
mutex globy2; // currently testing this at mt_open
//~ vector<unique_lock<mutex>*> all_locks(1000); // arbitrary limit, would use larger number or transaction model in practice
//~ vector<string> file_names(1000);
int num_active=0;
class FileLock;
vector<FileLock*> file_locks(1000);


struct User{
public:
	string username, password;
	unsigned int id;
	string full_name;
	unsigned int age;
	string email;
	unsigned int followers, followees;
	char free_bit;
	unsigned int num_woots;
};

// exit(code) calls are only for impossible-state scenarios. if initial set up registers at least 1 user, everything taken care of okay
const int  MIN_USER_LEN=8, MAX_USER_LEN=24+1, MIN_PWD_LEN=10, MAX_PWD_LEN=32+1, MAX_ID_LEN=10+1; // at most 10 digits for the # of users
const int  MAX_FNM_LEN=25, MAX_LNM_LEN=25+1, MAX_EML_LEN=100+1, MAX_NUM_WOOTS=10; // at most 10 digits for the # of woots per user
// age is hardcoded to 4, 3 digits + 1 pad, so constant here
const int  MAX_FLR_LEN=10+1, MAX_FLE_LEN=10+1; // at most 10 digits for the # of followees
const int  MAX_ULINE_LEN=(24)+1+(32)+1+(10)+1+(25)+1+(25)+1+(4)+1+(100)+1+(10)+1+(10)+1+(10)+1;
//const int  MAX_ULINE_LEN=MAX_USER_LEN+MAX_PWD_LEN+MAX_ID_LEN+MAX_FNM_LEN+1+MAX_LNM_LEN+4+MAX_EML_LEN+MAX_FLR_LEN+MAX_FLE_LEN+1+MAX_NUM_WOOTS+1;
const int  USERS_PER_FILE=10, FID_HDR_LEN=5, FLRS_PER_LINE=10, MAX_FLR_LINE_LEN=(MAX_ID_LEN*FLRS_PER_LINE)+1;
const char MY_DELIMITER=' '; // while this is a global, the code will only function if this is a whitespace character
const int  WOOTS_PER_LINE=10, MAX_WOOT_LEN=100, MAX_WOOT_TMSTP=22, MAX_WOOT_LINE=(10)*(100+1+22+1);
const int  MAX_WOOT_LINE_LEN=WOOTS_PER_LINE*(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);

const int MSG_SIZE=560;
const string FILE_PATH="/var/www/html";


void net_connection(char** argv);
void function_tester(char cond='n');
void multi_threaded_tester(char cond='n');
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
	//~ net_connection(argv);
	multi_threaded_tester('y');
	return 0;
}

void reply(int connfd, string msg){
	/* 
	reply(connfd, msg) takes a connection file descriptor, and a message, and sends out
	* that variable length message over the network.
	*/
	stringstream reply_ss; // stringstream to append endl to message
	reply_ss << msg << endl; // NEED that endl for php to know line has ended
	cerr << endl << "Sending reply: " << reply_ss.str() << endl; // local output
	string reply_s(reply_ss.str()); // make a string out of the stream
	char* reply_c=new char[reply_s.size()]; // allocate space for a char* the size of our line
	reply_s.copy(reply_c, reply_s.size() ); // fill that char* with the line's contents
	write(connfd, reply_c, reply_s.size()); // write that char* with that size out to the network
	delete[] reply_c;
}

User* user_unexplode(stringstream& ss){
	/*
	user_unexplode(ss) takes in a stream string by reference that contains the received command
	* and parameters passed from the network, which it then uses to unpack the exploded, or marshalled, 
	* attributes of a user object. A new user object is then constructed with these attributes.
	* The caller of this function is responsible for deallocating this heap memory.	
	*/
	string username, password, first_name, last_name, email;
	unsigned int age, id, followers, followees, num_woots;
	char free_bit;
	ss >> username >> password >> id;
    ss.clear(); // after every read into int
    ss >> first_name >> last_name >> age;
    ss.clear();
    ss >> email >> followers;
    ss.clear();
    ss >> followees;
    ss.clear(); 
    ss >> free_bit >> num_woots;
    User* newbie=new User;
    newbie->username=username;
    newbie->password=password;
    newbie->id=id;
    newbie->full_name=first_name +' '+ last_name;
    newbie->age=age;
    newbie->email=email;
	newbie->followers=followers; // these will be rewritten if calling create_new_user
	newbie->followees=followees; // but need them to have a dummy value when passed to the server
	newbie->free_bit=free_bit;
	newbie->num_woots=num_woots;
	return newbie;
}

string user_explode(User* user_obj){
	/*
	user_explode(User*) takes in a pointer to a user object and marshals all of its attributes
	into a string that can be sent over the network.
	*/
	stringstream buf_ss;
	buf_ss.width(MAX_USER_LEN); 				buf_ss << user_obj->username;		
	buf_ss.width(MAX_PWD_LEN); 					buf_ss << user_obj->password;		
	buf_ss.width(MAX_ID_LEN); 					buf_ss << user_obj->id;		
	buf_ss.width(MAX_FNM_LEN+MAX_LNM_LEN+1);	buf_ss << user_obj->full_name;		
	buf_ss.width(4); 							buf_ss << user_obj->age;		
	buf_ss.width(MAX_EML_LEN); 					buf_ss << user_obj->email;		
	buf_ss.width(MAX_FLR_LEN); 					buf_ss << user_obj->followers;		
	buf_ss.width(MAX_FLE_LEN); 					buf_ss << user_obj->followees;		
	buf_ss.width(1); 							buf_ss << user_obj->free_bit;		
	buf_ss.width(MAX_NUM_WOOTS); 				buf_ss << user_obj->num_woots;		
	return buf_ss.str();
}

string cult_explode(vector<int> cult){
	/*
	cult_explode(vector<int>) takes in a vector of user IDs, corresponding to some user's list of 
	followers or followees, and marshals this vector by packing the IDs into one space-delimited
	string to be sent over the network.
	*/
	stringstream buf_ss;
	for(size_t i=0; i<cult.size(); i++){
		buf_ss.width(MAX_ID_LEN);
		buf_ss << cult[i];
		buf_ss.clear();
	}
	return buf_ss.str();
}

string unexplode_woot(stringstream& ss){
	/*
	unexplode_woot(ss) takes in a stream string by reference that contains the received command
	* and parameters passed from the network, which it then uses to unpack the woot mesage sent to
	* be written to the server. This woot is returned as a string.
	*/
	string big_line(ss.str());
	char* woot_c=new char[MAX_WOOT_LEN];
	big_line.copy(woot_c, MAX_WOOT_LEN, 40+MAX_ULINE_LEN); // hard coded command field size
	string to_return(woot_c);
	to_return.resize(MAX_WOOT_LEN);
	delete[] woot_c;
	woot_c=NULL;

	return to_return;
}

string unexplode_ts(stringstream& ss){
	/*
	unexplode_ts(ss) takes in a stream string by reference that contains the received command
	* and parameters passed from the network, which it then uses to unpack the woot mesage sent to
	* be written to the server. The timestamp of that woot is returned as a string.
	*/
	string big_line(ss.str());
	char* ts_c=new char[MAX_WOOT_TMSTP];
	big_line.copy(ts_c, MAX_WOOT_TMSTP, 40+MAX_ULINE_LEN+MAX_WOOT_LEN+1); // hard coded command field size
	string to_return(ts_c);
	to_return.resize(MAX_WOOT_TMSTP);
	delete[] ts_c;
	ts_c=NULL;

	return to_return;
}

void reply_pack(int connfd, vector<string>* items, unsigned int object_size){
	/*
	reply_pack(connfd, vector<Item>*) takes in a pointer to a vector of Items (string or ints), representing a list of
	* woot messages belonging to some one user, or IDs belonging to some user's list of followers/followees
	*  and proceeds to send those strings out over the network however many fit in a message at a time. 
	* Since the default message size is MSG_SIZE bytes, and each item is object_size bytes (with 1 included for spacing),
	* MSG_SIZE/object_size items are sent over per message.
	*/
	stringstream reply_ss;
	int num_packaged=0;
	unsigned int objs_per_msg=MSG_SIZE/object_size;
	int iterations=((*items).size()/objs_per_msg)+((*items).size()%objs_per_msg!=0?1:0);
	for(int i=0; i<iterations; i++){
		reply_ss.str("");
		for( size_t j=0; j<objs_per_msg && num_packaged<(int)(*items).size(); num_packaged++, j++) { 
			reply_ss << (*items)[num_packaged]; 
			reply_ss.clear();
		}
		reply(connfd, reply_ss.str()); 
	} // space delimiter already included between woots
}

void reply_pack_str(int connfd, vector<int>* items, unsigned int object_size){
	/*
	reply_pack_str(connfd, vector<Item>*) performs the job of reply_pack for IDs packed into one string.
	*/
	stringstream reply_ss;
	int num_packaged=0;
	unsigned int objs_per_msg=MSG_SIZE/object_size;
	int iterations=((*items).size()/objs_per_msg)+((*items).size()%objs_per_msg!=0?1:0);
	for(int i=0; i<iterations; i++){
		reply_ss.str("");		
		for( size_t j=0; j<objs_per_msg && num_packaged<(int)(*items).size(); num_packaged++, j++) { 
			reply_ss.width(MAX_ID_LEN);
			reply_ss << (*items)[j];
			reply_ss.clear();
			reply_ss << " ";
		}
		reply(connfd, reply_ss.str()); 
	} // space delimiter already included between woots
}
void handle_php(int connfd, char* cmd, int cmd_size){
	/*
	handle_php(connfd, cmd, cmd_size) takes in a connection file descriptor (connfd), a received command
	* from the network (cmd), and the length of that command. It then tokenizes the message received
	* to extract the command type given, and enters a block of switch statements to decide how to 
	* respond to each specified command type.
	* Note: no error messages are passed back to php. Instead objects are returned
	* that the php code expects in the case of an error, and is already checking for,
	* such as an empty user object returned on a read_user() command.
	* 	Thus all exit(code) lines are simply for local testing and impossible-case debugging of the .cpp code.
	*/	
	
	stringstream received_ss;// make a stream out of the message
	received_ss.str(string(cmd));
	string received_str;
	received_ss >> received_str; // read in the comand token
	
	// switch block based on passed in command
	if(received_str=="create_new_user"){
		User* newbie=user_unexplode(received_ss);
		if(newbie!=0){ 
			create_new_user(newbie);
			reply(connfd, user_explode(newbie) ); 
		}
		delete newbie;
		newbie=NULL;
	}
	else if(received_str=="read_user"){
		string username;
		received_ss >> username;
		User * read_out=read_user(username);
		if(read_out!=NULL){ reply(connfd, user_explode(read_out)); }
		delete read_out;
		read_out=NULL;
	}
	else if(received_str=="flush_user"){
		User* flush=user_unexplode(received_ss);
		if(flush!=0){ 
			stringstream file_path_ss;
			file_path_ss << FILE_PATH <<  "/users/u_" << flush->id/USERS_PER_FILE << ".txt";
			flush_user(file_path_ss, flush, (flush->id%USERS_PER_FILE)*(MAX_ULINE_LEN) );
			reply(connfd, user_explode(flush) ); // unnecessary network traffic, but just in case
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
	}
	else if(received_str=="follow"){
		User* home_user=user_unexplode(received_ss);
		User* other_user=user_unexplode(received_ss); // since by &, this should work
		if( home_user!=NULL && other_user != NULL){ follow(home_user, other_user); }
	}
	else if(received_str=="unfollow"){
		User* home_user=user_unexplode(received_ss);
		User* other_user=user_unexplode(received_ss); // since by &, this should work
		if( home_user!=NULL && other_user != NULL){ unfollow(home_user, other_user); }
	}
	else if(received_str=="load_following"){ 
		// a real practical limit of MSG_SIZE bytes to load_following at one time
		User* home_user=user_unexplode(received_ss);
		string group_type;
		received_ss >> group_type;
		int num_ppl=-1;
		received_ss >> num_ppl;
		received_ss.clear();
		if(home_user!=NULL){ 
			vector<int> cult=load_following(home_user, group_type, num_ppl);
			reply_pack_str(connfd, &cult, MAX_ID_LEN+1); // +1 for spacing
		}
	}
	else if(received_str=="write_woot"){
		User* home_user=user_unexplode(received_ss);
		string woot=unexplode_woot(received_ss);
		string timestamp=unexplode_ts(received_ss);
		if(home_user!=NULL){
			write_woot(home_user, woot, timestamp);
			reply(connfd, user_explode(home_user) ); // have to increment num_woots
		} 
	}
	else if(received_str=="load_woots"){
		User* wooter=user_unexplode(received_ss);
		int num_woots=-1;
		received_ss >> num_woots;
		received_ss.clear();
		if(wooter!=NULL){
			vector<string>* woots=load_woots(wooter, num_woots);
			reply_pack(connfd, woots, MAX_WOOT_LEN+MAX_WOOT_TMSTP+1); // +1 for spacing
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
			reply_pack(connfd, woots, MAX_WOOT_LEN+MAX_WOOT_TMSTP+1);
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
	else { reply(connfd, received_str); } //invalid commands handled here

	// about to close connection, clean up
	delete cmd;
	cmd=nullptr;
	close(connfd);
}

void net_connection(char** argv){
	/*
	  From Stevens Unix Network Programming, vol 1.
	  Minor modifications by John Sterling
	  Further minor modifications (in step 5) by Robert Ryszewski
	 */
	int	listenfd, connfd;  // Unix file descriptors. its just an int
    struct sockaddr_in	servaddr;  // Note C use of struct

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
    
	servaddr.sin_port        = htons(PORT_NUM);	// wooter file server

    // 3. "Bind" that address object to our listening file descriptor
	if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) == -1) {
		perror("Unable to bind port");
		exit(2);
	}

    // 4. Tell the system that we are going to use this sockect for
    //    listening and request a queue length
	if (listen(listenfd, LISTENQ) == -1) {
		perror("Unable to listen");
		exit(3);
	} 
	


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

   		// We had a connection.  Do whatever our task is.
		//~ char cmd[MSG_SIZE];
		char* cmd= new char[MSG_SIZE];
		
		int read_well=read(connfd, cmd, MSG_SIZE);
		cmd[MSG_SIZE-1]='\0'; // stringstreams life is easier
		
		cout << "Received cmd: " << cmd << endl;
		
		if(read_well==MSG_SIZE){ 
			// copy connfd by value so can create a new connection and not lose this fd
			// same with the rest
			//~ char* new_cmd=cmd;
			//~ cout << "Thread is getting command:" << cmd << endl;
			//~ globy.lock();
			//~ request_lock.lock();
			thread client_request([connfd, cmd, MSG_SIZE] { handle_php(connfd, cmd, MSG_SIZE); });
			client_request.detach(); // so if main exits we dont crash everything, just in case?

			//~ cout << "Thread got command:" << cmd << endl;
			// 6. Close the connection with the current client and go back
			//    for another. AFTER this client has finished on that connection
			// inside process php now
			//~ handle_php(connfd, cmd, MSG_SIZE);
			//~ delete cmd;
			//~ cmd=nullptr;
		}
    	// else fails silently
		//~ delete cmd;
		//~ cmd=NULL;
	//~ close(connfd);
	}
	 
	// for threading, had to make following changes
	// 1) close(connfd) moved to end of handle_php() so that new connfds can be grabbed for each conn
	// 2) cmd is now a dynamic array, responsibility and deletion handed over to handle_php()
	// 3) read() now does MSG_SIZE since sizeof(cmd) wont work since not stack variable anymore
	// 4) if(read_well==sizeof(cmd)) now also MSG_SIZE instead of sizeof(cmd)
	// a single thread, a single request works fine now
	
	// fixed tester code. running a loop of read user, write woot didnt evne work
	// when sequential when ran two at same time. 
	// test A would read user, write a woot, read user, write a woot
	// test B would begin. test A does read user. test B does read user, write woot
	// now test A writes woot. the woot count is relative to A's stale read of the user
	// thus woot count gets "lost", lower than it should be total
	
}

void test_follow(){
	vector<thread> join_back2;
	for(int i=0; i<40; i++){
		User* usey8=read_id(i%40);
		for(int j=i+20; j<i+60; j++){
			User* usey9;
			if((i%40)==(j%40)){ usey9=read_id((j+1)%40); }
			else { usey9=read_id((j)%40); }
			if(usey8!=NULL && usey9!=NULL) {  
				join_back2.push_back(thread([&] { follow(usey8, usey9); } ));
			}
			usey9=NULL;
		}
		usey8=NULL;
	}
	for(size_t i=0; i<join_back2.size(); i++){
		join_back2[i].join();
		cout << "flw_joined #" << i << endl;
	}
}


void multi_threaded_tester(char cond){	
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
			////~ join_back.push_back(thread([i] { cout << "thread: " << i << endl; } ) );
		//	//~ join_back[join_back.size()-1].detach();
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
	
	// read users by username
	vector<thread> join_back2;
	for(int i=0; i <40; i++){ 				
		string original_name="test_accy";
		
		stringstream fullest_name_ss;
		fullest_name_ss << original_name << i;
		fullest_name_ss.clear();
		original_name=fullest_name_ss.str();
		fullest_name_ss.str("");

		join_back2.push_back(thread([original_name, i] { cout << "ZZ\ti:\t" << i << endl << user_explode(read_user(original_name)) << endl; }) );
		//~ //join_back[join_back.size()-1].detach();
		// heap memory being lost here
		join_back2.push_back(thread([i] { User* read_u=read_id(i); cout << "QQ\ti:\t" << i << "\t"; if(read_u!=NULL){ cout << user_explode(read_id(i)) << endl; } } ));
	
	}

	//~ test_follow();

	for(size_t i=0; i<join_back2.size(); i++){
		join_back2[i].join();
		cout << "end_joined #" << i << endl;
	}


	cout << endl << " ending mt_tester " << endl;
}

class FileLock{
	public:
	FileLock(){}
	FileLock(const string& file_path) : file_name(file_path) { cout << "FileLock created for: " << file_path << endl; }
	string get_name() { return file_name; }
	void file_lock();
	void file_unlock();
	mutex& mut();
	
	private:
	string file_name;
	mutex file_mut;
	unique_lock<mutex> locked;
};

void FileLock::file_lock(){ 
	cout << "Thread: " << this_thread::get_id() << "\t\t\twait for\tlock: " << file_name << endl;
	locked=unique_lock<mutex>(file_mut); 
	cout << "Thread: " << this_thread::get_id() << " \t\tobtained\tlock: " << file_name << endl;
}
void FileLock::file_unlock(){ 
	cout << "Thread: " << this_thread::get_id() << " \t\tpre-release\tlock: " << file_name << endl;	
	locked.unlock(); 
	cout << "Thread: " << this_thread::get_id() << " \t\treleased\tlock: " << file_name << endl;
}
mutex& FileLock::mut(){ return file_mut; }

// mt_open and mt_close should not open and close actual files
// if you want to deal with a file, check existence, etc, mt_open
//~ int mt_open(string path){
	//~ /* 
	//~ Distributing locks here is a critical region in itself.
	//~ A condition variable is unnecessary as the "condition" has only two values: somebody is in the region or they are not.
	//~ * Thus a simple big lock suffices. A cond_var however would do away with the busy waiting of a lock. 
	//~ * Thus a cond_var may be used in future revisions.
	//~ 
	//~ A big lock is necessary here so that there are no race conditions in checking if a given FileLock
	//~ * has already been created, and double creating. This does introduce a bottleneck of sequentialism
	//~ * but it allows for creating a lock per file object, and thus is far less sequential than a single big lock for EVERY file access.
//~ 
	//~ */
	//~ unique_lock<mutex> global_ul(globy2); // lock this critical region
	//~ cout << "Thread: " << this_thread::get_id() << "\t\taccessed\tglobal lock\tin mt_open" << endl;
	//~ int lock_pos=0;
	//~ bool found=false;
	//~ while( lock_pos < num_active ){ // try to find the position of the desired FielLock
		//~ if(file_locks[lock_pos]->get_name()==path){ 
			//~ found=true;
			//~ break;
		//~ }
		//~ lock_pos++;
	//~ }
	//~ if(!found){ // if not found, create it on the heap to avoid needing to deal with move construction
		//~ FileLock* new_fl= new FileLock(path);
		//~ file_locks[num_active]=(new_fl); 
		//~ num_active++; // using a fixed size vector to avoid resizing/move construction. thus inc the real count
		//~ global_ul.unlock(); 
		//~ // MUST unlock this region before attempting to lock your FileLock, so that a single FileLock attempt does not hold up EVERY call to mt_open
		//~ // that would simply lead to deadlocks. this way only threads waiting on the same FileLock object wait on each other		
		//~ new_fl->file_lock(); 
	//~ } else{ // it was found
		//~ global_ul.unlock(); // unlock this critical region, action is done, again must do so to avoid deadlock
		//~ 
		//~ file_locks[lock_pos]->file_lock();
	//~ }	
	//~ cout << "Thread: " << this_thread::get_id() << "\t\treleased\tglobal lock\tin mt_open" << endl;
	//~ return lock_pos;	
//~ }

mutex& mt_open(string path){
	/* 
	Distributing locks here is a critical region in itself.
	A condition variable is unnecessary as the "condition" has only two values: somebody is in the region or they are not.
	* Thus a simple big lock suffices. A cond_var however would do away with the busy waiting of a lock. 
	* Thus a cond_var may be used in future revisions.
	
	A big lock is necessary here so that there are no race conditions in checking if a given FileLock
	* has already been created, and double creating. This does introduce a bottleneck of sequentialism
	* but it allows for creating a lock per file object, and thus is far less sequential than a single big lock for EVERY file access.

	*/
	unique_lock<mutex> global_ul(globy2); // lock this critical region
	cout << "Thread: " << this_thread::get_id() << "\t\twaiting on\tlock:\t" << path << endl;
	int lock_pos=0;
	bool found=false;
	while( lock_pos < num_active ){ // try to find the position of the desired FielLock
		if(file_locks[lock_pos]->get_name()==path){ 
			found=true;
			break;
		}
		lock_pos++;
	}
	if(!found){ // if not found, create it on the heap to avoid needing to deal with move construction
		FileLock* new_fl= new FileLock(path);
		file_locks[num_active]=(new_fl); 
		num_active++; // using a fixed size vector to avoid resizing/move construction. thus inc the real count
		cout << "Thread: " << this_thread::get_id() << "\t\tobtained\tlock:\t" << path << endl;
		global_ul.unlock(); 
		// MUST unlock this region before attempting to lock your FileLock, so that a single FileLock attempt does not hold up EVERY call to mt_open
		// that would simply lead to deadlocks. this way only threads waiting on the same FileLock object wait on each other		
		return new_fl->mut();
	} else{ // it was found
		cout << "Thread: " << this_thread::get_id() << "\t\tobtained\tlock:\t" << path << endl;
		global_ul.unlock(); // unlock this critical region, action is done, again must do so to avoid deadlock
		return file_locks[lock_pos]->mut();
	}	

	// serious error
	//~ return lock_pos;	
}

void mt_close(int lock_pos){
	cout << "Thread: " << this_thread::get_id() << "\tclosing file: " << file_locks[lock_pos]->get_name() << endl;
	//~ user_file.close(); // the file should be flushed BEFORE releasing the lock
	file_locks[lock_pos]->file_unlock();
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

void flush_user(stringstream& file_name_ss, User* user_obj, int offset){
	/*
	flush_user() writes the attributes of $user_obj one by one into the file $file_handle
	at the offset $offset. Each attribute is padded to meet its maximum field size
	so that each field, line, and user file are of a fixed, constant size.
	*/
	fstream user_file;
	user_file.open(file_name_ss.str().c_str());
	if(!user_file){
		cerr << "Error opening: " << file_name_ss.str() << endl;
		exit(6);
	}
	user_file.seekp(offset, user_file.beg);
	stringstream buf_ss;
	
	// set a fixed width stringstream to pad each field, then write to file
	buf_ss.width(MAX_USER_LEN); 						
	buf_ss << user_obj->username;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(MAX_PWD_LEN); 						
	buf_ss << user_obj->password;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(MAX_ID_LEN); 							
	buf_ss << user_obj->id;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(MAX_FNM_LEN+MAX_LNM_LEN+1); 			
	buf_ss << user_obj->full_name;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(4); 									
	buf_ss << user_obj->age;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(MAX_EML_LEN); 						
	buf_ss << user_obj->email;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(MAX_FLR_LEN); 						
	buf_ss << user_obj->followers;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(MAX_FLE_LEN); 						
	buf_ss << user_obj->followees;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(1); 									
	buf_ss << user_obj->free_bit;		
	user_file << buf_ss.str();							
	buf_ss.str("");
	buf_ss.width(MAX_NUM_WOOTS); 						
	buf_ss << user_obj->num_woots;		
	user_file << buf_ss.str();							
	buf_ss.str("");
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
		cerr << "Error opening: " << fids_path << endl;
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
			cerr << "The directory: " << start_path << test_path << " does not exist" << endl;
			mkdir((start_path+test_path).c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
			cerr << "Have created dir! " << endl;
		}
		else{
			//cerr << "The directory: " << start_path << test_path << " already exists" << endl;
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
    	cerr << "Could not create: " << file_name_ss.str() << " as it already exists." << endl;
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
			cerr << "Error opening: " << file_name_ss.str() << endl;
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
    Returns a NULL pointer if failed somehow.
    */
	
	// no read/write locks are implemented to save time and complexity, but that does mean busy waiting on the fids when no writing is taking place
	// justifiable by the low volume of simultaneous create_new_user calls
	// overall approach: lock fids, lock necessary file, unlock necessary file, unlock fids; one atomic operation
	// code is not very exception safe since, although using a unique lock, its global and won't scope out on a given thread getting an exception
 
    //~ cout << "Thread: " << this_thread::get_id() << " entered create_new_user " << endl;
    //~ cout << "User_obj: username=" << user_obj->username << endl;   
    int current_line=0;
	// critical region determining if fids exists, reading it, and updating it
    string fids_path = "/var/www/html/fids/fids.txt";  
	
	//~ int lock_fids=mt_open(fids_path); // do so to allow only one access (read or write) to fids at a given time
    unique_lock<mutex> fids_ul(mt_open(fids_path));
    
    int result=access(fids_path.c_str(), F_OK); // keep the fids lock until the end of the function
    
    if( (result < 0) && (errno == ENOENT) ) { // if file does not exist
    	create_file("fids","0","0"); 
    }
	else{ // if file does exist
		fstream fids; // gave up the "i" so that wouldnt have to template mt_open
		fids.open(fids_path.c_str());
		if(!fids){
			cerr << "Error opening: " << fids_path << endl;
			fids.close();
			//~ mt_close(lock_fids); // release the lock if exiting
			fids_ul.unlock();
			
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
	user_path_ss << FILE_PATH <<  "/users/u_" << current_line << ".txt";
	
	//~ int lock_uf=mt_open(user_path_ss.str()); // lock the file you're about to create or mod
	unique_lock<mutex> uf_ul(mt_open(user_path_ss.str()));
	
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
		fids_decrease((user_obj->id/USERS_PER_FILE)); // this is why still need the fids_lock
		flush_user(user_path_ss, user_obj, 0); // 0 since this is a new file being made
		//~ mt_close(lock_uf); // release the user file lock first
		uf_ul.unlock();
		//~ mt_close(lock_fids); // then release the fids lock, to respect this ordering
		fids_ul.unlock();
		
		return user_obj;
	} 
	else { // if the file exists, it has free space for new users, since we checked the fids
		// if file exists, you still have the lock on the file, proceed
		fstream user_file;
		user_file.open(user_path_ss.str().c_str());
		if(!user_file){
			cerr << "Error opening: " << user_path_ss.str() << endl;
			//~ mt_close(lock_uf); // release user lock, then fids lock, respect the ordering
			uf_ul.unlock();
			fids_ul.unlock();
		
			//~ mt_close(lock_fids);
			exit(7);
		}
		int current_id=0;
		char current_bit='\0';
		int bit_pos=MAX_ULINE_LEN-MAX_NUM_WOOTS-1-1;
		user_file.seekg(bit_pos, user_file.cur);		
		while(current_id<USERS_PER_FILE){ // scan until you find the first free spot
		 	current_bit='\0';
		 	user_file >> current_bit;
			if(current_bit=='\000'){  // fstream wont read whitespace
				user_obj->id=current_id+(USERS_PER_FILE*current_line);
				user_obj->followers=0;
				user_obj->followees=0;
				user_obj->num_woots=0;
				user_obj->free_bit='n';
				user_file.close(); 
				// no actual mt_open IN fids_decrease of flush_user
				// protected by the fids lock
				fids_decrease(user_obj->id / USERS_PER_FILE);
				// protected by the overarching mt_open on this user file
				flush_user(user_path_ss, user_obj, current_id*MAX_ULINE_LEN);								
				//~ mt_close(lock_uf);	// respect the order	
				//~ mt_close(lock_fids);	
				uf_ul.unlock();
				fids_ul.unlock();
			
				return user_obj;
			}
			user_file.seekg(MAX_ULINE_LEN-1, user_file.cur);
			current_id+=1;
		}
	}
	//~ mt_close(lock_uf); // respect the other
	//~ mt_close(lock_fids);
	uf_ul.unlock();
	fids_ul.unlock();
		
	return nullptr;
}

User* unflush_user(string file_name, int offset, bool unsure){	
	/*
	unflush_user() reads the attributes of $user_obj one by one from the file $file_handle
	at the given file position. Each attribute's padding is removed upon reading from file.

	It assumes a valid file_name and offset, does not error check for misuse.
	* The caller should use it properly.
	* A null pointer is returned if user could not be unflushed.
	* A valid return of a user object is one allocated on the heap. Caller is responsible for freeing up this memory.
	*/
	if(offset > MAX_ULINE_LEN*USERS_PER_FILE) { 
		cerr << "Error: tried to read line # " << offset/MAX_ULINE_LEN << " when only " << USERS_PER_FILE << " user lines per file " << endl;
		exit(10);
	} 
	
	int result=access(file_name.c_str(), F_OK);
	if( (result<0)&&(errno=ENOENT) ){
		return NULL; // user's file does not exist, user does not exist
	}
	ifstream user_base;
	user_base.open(file_name.c_str());
	if(!user_base && !unsure){
		cerr << "Error opening: " << file_name << endl;
		exit(11);
	}
	else if(!user_base && unsure){ // if doing read_id and id might be arbitrary
		cerr << "Error: the file " << file_name << " does not exist in the user base" << endl;
		return NULL; 
	}
	user_base.seekg(offset, user_base.beg);

	// create a new user object to pull out of the file database
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

User* read_user(string username){
	/*
	read_user() takes in a username string, then finds it in the user file base and returns its user object.
	If the user does not exist in the user file base, a void return takes place.
	* A null pointer is returned if somehow fails.
	Othewise a new user object pointer is returned, which the caller
	is responsible for freeing up the memory of after use.
	*/
	//~ int file_lock_num=-1;
	int file_number=0;
	while(file_number>-1){ // search every user file
		stringstream file_name_ss;
		file_name_ss << FILE_PATH <<  "/users/u_" << file_number << ".txt";
		
		//~ file_lock_num=mt_open(file_name_ss.str());
		unique_lock<mutex> file_lock(mt_open(file_name_ss.str()));
		
		int result=access((file_name_ss.str()).c_str(), F_OK);
		if( (result < 0) && (errno == ENOENT) ) { // if file does not exist
			cerr << "The file: " << file_name_ss.str() << " does not exist" << endl;
			
			//~ mt_close(file_lock_num);
			file_lock.unlock();
			
			return NULL; // return nothing if not found
		}
		else { // file exists
			fstream user_base;
			user_base.open(file_name_ss.str().c_str());
			if(!user_base){
				cerr << " The file: " << file_name_ss.str() << " could not be opened." << endl;
				
				//~ mt_close(file_lock_num);
				file_lock.unlock();
				
				exit(12);
			}	
			while(user_base.tellg() != -1){
				string read_username;
				user_base >> read_username;
				if(read_username == username){ // if it matches what we want, build and return that user object
					user_base.seekg(-MAX_USER_LEN, user_base.cur);
					int offset=user_base.tellg();
					User* user_obj=unflush_user(file_name_ss.str(), offset); // this could be null
					
					//~ mt_close(file_lock_num);
					file_lock.unlock();
					
					return user_obj;
				}				
				else {  user_base.seekg(MAX_ULINE_LEN-MAX_USER_LEN, user_base.cur);  } 
			} // if not a match, read next user
			user_base.close();
		}
		//~ mt_close(file_lock_num); // done with this file, move onto the next
		file_lock.unlock();

		file_number++;
	}
	//~ if(file_lock_num!=-1) { mt_close(file_lock_num); }

	return NULL; // if inexplicably gets here
}

User* read_id(int id_num){ 
	/*
	read_id() takes in a username id #, then finds it in the user file base and returns its user object.
	If the user does not exist in the user file base, a null pointer return takes place.
	This function allows for O(1) look up for followers/followees instead of an O(n) scan.
	The user object returned is allocated on the heap, and the function caller is responsible
	for deallocating this memory.
	*/
	stringstream file_name_ss;
	file_name_ss << FILE_PATH << "/users/u_" << id_num/USERS_PER_FILE << ".txt";

	unique_lock<mutex> uf_lock(mt_open(file_name_ss.str()));
	User* result=unflush_user(file_name_ss.str(), MAX_ULINE_LEN*(id_num % USERS_PER_FILE), true); // unsure if valid ID
	uf_lock.unlock();
	
	return result;
}

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
	file_name_ss << FILE_PATH << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
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
		stringstream sub_line_ss;
		int last_token=-1;
		for(int current_fle=0; current_fle < flwrs_in_cur_file; current_fle++){ // flwrs follow is just a line of IDs separated by spaces
			int current_id=-1;
			char * buffer = new char [MAX_ID_LEN];
			following.read(buffer,MAX_ID_LEN); 
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
				if(current_id!=last_token){ // once hit last id in line, the read repeats the last token
					followees_read++;
					last_token=current_id;
				}
				else { break; } // done with the line

				if(current_id==followee_id) { 
					stringstream triplet_ss;
					triplet_ss << file_name_ss.str() << ' ' << line_num << ' ' << current_fle;
					return triplet_ss.str(); 
				}
			} // reads by a fixed amount due to holes left by unfollow()ing
		}
		following.close();
		current_column++;
		file_name_ss.str("");
		file_name_ss << FILE_PATH << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt" ;
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
}

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
	file_name_ss << FILE_PATH << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
	int group=-1;
	if(type=='e'){ group=user_obj->followees; }
	else if(type=='r') { group=user_obj->followers; }
	else {
		cerr << "An invalid type has been passed to first_free. Exiting" << endl;
		exit(13);
	}
	int followers_read=0;
	int line_num=user_obj->id%USERS_PER_FILE;
	
	int result=access(file_name_ss.str().c_str(), F_OK);
	while( !((result < 0)&&(errno==ENOENT)) && followers_read < group){ // a gap has been found
		ifstream following;
		following.open(file_name_ss.str().c_str());
		if(!following){
			cerr << "Could not open file: " << file_name_ss.str() << endl;
			exit(14);
		}

		following.seekg(line_num*(1+FLRS_PER_LINE*MAX_ID_LEN));
		int flwes_in_cur_file=FLRS_PER_LINE;
		// no && (followers_read < group) since want to finish line and get free spot here
		for(int current_fle=0; (current_fle < flwes_in_cur_file); current_fle++){
			char * buffer = new char [MAX_ID_LEN];
			following.read(buffer,MAX_ID_LEN); 
			stringstream current_id_ss; 
			string cut_junk(buffer);
			cut_junk.resize(MAX_ID_LEN); 
			current_id_ss.str(cut_junk);
			int num=-1;
			string temp(current_id_ss.str());
			current_id_ss >> num;
			delete[] buffer;
			buffer=NULL;// no longer needed
			
			string empty(MAX_ID_LEN, MY_DELIMITER);
			if(current_id_ss.str() == empty){
				current_id_ss.str("");
				current_id_ss.clear(); // every time it outputs to a number
				current_id_ss << file_name_ss.str();
				current_id_ss << " ";
				current_id_ss << line_num;
				current_id_ss << " ";
				current_id_ss << current_fle;
				current_id_ss << " " << current_column;
				string debug(current_id_ss.str());
				return current_id_ss.str();				
			}
			else{ followers_read++; }
		}
		following.close();
		current_column++;
		file_name_ss.str("");
		file_name_ss << FILE_PATH << "/flw" << type << "s/f" << type << "_r" << user_row << "c" << current_column << ".txt";
		result=access(file_name_ss.str().c_str(), F_OK);
	}
	if( !((result < 0)&&(errno==ENOENT)) ) { // file exists
		stringstream current_id_ss;
		current_id_ss << file_name_ss.str() << ' ' << line_num << ' ' << 0 << ' ' << current_column;
		return current_id_ss.str(); // return the new file and spot to start filling
	}
	else{ 
		stringstream current_id_ss; // file doesn't exist, user must make it
		current_id_ss << -1 << ' ' << line_num << ' ' << -1 << ' ' << current_column;
		return current_id_ss.str();
	}
}

void gen_follow_func(User* home_user, User* other_user, char op, char group){
	/*
	gen_follow_func() takes in two user pointers, a character specifying the operation, and a character specifying the group.
	The op can either be "f" for follow or "u" for unfollow. The group can be "1" to indicate home_user performing the op
	to other_user (following/unfollowing them), or "2" to indicate other_user reciprocating the op from home_user (by
	being followed or unfollowed by home_user in the database).
	* The subsequent operation is then performed.
	*/
	if( !(op=='f' || op=='u') || !(group=='1' || group=='2') ){
		cerr << "Invalid options passed to gen_follow_func: " << op << "," << group << endl;
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
				file_name_ss << FILE_PATH <<  "/flw" << type << "s/f" << type << "_r" << row_val << "c" << col_file_num << ".txt";
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
			file_name_2_ss << FILE_PATH << "/users/u_" << fnum << ".txt";
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
file_name_ss << FILE_PATH << "/flw" << type << "s/f" << type << "_r" << user_row << "c"
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
		following.read(buffer,MAX_ID_LEN); 
		string temp(buffer);
		temp.resize(MAX_ID_LEN);
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
		file_name_ss << FILE_PATH << "/flw" << type << "s/f" << type << "_r" << user_row << "c"
		<< current_column << ".txt";
		result=access(file_name_ss.str().c_str(), F_OK);
		
	}
	return cult;
}

void write_woot(User* home_user, string woot, string timestamp){
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
	file_name_ss << FILE_PATH << "/woots/w_r" << row_val << "c" << col_val << ".txt";
	
	int result=access(file_name_ss.str().c_str(), F_OK);
	while( !((result<0)&&(errno==ENOENT)) ){
		// read woot by woot for empty spot, and write it and return
		fstream woot_file;
		woot_file.open(file_name_ss.str().c_str());
		if(!woot_file){
			cerr << "Could not open file: " << file_name_ss.str() << endl;
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
				file_name_ss << FILE_PATH << "/users/u_" << home_user->id/USERS_PER_FILE << ".txt";
				flush_user(file_name_ss, home_user, (home_user->id%USERS_PER_FILE)*MAX_ULINE_LEN );
				return;
			} // if a woot exits, just read the next woot
		}
		woot_file.close();
		col_val++;
		file_name_ss.str("");
		file_name_ss << FILE_PATH << "/woots/w_r" << row_val << "c" << col_val << ".txt";
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
		cerr << "Could not open file: " << file_name_ss.str() << endl;
		exit(18);
	}
	woot_file.seekp( (home_user->id%USERS_PER_FILE)*(1+MAX_WOOT_LINE_LEN)); // for EOL
	woot_file << woot_ss.str() << ' ';
	woot_file << timestamp_ss.str() << ' ';
	woot_file.close();
	
	home_user->num_woots++;
	file_name_ss.str("");
	file_name_ss.clear();
	file_name_ss << FILE_PATH << "/users/u_" << home_user->id/USERS_PER_FILE << ".txt";
	flush_user(file_name_ss, home_user, (home_user->id%USERS_PER_FILE)*MAX_ULINE_LEN);
}

vector<string>* load_woots_seq(User* home_user, int num_woots){
	/*
	load_woots_seq() takes in the logged in user object, the number of woots to load, and returns
	an array of that many woots starting from the least recent one.
	*/
	return load_woots(home_user, num_woots, 'y');
}

vector<string>* load_woots(User* home_user, int num_woots, char seq){
	/*
	load_woots() takes in the logged in user object, the number of woots to load, and returns
	an array of that many woots starting from the most recent one.
	*/
	int row_val=home_user->id/USERS_PER_FILE;
	int col_val=-1;
	if(seq=='y') { col_val=0; }
	else if(seq=='n') { col_val=((home_user->num_woots -1)/WOOTS_PER_LINE); } // change 1

	stringstream file_name_ss;
	file_name_ss << FILE_PATH << "/woots/w_r" << row_val << "c" << col_val << ".txt";
	int woots_read=0;
	vector<string>* woots=new vector<string>;	
	
	int result=access(file_name_ss.str().c_str(), F_OK);
	while( !( (result<0) && (errno=ENOENT) ) && woots_read < num_woots ){
		// read woot by woot for empty spot, and write it and return
		ifstream woot_file;
		woot_file.open(file_name_ss.str().c_str());
		if(!woot_file){
			cerr << "Could not open file: " << file_name_ss.str() << endl;
			exit(18);
		}
		int next_line=-1;
		if(seq=='y') { next_line=0; }
		else if(seq=='n') { next_line=1; }
		woot_file.seekg( (next_line+home_user->id%USERS_PER_FILE)*(1+MAX_WOOT_LINE_LEN)-1 ); //new line character one time

		// goes to the line AFTER yours, so you can read the timestamp and the last woot in first
		if(seq=='n'){
			if( woot_file.tellg()==-1){
				woot_file.clear();
				woot_file.seekg(0, woot_file.end);
			}
			woot_file.seekg(-(MAX_WOOT_TMSTP+1+MAX_WOOT_LEN+1), woot_file.cur);
		}
		for(int index=0; index < WOOTS_PER_LINE && woots_read < num_woots; index++){			
			char* buffer = new char[MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1];
			woot_file.read(buffer, MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			string cut_junk(buffer);
			cut_junk.resize(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);
			
			stringstream read_woot_ss;
			read_woot_ss.str(cut_junk);
			string empty(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1, MY_DELIMITER);
			// don't load any empty woots, but possible to have  1 woot in otherwise empty line, keep extracting to end
			if(seq=='n'){ woot_file.seekg(-(2)*(MAX_WOOT_TMSTP+1+MAX_WOOT_LEN+1), woot_file.cur); }
			if(read_woot_ss.str() != empty) { 
				woots_read++;
				woots->push_back(read_woot_ss.str()); 
			}
			delete[] buffer;		
		}	
		woot_file.close();
		if(seq=='y') { col_val++; }
		else if(seq=='n') { col_val--; }
		file_name_ss.str("");
		file_name_ss << FILE_PATH << "/woots/w_r" << row_val << "c" << col_val << ".txt";
		result=access(file_name_ss.str().c_str(), F_OK);
	}
	return woots;
}
