/*
  From Stevens Unix Network Programming, vol 1.
  Minor modifications by John Sterling
 */

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


  struct Woot{ 
    char message[100];
    char timestamp[22];
};

struct User{
    char username[24];
    char password[32];
    char id[10];
    char full_name[51];
    char age[3];
    char email[100];
    char followers[10];
    char followees[10];
    char free_bit;
    char num_woots[10];
};

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
    char MY_DELIMITER='.';
    int WOOTS_PER_LINE=10;     int MAX_WOOT_LEN=100;      int MAX_WOOT_TMSTP=22;
    int MAX_WOOT_LINE=(10)*(100+1+22+1);
   // int MAX_WOOT_LINE_LEN=WOOTS_PER_LINE*(MAX_WOOT_LEN+1+MAX_WOOT_TMSTP+1);



int main(int argc, char **argv) {
    int	listenfd, connfd;  // Unix file descriptors. its just an int
    struct sockaddr_in	servaddr;  // Note C use of struct


   // need to decide format of what we're going to send
 // minimal fixed # of bytes for command, then for operands from there, and a max total buffer size
    char buff[MAXLINE];
    char cmd[6];

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
    servaddr.sin_port        = htons(PORT_NUM);	// daytime server

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
    int read_well=read(connfd, cmd, sizeof(cmd));
    if(read_well==sizeof(cmd)){

        if(strncmp(cmd,"follow", sizeof(cmd))==0){ fprintf(stdout,"follow yes<\r\n\r\n");}
        else { fprintf(stdout,":((\r\n\r\n"); }
 //       snprintf(cmd, sizeof(cmd), "%.6s\r\n", "msgG");
            write(connfd, cmd, strlen(cmd));
        }
        else{
        // printf(sizeof(buff));
        }


        // We had a connection.  Do whatever our task is.
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", "testomg");
        int len = strlen(buff);

        if (len != write(connfd, buff, strlen(buff))) {
            perror("write to connection failed");
        }

        // 6. Close the connection with the current client and go back
        //    for another.
        close(connfd);
    }
}



struct User* create_new_user(struct User* user_obj){    
    /*
    create_new_user() takes in a user object assembled from the registration page and fully
    integrates that user into the system. This is done by finding it a free user ID and by
    writing the user's attributes to the first available spot in the user file base.
    * The user object is then returned for convenience.
    */
    int user_id=-1;
    int current_line=0;
//there is no NEW operator
    struct User* user_obj = new struct User;
//    if(!file_exists("fids/fids.txt")){ create_file("fids", 0, 0); }
    //else{
        int fids=fopen("fids/fids.txt", "r+") or die("Could not open file");
        int current_free=(int)trim(fread(fids,strlen(USERS_PER_FILE)),".");  
        while( current_free == 0 ) { // find the first user file # with free slots for new users
            if(!feof(fids)){ fseek(fids, 1, SEEK_CUR); } // move to the next line
            else { break; }
            current_line++;
            current_free=(int)trim(fread(fids,strlen(USERS_PER_FILE)),"."); // read the next fids values
        }
        fclose(fids);
    //} // current_line is now which user file you want
    char* file_name="users/u_".current_line.".txt";

    if(!file_exists(file_name)){  // make the file, flush the user, update fids
        user_id=current_line*USERS_PER_FILE;
        create_file("users", current_line, 0);     
        int u_file=fopen(file_name, "r+") or die("Could not open file: ".file_name);
        user_obj->id=user_id;
        user_obj->followers=0;
        user_obj->followees=0;
        user_obj->num_woots=0;
        user_obj->free_bit="n";
        flush_user(u_file, user_obj, 0); // write the user and their attributes to the file
        fclose(u_file);
        fids_decrease((int)(user_obj->id/USERS_PER_FILE));
        return user_obj;
    }
    else { // if the file exists, it has free space for new users
        u_file=fopen(file_name, "r+") or die("Could not open file: ".file_name);
        int current_id=0;
        int bit_pos=MAX_ULINE_LEN-MAX_NUM_WOOTS-1-1;
        fseek(u_file, bit_pos);
        
        while(current_id<100*USERS_PER_FILE){ // scan until you find the first free spot
            current_bit=fread(u_file, 1);
            if(current_bit=="."){
                fseek(u_file, -(bit_pos+1));
                user_obj->id=current_id+(USERS_PER_FILE*current_line);
                user_obj->followers=0;
                user_obj->followees=0;
                user_obj->num_woots=0;
                user_obj->free_bit="n";
                flush_user(u_file, user_obj, (current_id)*MAX_ULINE_LEN);
                fclose(u_file);
                fids_decrease((atoi((user_obj->id))/USERS_PER_FILE));
                return user_obj;
            }
            fseek(u_file, MAX_ULINE_LEN-1, SEEK_CUR);
            current_id+=1;
        }
    }
    return 0;
    // THIS RETURNS A POINTER TO A NEWLY ALLOCATED ITEM, YOU NEED TO DELETE IT
}
