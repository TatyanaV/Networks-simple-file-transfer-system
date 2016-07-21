# 	Name:  Tatyana Vlaskin and see refferences
# 	Program: ftclient.py
#	Description:
# 	This program performs the client side of a file trasfer program. Server is waiting for the clients. The
# 	client side of the program starts when the client indicates the server IP address and the port number
#   of the server it would like to connect as well as the command that they want to perform. Two commands are
#	allowed <-l> to list files in the directory of the server and <-q> <filename> to get a file from the server.
#	The client needs to enter command line that is 5 or 6 elements long, if any other length is entered, an error
#	will be displayed. If the client enteres either 5 or 6 elements long, but will use other command than <-l> or
#	<-g> the command will be send to the server, the server will determine that the command is invalid and will
#	send an error message to the client. If the client requests a file that does not exist in the server directory,
#	the server will send an error message to the client. After EACH command the client connection is closed and the server
#	connection stays open wayting for other clients.
#	Reference:available upon request

#

import socket
import time
import os.path
import getopt
import sys

#Function to check that the user eneted correct number of argumers
def argumentsCheck():
	#only 5 or 6 arguments are allowed to be entered
	if len(sys.argv) != 5 and len(sys.argv) !=6:
		#print (len(sys.argv))
		print "ERROR INVALID # OF ARGUMENTS ENTERED.\n"
		print "TYPE: <Server IP> [<control port> -l OR -g [file name]] <dataPort>\n"
		sys.exit()
	# if the entered command is -g, but a file name was not indicated
	if sys.argv[3] == "-g" and len(sys.argv) != 6:
		print "ERROR INVALID # OF ARGUMENTS ENTERED.\n"
		print "TYPE: <Server IP> [<control port> -l OR -g [file name]] <dataPort>\n"
		sys.exit()
	#if the entered command is -l, but not enough arguments were entered
	if sys.argv[3] == "-l" and len(sys.argv) != 5:
		print "ERROR INVALID # OF ARGUMENTS ENTERED.\n"
		print "TYPE: <Server IP> [<control port> -l OR -g [file name]] <dataPort>\n"
		sys.exit()
	#if entered command is -g and file is entered
	if len(sys.argv) == 6 and sys.argv[3] == "-g":
		#if a file with the same name already exists in the client directory
		if os.path.isfile(sys.argv[4]):
			#let the user know that the file already exists
			print "\nFILE ALREADY EXISTS WITH THAT NAME IN YOUR DIRECTORY."
			#ask the user if they want to overwire the file
			sys.stdout.write('Do you want to overwrite the file? [YES, NO, Y, N]')
			decision = raw_input("")
			while 1:
				#checked the entry of the client
				if decision.lower() == "yes" or decision.lower() == "y":
					print "FILE WILL BE OVERWRITTEN."
					break
				elif decision.lower() == "no" or decision.lower() == "n":
					print "FILE WILL NOT BE OVERWRITTEN."
					print "PROGRAM EXITED."
					sys.exit()
				else:
					sys.stdout.write('ONLY THE FOLLOWING ENTRIES ARE ALLOWED: [YES, NO, Y, N] ')
					decision = raw_input("")

#Function is called  if the client entered -l command and 5 arguments were enetered in the command line
# This function will send -l and datasocket to the server. this will get the contentes of the directory of the server
def listCommand(commandSocket):
	print "CONNECTED TO THE SERVER.\n"
	print "THE COLLOWING COMMAND WAS SENT: -l\n"
	commandSocket.send(sys.argv[4]) # send datasocket
	commandSocket.send("-l")
# Function is called if the client entered any other command and there are 5 arguments entered on the command line
#Entered command and datasocket will be sent to the server
def fiveArgumentError(commandSocket):
	print "CONNECTED TO THE SERVER.\n"
	print "UNKNOWN COMMAND WAS SENT.\n"
	commandSocket.send(sys.argv[4]) # send datasocket
	commandSocket.send(sys.argv[3]) #send the command
# Function is called if the client entered any other command and there are 6 arguments entered on the command line
#Entered function and datasocket will be sent to the server
def sixArgumentError(commandSocket):
	print "CONNECTED TO THE SERVER.\n"
	print "UNKNOWN COMMAND WAS SENT.\n"
	commandSocket.send(sys.argv[5]) # send datasocket
	commandSocket.send(sys.argv[3]) # send the command
