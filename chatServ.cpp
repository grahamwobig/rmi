#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <mutex>
#define PORT_NO 8100
#define BACKLOG 5
#define END_MSG "*$*"

	
using namespace std;

int sendData(int fd, char* buff) {
	int numBytes = sizeof(*buff);
	int sent = 0;
	if (*buff == '#') {
		return -2;
	}
	do {
		sent = send(fd, buff, sizeof(buff), 0);
		if (sent < 0) {
			perror("Error sending from Server");
			return -1;
		}
		numBytes += sent;
	}while (numBytes != sent);
	return 0;
}

int recvData(int fd, char* buff) {
	int recvd = 0;
	do {
		recvd = recv(fd, buff, sizeof(buff), 0);
		if (recvd <= 0) {
			if (recvd == 0) {
				cout << "Client disconnected!" << endl;
				return -2;
			}
			return -1;
		}
	} while (*buff != END_MSG)
}

int main(int argc, char *argv[]) {
	int servSock, cliSock, len, retVal;
	struct sockaddr_in serv;
	char buffer[1024];
	//create the socket
	servSock = socket(AF_INET, SOCK_STREAM, 0);
	if (servSock < 0) {
		perror("Failed to create socket");
		exit(1);
	}
	cout << "Socket Created" << endl;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT_NO);
	serv.sin_addr.s_addr = INADDR_ANY;
	//bind the socket to the port
	retVal = bind(servSock, (struct sockaddr*) &serv, sizeof(serv));
	if (retVal < 0) {
		perror("Failed to bind socket");
		exit(1);
	}
	cout << "Socket bind was successful!" << endl;
	retVal = listen(servSock, BACKLOG);
	if (retVal < 0) {
		perror("Failed to listen for connection");
		exit(1);
	}
	cout << "Listening for connections..." << endl;
	len = sizeof(serv);
	cliSock = accept(servSock, (struct sockaddr*) &serv, (socklen_t*) &len);
	if (cliSock < 0) {
		perror("Failed to connect with Client");
		exit(1);
	}
	cout << "Connection with Client was successful!" << endl;
	cout << "Type # to end the connection" << endl;
	bool endConnect = false;
	/**
 	* insert threading logic
 	*/
	close(servSock);
	return 0;
}
