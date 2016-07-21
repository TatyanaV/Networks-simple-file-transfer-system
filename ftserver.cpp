/*
Name:
                Tatyana Vlaskin and see refferences
email:
                vlaskint@oregonstate.edu
Class:
                CS372
Assignment:
                Project #2
Program:
                ftserver.cpp

Description:	This program should be placed in the directory, lets say server. The name of
                the directory is NOT hard coded, so it can be any name. Additionally to this program,
                the directory should have some .TXT files. After the program is compiled, the program takes
                the port # as a command line argument, creates welcoming socket and begins listening
                for connection requests on the specified port. The client will either send a request
                to get a list of the files located in the directory or will request to transfer one of the .TXT
                files from the directory. After the client received the requested information connection is
                closed for the client. ftserver continue waiting for the next client until terminated by a supervisor (SIGINT).
REFFERENCES: available upon request

*/


#include <signal.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

int cnrtlPNum=0;
/*
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};
*/
//http://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
struct sockaddr_in hostAddress;
int controlSocket, dataSocket; //control and data socker, needed per assignment description
int initial_Socket=0;//inital socket, when the server is started
int data_Port=0;//date port
char commandFromClient[5];//command that will be received from the client



// Used to reset commandFromClient, old command will be essased - set to 0
void reset(char *deleteContents){
	memset(deleteContents, 0, sizeof(deleteContents));
}



/*
    Function used to connect to the data_Port.
    If no error occurs, function returns a descriptor referencing the new socket.
    Otherwise, a value of INVALID_SOCKET is returned and function is existed with exit(1)
    This function used the following structure and the following functions:
    1. struct sockaddr_in
    2.	int getpeername(_In_SOCKET s, Out_struct sockaddr *name, _Inout_ int *namelen);
    3.htons(data_Port);
    4. socket(AF_INET, SOCK_STREAM, 0) -- for TCP connection
    5.int connect(_In_ SOCKET  s, _In_ const struct sockaddr *name, _In_ int namelen);

 */
int createSocket(){
	struct sockaddr_in addrs;
	int connectChecker = -1;
	int ipChecker = 0;
	int dataCon;
	// Found out about getpeername from
	// http://www.retran.com/beej/getpeernameman.html
	unsigned int addrsSize = sizeof(addrs);
	//getpeername function retrieves the address of the peer to which a socket is connected
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ms738533%28v=vs.85%29.aspx
	/*
	int getpeername(
        _In_    SOCKET          s,
        _Out_   struct sockaddr *name,
        _Inout_ int             *namelen
    );

    Parameters: s [in]   A descriptor identifying a connected socket.
                name [out]     The SOCKADDR structure that receives the address of the peer.
                namelen [in, out]    A pointer to the size, in bytes, of the name parameter.

*/
	ipChecker = getpeername(controlSocket, (struct sockaddr *) &addrs, &addrsSize);
	if(ipChecker == -1)	{
		cout <<"ERROR: in receiving Client's IP" << endl;
		close(controlSocket);
		exit(1);
	}

    //set port number
    //converts the unsigned short integer netshort from network byte order to host byte order
    // convert our byte orderings to "big-endian" before receiving them
	//see http://beej.us/guide/bgnet/output/html/multipage/htonsman.html
	// Used to connect to new data_Port.
	addrs.sin_port = htons(data_Port);

	// Used to create a new socket for the data to transfer over
    /*
    The socket() call creates an unnamed socket and returns a file descriptor to the calling process.
    The function prototype is as follows:  int socket(int domain, int type, int protocol);
    where domain specifies the address domain, type specifies the socket type, and the third
    argument is the protocol (this is usually set to 0 to automatically select the appropriate default).
    Here is a typical socket() call:    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    This establishes a socket in the Internet domain, and configures it for stream-oriented
    communication using the default TCP protocol*/
	dataCon = socket(AF_INET, SOCK_STREAM, 0);
	if(dataCon == -1)	{
		cout << "ERROR: in creating a socket "  << endl;
		exit(1);
	}

	// Used to connect to the Client
	while(connectChecker == -1)	{
	    //https://msdn.microsoft.com/en-us/library/windows/desktop/ms737625%28v=vs.85%29.aspx
	    //The connect function establishes a connection to a specified socket
	    //If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code
	    /*

            int connect(
              _In_ SOCKET                s,
              _In_ const struct sockaddr *name,
              _In_ int                   namelen
            );

            Parameters: s [in]    A descriptor identifying an unconnected socket.
                        name [in] A pointer to the sockaddr structure to which the connection should be established.
                        namelen [in]     The length, in bytes, of the sockaddr structure pointed to by the name parameter.

	    */
		connectChecker = connect(dataCon, (struct sockaddr *) &addrs, sizeof(addrs));
	}

	return dataCon;
}

