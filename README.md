# Networks-simple-file-transfer-system
#Program Requirements:
Design and implement a simple file transfer system, i.e., create a file transfer server and a file transfer client. Write the ftserver and the ftclient programs. The final version of your programs must accomplish the following tasks: </br>
1. ftserver starts on Host A, and validates command-line parameters (SERVER_PORT).</br>
2. ftserver waits on PORTNUM for a client request.</br>
3. ftclient starts on Host B, and validates any pertinent command-line parameters. (SERVER_HOST, SERVER_PORT, COMMAND, FILENAME, DATA_PORT, etc…)</br>
4. ftserver and ftclient establish a TCP control connection on <SERVER_PORT>. (For the remainder of this description, call this connection P)</br>
5. ftserver waits on connection P for ftclient to send a command.</br>
6. ftclient sends a command (-l (list) or -g <FILENAME> (get)) on connection P.</br>
7. ftserver receives command on connection P.</br>
If ftclient sent an invalid command</br>
• ftserver sends an error message to ftclient on connection P, and ftclient displays the message on-screen.
otherwise</br>
• ftserver initiates a TCP data connection with ftclient on <DATA_PORT>. (Call this connection Q)</br>
• If ftclient has sent the -l command, ftserver sends its directory to ftclient on connection Q, and ftclient displays the directory on-screen.</br>
• If ftclient has sent -g <FILENAME>, ftserver validates FILENAME, and either</br>
- sends the contents of FILENAME on connection Q. ftclient saves the file in the current default directory (handling "duplicate file name" error if necessary), and displays a "transfer complete" message on-screen</br>
or</br>
- sends an appropriate error message (“File not found”, etc.) to ftclient on connection P, and ftclient displays the message on-screen.</br>
• ftserver closes connection Q (don’t leave open sockets!).</br>
8. ftclient closes connection P (don’t leave open sockets!) and terminates.</br>
9. ftserver repeats from 2 (above) until terminated by a supervisor (SIGINT).</br>

#SERVER SIDE:

1.  Take ftserver.cpp program and place it in some directory. Lets call its called server. It does not matter which
directory it will be placed, but it has to be different directory from the ftclient.py. Also place few .txt files 
in that directory</br>
2. Open a terminal that will be your server terminal.</br>
3. Enter a directory where you placed ftserver.cpp by typing cd [name of directory]</br>
4. Type: g++ ftserver.cpp -o ftserver to compile the program</br>
5. After the program is compiled.</br>
a. Type: hostname –i</br>
b. An IP address will be displayed, write down that number</br>
6.Type ./ftserver port # </br>
a. Please note that only ports in the range 1024 -65535 will be accepted. Those ports are not used for most of the applications and can be freely  
used. </br>
b. If you type a number not in the range 1024 -65535, you will be asked a reenter a port number</br>
c. If you forget to type port number, you will be asked to enter a port number</br>
d. NOTE: I have NOT tested all those ports, I was using port in the 40000 range during testing</br>

7. You are done with the server side for NOW. The server is waiting for clients requests </br>
8. ftserver continue waiting for the client until terminated by a supervisor (SIGINT).</br>

#CLIENT SIDE:
1. Place ftclient.py file in some directory, lets say client. The name of the directory does not matter, but it has to be 
a different one from the server directory</br>
2. Open 2nd terminal</br>
3.Enter the directory where you placed ftclient.py file by typing cd [name of the directory]</br>
3.Type commands:</br>

A. python ftclient.py IP # OF THE SERVER-see above PORT # OF THE SERVER-see above -l PORT NUMBER FOR THE DATA- has to be different from the Server Port</br>
This will list contents of the server directory</br>
B. python ftclient.py <IP # OF THE SERVER-see above> PORT # OF THE SERVER-see above -g [NAME OF THE FILE THAT YOU WANT TO TRANSFER] PORT NUMBER FOR THE DATA- has to be different from the Server Port</br>
This will transfer the file from the server directory to the client directory if the file exists. Please note that LARGEST SIZE THAT CAN BE TRANSTERED IS: 262144 BYTES. That can be change in the ftclient.py file by changing the bytes number in the following line: buffer = cmndSocket1.recv(262144)</br>
C.python ftclient.py <IP # OF THE SERVER-see above> <PORT # OF THE SERVER-see above> -g [NAME OF THE FILE THAT DOES NOT EXIST IN THE SERVER DIRECTORY] PORT NUMBER FOR THE DATA- has to be different from the Server Port </br>
The server will send an error message saying that the file was not found</br>
D.Type anything that has less than 5 and 6 arguments, you will be asked to reenter a command</br>
E. python ftclient.py <IP # OF THE SERVER-see above> <PORT # OF THE SERVER-see above> -k PORT NUMBER FOR THE DATA- has to be different from the Server Port </br>
type any command , in this case I typed -k and make sure that are 5 arguments in the command line. The command will be sent to the server, the server will flag the command as an invalid and will send an invalid command message to the client </br>
F. python ftclient.py <IP # OF THE SERVER-see above> PORT # OF THE SERVER-see above -K [NAME OF THE FILE] PORT NUMBER FOR THE DATA- has to be different from the Server Port </br>
type any command , in this case I typed -k and [file name] and make sure that are 6 arguments in the command line. The command will be sent to the server, the server will flag the command as an invalid and will send an invalid command message to the client</br>
G. If you request to transfer a file that is already located in the client directory, you will be asked if you want to overwrite the file or not. You can either say yes or no - this was taken from one of the references provided in the code</br>


#***********PLEASE NOTE: I had issues with the random charactes appending to the end of the file when it was transfered.********************
Following suggestions on the following blog: http://stackoverflow.com/questions/1424763/appending-data-to-a-null-character-in-character-array-to-send-data-through-socke
However, according to that blog I should have done something like this:
out_buffer = (char*)malloc(fsize + 2);
out_buffer [fsize+1] = '\0';