#Function is called if the client entered -g [filename] and 6 aregumentes wre entered on the command line
#function will send -g and datasocket to the server
#this will transfer files from the server to the client
def getCommand(commandSocket):
	print "CONNECTED TO THE SERVER.\n"
	print "THE COLLOWING COMMAND WAS SENT: -g [fileName]\n"
	commandSocket.send(sys.argv[5]) # send datasocket
	commandSocket.send("-g")
	time.sleep(.2)
	commandSocket.send(sys.argv[4])

#Function that makes datasocket, listens to it, receives response from the server and performs corresponind actions
def clientDataSocket():
	#creates a clients socket.
   #the 1st parameter indicates the address familyt, AF_INET indicates that the underlighting network is IPv4
   #2nr parameter indicates that the socket is of type SOCK_STREAM, which means that it is TCP socket
	dataSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	#bind socket to the adress
	#if command is -l, then the port is the 4th argument
	if sys.argv[3] == "-l":
		dataSocket.bind(('', int(sys.argv[4])))
	#if only 5 arguments were entered, but command is not -l, but some erroneous command, port # is the 4th argument
	elif len(sys.argv) == 5 and sys.argv[3] != "-l":
		dataSocket.bind(('', int(sys.argv[4])))
	#because only 6 arguments are allowed, if any other command is ented, then the port number is the 5th argument
	else:
		dataSocket.bind(('', int(sys.argv[5])))
	#https://docs.python.org/2/library/socket.html
	#listen to connection made to the socket
	dataSocket.listen(50)
	#accpeting a connection
	cmndSocket1, cmndSocket2 = dataSocket.accept()
	#receive data from the socket, size should be power of 2
	buffer = cmndSocket1.recv(262144)
	#check for errors
	errorCheck = "ERROR: FILE DOES NOT EXIST.\n"
	errorCheck2 = "ERROR: INVALID COMMAND WAS ENTERED.\n"
	if sys.argv[3] == "-l":
		print "LIST SERVER DIRECTORY CONTENTS: " + buffer
	if buffer == errorCheck:
		print "FILE \"" + sys.argv[4].upper() + "\" NOT FOUND.\n"
		print "SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.\n"
		sys.exit()
	if buffer == errorCheck2:
		print "ERROR: INVALID COMMAND WAS ENTERED.\n"
		print "SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.\n"
		sys.exit()
	if sys.argv[3] == "-g":
		print "FILE RECEIVED: " + sys.argv[4]
		with open(sys.argv[4], 'w') as newFile:
			newFile.write(buffer)
	return dataSocket


if __name__ == '__main__':
	#check to make sure that correct number of arguments were entered
	argumentsCheck()
	print "\nCONTACTING SERVER."
	#creates a clients socket.
    #the 1st parameter indicates the address familyt, AF_INET indicates that the underlighting network is IPv4
    #2nr parameter indicates that the socket is of type SOCK_STREAM, which means that it is TCP socket
	clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	#before the client can send data to the server using TCP socket, a TCP connection must be first established between
    #server and the client
    #the line below initiates a connection between client and the server
    #the paremeter of the connect() function is the adress of the SERVER side of the connection - IP and PORT #
	clientSocket.connect((sys.argv[1], int(sys.argv[2])))
	# if the client entered -l command and 5 arguments were enetered in the command line: listCommand() will be called
	if sys.argv[3] == "-l":
		listCommand(clientSocket)
	#if the client entered -g [filename] and 6 aregumentes wre entered on the command line: getCommand() will be called
	elif sys.argv[3] == "-g":
		getCommand(clientSocket)
	#if the client entered any other command and there are 6 arguments entered on the command line: sixArgumentError() will be called
	elif len(sys.argv) == 6 and sys.argv[3] != "-g":
		sixArgumentError(clientSocket)
	#if the client entered any other command and there are 5 arguments entered on the command line: fiveArgumentError() will be called
	elif len(sys.argv) == 5 and sys.argv[3] != "-l":
		fiveArgumentError(clientSocket)
	dataSocket = clientDataSocket()
	clientSocket.close()
	dataSocket.close()
	print "SOCKET CLOSED. CLIENT SIDE OF THE PROGRAM TERMINATED.\n"



































