/*
    Function that gets the port number.
    Function uses the following functons:
    1.ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    2.memcpy(temp, commandFromClient, sizeof(temp));
    3.nt dp = atoi(temp);
    Function received infromation about the port number, copies contents from the source to the destination,and
    converts information from the string format to the int format.
    Function returns port number.

*/
int determineIncommingPortNumber(){
	reset(commandFromClient);
    //receives messages from a connected socket
    //no messages are available at the socket, the receive calls wait for a message to arrive
    //ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    /*
    The recv() function takes the following arguments:
        socket    Specifies the socket file descriptor.
        buffer    Points to a buffer where the message should be stored.
        length    Specifies the length in bytes of the buffer pointed to by the buffer argument.
        flags     Specifies the type of message reception.
    */
    //please note that the size of the buffer is 500
	recv(controlSocket, commandFromClient, sizeof(commandFromClient), 0);
	char temp[500];
	//void * memcpy ( void * destination, const void * source, size_t num );
	//Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination.
	//destination is returned
	//http://www.cplusplus.com/reference/cstring/memcpy/
	memcpy(temp, commandFromClient, sizeof(temp));
	//int atoi (const char * str);
	//Parses the C-string str interpreting its content as an integral number, which is returned as a value of type int
	//http://www.cplusplus.com/reference/cstdlib/atoi/
	int dp = atoi(temp);
	return dp;
}

/*
    Function to list contents of the server directory.
    Uses the following structure:
	struct dirent {
               ino_t          d_ino;       //inode number
               off_t          d_off;       // offset to the next dirent
               unsigned short d_reclen;    // length of this record
               unsigned char  d_type;      //type of file; not supportednby all file system types
               char           d_name[256]; // filename
    //http://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
	//http://stackoverflow.com/questions/18428767/how-can-i-get-all-file-name-in-a-directory-without-getting-and-in-c
	//http://www.cplusplus.com/forum/beginner/142027/
	// http://www.cplusplus.com/forum/general/100714/
	//http://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
	// http://pubs.opengroup.org/onlinepubs/007908775/xsh/dirent.h.html
    //http://man7.org/linux/man-pages/man2/send.2.html
    //http://beej.us/guide/bgnet/output/html/multipage/sendman.html

*/
void listDirectory(){
	string holder; // string to store names of the files in the directory
	DIR * dir;
	struct dirent * fname;
	cout << "***LISTING DIRECTORY CONTENTES. SENDING ON data_Port: " << data_Port << " *** \n" << endl;
	//get a data socket, this infomratin is nessessary to send the list to the client
	dataSocket = createSocket();
    if((dir = opendir("./")) == NULL)	{
		cout << "ERROR: in opening a directory" << endl;
		exit(10);
	}
	while((fname = readdir(dir)) != NULL){
		if((strcmp(fname->d_name, "..") != 0) && (strcmp(fname->d_name, ".") != 0)){
		    //save the names of the files into a string
		    //add a space to separate file names
			holder = fname->d_name;
			holder += " ";
			//conver string into the char
			const char* buffer = holder.c_str();
            //ssize_t send(int sockfd, const void *buf, size_t len, int flags);
            //http://man7.org/linux/man-pages/man2/send.2.html
            //the list of all files will be send to the client
			send(dataSocket, buffer, strlen(buffer), 0);
		}
	}
	reset(commandFromClient);
	close(controlSocket);
	close(dataSocket);
}

