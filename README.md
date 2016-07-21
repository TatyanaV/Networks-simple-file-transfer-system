# Networks-simple-file-transfer-system
#Program Requirements:
Design and implement a simple file transfer system, i.e., create a file transfer server and a file transfer client. Write the ftserver and the ftclient programs. The final version of your programs must accomplish the following tasks: </br>
1. ftserver starts on Host A, and validates command-line parameters (SERVER_PORT).</br>
2. ftserver waits on <PORTNUM> for a client request.</br>
3. ftclient starts on Host B, and validates any pertinent command-line parameters. (<SERVER_HOST>, <SERVER_PORT>, <COMMAND>, <FILENAME>, <DATA_PORT>, etc…)</br>
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