But that did not work, so I had to chage that to:
out_buffer = (char*)malloc(fsize + 1);
out_buffer [fsize] = '\0';
It seems to work, but in case you've notice that some of the files that are transfered have random charactes at the end, it has something to do with NULL character at the end

#**********PLEASE NOTE: THE LARGEST SIZE THAT CAN BE TRANSTERED IS: 262144 BYTES. **************************************************
That can be change in the ftclient.py file by changing the bytes number in the following line: buffer = cmndSocket1.recv(262144)
The recommendation is to multiply bytes numbers by 2 to get to a large number

#****************PLEASE NOTE: THERE IS NO ERROR CHECK TO INCORRECTLY ENTERED IP OR PORT #S********************************************
program will have erratically if invalid entry is make for port # or IP#



#MOCK RUN - THOUGH THE PROGRAM 
SERVER SIDE:
[vlaskint@flip3 ~]$ hostname -i
128.193.54.10
[vlaskint@flip3 ~]$ cd server
[vlaskint@flip3 ~/server]$ g++ ftserver.cpp -o ftserver
[vlaskint@flip3 ~/server]$ ./ftserver 1023
ERROR: Port number must be in the range [1024 - 65535]
[vlaskint@flip3 ~/server]$ ./ftserver 65536
ERROR: Port number must be in the range [1024 - 65535]
[vlaskint@flip3 ~/server]$ ./ftserver 40010
**WAITING FOR THE CLIENT**

** CONNECTION ESTABLISHED. USING PORT: 40010 **
ERROR: INVALID COMMAND WAS ENTERED.

** CONNECTION ESTABLISHED. USING PORT: 40010 **
***LISTING DIRECTORY CONTENTES. SENDING ON data_Port: 40011 ***

** CONNECTION ESTABLISHED. USING PORT: 40010 **
**FILE1.txt IS REQUESTED. SENDING ON data_Port: 40011**

** CONNECTION ESTABLISHED. USING PORT: 40010 **
**FILE2.txt IS REQUESTED. SENDING ON data_Port: 40011**

** CONNECTION ESTABLISHED. USING PORT: 40010 **
**FILE2222.txt IS REQUESTED. SENDING ON data_Port: 40011**

ERROR: FILE DOES NOT EXIST.

** CONNECTION ESTABLISHED. USING PORT: 40010 **
***LISTING DIRECTORY CONTENTES. SENDING ON data_Port: 40011 ***


^C
 ** SIGNAL RECEIVED.  SOCKETS THAT USED40010 AND 40011 CLOSED. SERVER IS SHUT DOWN. **




CLIENT SIDE:
[vlaskint@flip3 ~]$ cd client
[vlaskint@flip3 ~/client]$ python ftclient.py 128.193.54.10 40010 -i 40011

CONTACTING SERVER.
CONNECTED TO THE SERVER.

UNKNOWN COMMAND WAS SENT.

ERROR: INVALID COMMAND WAS ENTERED.

SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.

[vlaskint@flip3 ~/client]$ python ftclient.py 128.193.54.10 40010 -l 40011

CONTACTING SERVER.
CONNECTED TO THE SERVER.

THE COLLOWING COMMAND WAS SENT: -l

LIST SERVER DIRECTORY CONTENTS: 1.txt 2.txt 3.txt 4.txt 7.txt 5.txt 6.txt ftserv                                                                                        er 1.pdf ftserver.cpp
SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.

[vlaskint@flip3 ~/client]$ python ftclient.py 128.193.54.10 40010 -g 1.txt 40011                                                                                        

FILE ALREADY EXISTS WITH THAT NAME IN YOUR DIRECTORY.
Do you want to overwrite the file? [YES, NO, Y, N]yes
FILE WILL BE OVERWRITTEN.

CONTACTING SERVER.
CONNECTED TO THE SERVER.

THE COLLOWING COMMAND WAS SENT: -g [fileName]

FILE RECEIVED: 1.txt
SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.

[vlaskint@flip3 ~/client]$ ls
1.txt  ftclient.py
[vlaskint@flip3 ~/client]$ python ftclient.py 128.193.54.10 40010 -g 2.txt 40011                                                                                        

CONTACTING SERVER.
CONNECTED TO THE SERVER.

THE COLLOWING COMMAND WAS SENT: -g [fileName]

FILE RECEIVED: 2.txt
SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.

[vlaskint@flip3 ~/client]$ ls
1.txt  2.txt  ftclient.py
[vlaskint@flip3 ~/client]$ python ftclient.py 128.193.54.10 40010 -g 2222.txt 40                                                                                        011

CONTACTING SERVER.
CONNECTED TO THE SERVER.

THE COLLOWING COMMAND WAS SENT: -g [fileName]

FILE "2222.TXT" NOT FOUND.

SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.

[vlaskint@flip3 ~/client]$ python ftclient.py 128.193.54.10 40010 -g 40011                                                                                              ERROR INVALID # OF ARGUMENTS ENTERED.

TYPE: <Server IP> [<control port> -l OR -g [file name]] <dataPort>

[vlaskint@flip3 ~/client]$ python ftclient.py 128.193.54.10 40010 -l 40011                                                                                              
CONTACTING SERVER.
CONNECTED TO THE SERVER.

THE COLLOWING COMMAND WAS SENT: -l

LIST SERVER DIRECTORY CONTENTS: 1.txt 2.txt 3.txt 4.txt 7.txt 5.txt 6.txt ftserv                                                                                        er 1.pdf ftserver.cpp
SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.

[vlaskint@flip3 ~/client]$