/*
    Function that transferes TEXT files from server to client
    //http://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
    //https://github.com/franciscotufro/simplex-framework/blob/d679a10e068425705d36d4477efdec1c66d4d454/modules/support/src/File.cpp
    // http://www.cplusplus.com/forum/general/100714/
    //http://www.linuxhowtos.org/C_C++/socket.htm
    //http://beej.us/guide/bgc/output/html/multipage/fseek.html
     //http://stackoverflow.com/questions/4365166/segmentation-fault-on-fread-help


*/
void getFile(){
	int passTest;
	char fileName[500];
	bool skipIt = false;
	// Sets the first num bytes of the block of memory pointed by ptr to the specified value (interpreted as an unsigned char)
	//everything is filled with 0
	memset(fileName, 0, sizeof(fileName));
	// receive the name of the file that the client wants
	recv(controlSocket, fileName, sizeof(fileName), 0);
	cout << "**FILE" << fileName << " IS REQUESTED. SENDING ON data_Port: " << data_Port << "**\n" << endl;
	// get the data socket.
	dataSocket = createSocket();
    //check that the file was opend without errors
	passTest = open(fileName, O_RDONLY);
	//if file does not exist, send that inforamtion to the client
	if(passTest == -1){
		const char* errorMsg = "ERROR: FILE DOES NOT EXIST.\n";
		//send the error message to the client
		send(dataSocket, errorMsg, strlen(errorMsg), 0);
		close(dataSocket);
		cout << errorMsg << endl;
		skipIt = true;
	}
    //if file exists
    if(skipIt == false)	{
		FILE *f;
		char *out_buffer;
		f = fopen(fileName, "rb");
		fseek(f, 0, SEEK_END);// find the end of the file (beware the 2Gb limitation, though)
        long fsize = ftell(f); //get the size of the file
        fseek(f, 0, SEEK_SET);//go back to the 1st element of the file
        out_buffer = (char*)malloc(fsize + 1);//allocate enough memory to the buffer
        //size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
        //Reads an array of count elements, each one with a size of size bytes, from the stream and stores them in the block of memory specified by pt
        fread(out_buffer, fsize, 1, f);//read contenst of the file to the buffer
        out_buffer [fsize] = '\0';/// THE PROBLEM IS HERE, I AM NOT SURE WHERE EXACTLY TO PUT A NULL TERMINATOR
		write(dataSocket, out_buffer, strlen(out_buffer));//send file to the client
		fclose(f);
		free(out_buffer);
	}
	reset(commandFromClient);
	close(dataSocket);
}

/*
    Function used to handle kill signals

*/
void sigintHandler(int pram){
	close(controlSocket);
	close(dataSocket);
	cout << "\n ** SIGNAL RECEIVED.  SOCKETS THAT USED" << cnrtlPNum << " AND " << data_Port << " CLOSED. SERVER IS SHUT DOWN. **\n" <<  endl;
	exit(1);
}


/*Function to accept an incomming connection and read messages from the client.
based on the command that was recieved from the client, the following outcomes are possibele
1.send the client contents of the directory
2. send the client a file
3. sned a client an error message in case invalid command has arrive
4. send the client FILE DOES NOT EXIST in case a file was requested and it does not exist in the directory
 */

void communicationEstablihsed(){
	// reset the command
	reset(commandFromClient);
	while(1){
        // AAccept a connection with the accept() system call.
		//This call typically blocks until a client connects with the server.
		//http://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
		controlSocket = accept(initial_Socket,(struct sockaddr*) NULL, NULL);
		cout << "** CONNECTION ESTABLISHED. USING PORT: " << cnrtlPNum << " **" << endl;
		// get the data_Port
		data_Port = determineIncommingPortNumber();
		reset(commandFromClient);
        //receives messages from a connected socket
        //no messages are available at the socket, the receive calls wait for a message to arrive
        //ssize_t recv(int sockfd, void *buf, size_t len, int flags);
        /*
        The recv() function takes the following arguments:
            socket    Specifies the socket file descriptor.
            buffer    Points to a buffer where the message should be stored.
            length    Specifies the length in bytes of the buffer pointed to by the buffer argument.
            flags     Specifies the type of message reception.
        */
		recv(controlSocket, commandFromClient, sizeof(commandFromClient), 0);
		//if command had -l, we need to list directories
		if(strcmp(commandFromClient, "-l") == 0){
				listDirectory();
        }
        //if command had -g, we need to send  a file
        else if(strcmp(commandFromClient, "-g") == 0){
				getFile();
        }
        //if command is invalid
       else{
            dataSocket = createSocket();
            const char* errorMsg2 = "ERROR: INVALID COMMAND WAS ENTERED.\n";
            //send the error message to the client
            send(dataSocket, errorMsg2, strlen(errorMsg2), 0);
            close(dataSocket);
            cout << errorMsg2 << endl;

        }

		close(controlSocket);
		close(dataSocket);
	}
}


