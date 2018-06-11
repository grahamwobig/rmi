#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#define PORT_NO 8100
#define END_MSG "*$*"


using namespace std;

int sendData(int fd, char* buff) {
	int numBytes = sizeof(buff);
	int sent = 0;
	if (*buff == '#') {
		return -2;
	}
	do {
		sent = send(fd, buff, sizeof(buff), 0);
		if (sent < 0) {
			perror("Error sending from Client");
			return -1;
		}
		numBytes += sent;
	} while (numBytes != sent)
}

int recvData(int fd, char* buff) {
	int recvd = 0;
	do {
		recvd = recv(fd, buff, sizeof(buff), 0);
		if (recvd <= 0) {
			if (recvd == 0) {
				cout << "Server disconnected!" << endl;
				return -2;
			}
			return -1;
		}
	} while (*buff != END_MSG)
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
	printf("Socket Created!\n");
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT_NO);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	val = connect(cliSock, (struct sockaddr*) &servAddr, sizeof(servAddr));
	if (val < 0) {
		perror("Failed to connect to the Server");
		exit(1);
	}
	printf("Connection with Server Successful!\n");
	/**
 	*threading logic goes here
 	*/
	return 0;
}
