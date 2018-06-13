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

bool endConnection = false;
std::mutex mut;
	
using namespace std;

void sendData(int fd, int* err) {
	while (1) {
		char buff[1024];
		memset(buff, 0, 1024);
		cin.getline(buff, 1024);
		if (*buff == '#') {
			endConnection = true;
			close(fd);
			break;
		}
		int numBytes = strlen(buff);
		int sent = 0;
		if (numBytes != 0) {
			do {
				int temp = send(fd, buff, 1024, 0);
				if (sent < 0) {
					perror("Error sending from Server");
					*err = 1;
				}
				sent += temp;
			}while (numBytes > sent);
		}
	}
}

void recvData(int fd, char* buff, bool *display, int* err) {
	while (!endConnection) {
		int recvd = 0;
		recvd = recv(fd, buff, 1024, 0);
		if (recvd <= 0) {
			if (recvd == 0) {
				cout << "Client disconnected! Type # to end session" << endl;
				endConnection = true;
				break;
			}
			*err = 1;
		}
		*display = true;
	}
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
	cout << "Type # to end the connection or enter something to send" << endl;
	bool disp = false;
	int sndErr = 0;
	int recvErr = 0;
	thread recvThd(recvData, cliSock, buffer, &disp, &recvErr);
	thread sendThd(sendData, cliSock, &sndErr);
	while (!endConnection) {
		if (disp)  {
			mut.lock();
			cout << "Client: " <<  buffer << endl;
			memset(buffer, 0, 1024);
			disp = false;
			mut.unlock();
		}	
		if (sndErr != 0) {
			break;
		}
		if (recvErr != 0) {
			break;
		}
	}
	recvThd.join();
	sendThd.join();
	close(servSock);
	return 0;
}
