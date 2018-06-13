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

bool endConnection = false;
std::mutex mut;

using namespace std;

void sendData(int fd, int* err) {
	while (1) {
		char buff[1024];
		memset(buff, 0, 1024);
		cin.getline(buff,1024);
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
					perror("Error sending from Client");
					*err = 1;
				}
				sent += temp;
			}while (numBytes > sent);
		}
	}
}

int recvData(int fd, char* buff, bool* display, int* err) {
	while (!endConnection) {
		int recvd = 0;
		recvd = recv(fd, buff, 1024, 0);
		if (recvd <= 0) {
			if (recvd == 0) {
				cout << "Server disconnected! Type # to end session" << endl;
				endConnection = true;
				break;
			}
			*err = 1;
		}
		*display = true;
	}
}

int main(int argc, char *argv[]) {
	int cliSock, val;
	struct sockaddr_in cliAddr;
	struct sockaddr_in servAddr;
	char buffer[1024];
	//create the socket
	cliSock = socket(AF_INET, SOCK_STREAM, 0);
	if (cliSock < 0) {
		perror("Failed to create socket");
		exit(1);
	}
	cout << "Socket Created!" << endl;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT_NO);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	val = connect(cliSock, (struct sockaddr*) &servAddr, sizeof(servAddr));
	if (val < 0) {
		perror("Failed to connect to the Server");
		exit(1);
	}
	cout << "Connection with Server Successful!" << endl;
	bool disp = false;
	int sndErr = 0;
	int recvErr = 0;
	thread recvThd(recvData, cliSock, buffer, &disp, &recvErr);
	thread sendThd(sendData, cliSock, &sndErr);
	while (!endConnection) {
		if (sndErr != 0) {
                        break;
                }
                if (recvErr != 0) {
                        break;
                }
		if (disp) { 
			mut.lock();
			cout << "Server: " << buffer << endl;
			memset(buffer, 0, 1024);
			disp = false;
			mut.unlock();
		}
	}
	recvThd.join();
	sendThd.join();
	close(cliSock);
	return 0;
}