int main (int argc, char *argv[]){
    //int portNumberCheck; //to validate the correct entry was made for port number
    // chartserce on host A waits fon a port[specified by command-line] for a client request
    //using command line, 2 arguments are expected:
    //1.chartserve
    //2. <port # on which to listen for connection requests, must be in the range [1024, 65535]
    // if incorrect number of arguments are entered display an error
  	if(argc!=2){
		cerr << "ERROR: No port specified. Type: ./ftserver.cpp <port number>, port must be in the range [1024-65535] " << endl;
		exit(1);
	}
    //per the following refferences: port # in the range 1024-65535 can be freely used
	//page 193 Computer Networkinng top down approach
	//https://technet.microsoft.com/en-us/library/gg398833.aspx
	//https://books.google.com/books?id=nDnSBQAAQBAJ&pg=PA389&lpg=PA389&dq=port+numbers+used+for+servers+1024-65535+chats&source=bl&ots=P50QoqyGGC&sig=7zbmQcn6m13xumh_aJl3bHB-c5E&hl=en&sa=X&ved=0CEoQ6AEwB2oVChMIudbevKLuyAIVRM9jCh2bEwcD#v=onepage&q=port%20numbers%20used%20for%20servers%201024-65535%20chats&f=false
	// places the port number in the var
	cnrtlPNum = atoi(argv[1]);
	if (cnrtlPNum < 1024 || cnrtlPNum > 65535) {
		cerr << "ERROR: Port number must be in the range [1024 - 65535]" << endl;
		exit(1);
	}
	//Used to process kill signals
	signal(SIGINT, sigintHandler);
    /*
    The socket() call creates an unnamed socket and returns a file descriptor to the calling process.
    The function prototype is as follows:  int socket(int domain, int type, int protocol);
    where domain specifies the address domain, type specifies the socket type, and the third
    argument is the protocol (this is usually set to 0 to automatically select the appropriate default).
    Here is a typical socket() call:    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    This establishes a socket in the Internet domain, and configures it for stream-oriented
    communication using the default TCP protocol*/
	initial_Socket = socket(AF_INET, SOCK_STREAM, 0);
    //fill memory with a constant byte
	//void *memset(void *s, int c, size_t n);
	// The memset() function fills the first n bytes of the memory area pointed to by s with the constant byte c
	//clearning the buffer by filling it 0
	memset(&hostAddress, '0', sizeof(hostAddress));
    //the socket will be bound to all local interfaces
    // Scince this is a server, the IP address has been set to INADDR_ANY,
    //specifying that connections will be accepted from any other host.
	hostAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //set port number
    //converts the unsigned short integer netshort from network byte order to host byte order
    // convert our byte orderings to "big-endian" before receiving them
	//see http://beej.us/guide/bgnet/output/html/multipage/htonsman.html
	hostAddress.sin_port = htons(cnrtlPNum);
    // Configure the server address
	//http://man7.org/linux/man-pages/man7/ip.7.html
	// address family, network is IPv4. According to the book, page 160 we do not
	//need to worry about understanding this right now, we will cover IPv4 in chapter 4
	hostAddress.sin_family = AF_INET;
    // Associate server-side socket with listening port.
	//binding the server socekt [that is, assigning] the port number to the servers socket
	bind(initial_Socket, (struct sockaddr*)&hostAddress, sizeof(hostAddress));
	cout << "**WAITING FOR THE CLIENT**\n" << endl;
    // Listen for connections
    //http://linux.die.net/man/2/listen
    //int listen(int sockfd, int backlog);
    // backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow
    //for more information see http://linux.die.net/man/2/listen
	listen(initial_Socket, 10);
	//accept an incomming connection.
	communicationEstablihsed();

	return 0;
}
























































