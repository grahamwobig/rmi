#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include "employee.h"

using namespace std;
	
int main(argc, char *argv[]) {
	int cliSock, ret, fd;
	struct sockaddr_in cli;
	struct sockaddr_in serv;
	//open shared memory
	fd = shm_open(argv[1], O_RDWR, 777);
	if (fd < 0) {
		perror("Error opening shared memory");
		return -1;
	}
	//map shared memory
	Employee* e = reinterpret_cast<Employee*> (mmap(NULL, sizeof(Employee), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
	if (reinterpret_cast<void*>(e) == MAP_FAILED) {
		perror("Failed to map shared memory");
		return -1;
	}
	//close fd for shared mem, no longer necessary
	close(fd);
	cout << "Shared Memory ready..." << endl;
	//create socket
	cliSock = socket(AF_INET, SOCK_STREAM, 0);
	if (cliSock < 0) {
		perror("Failed to create socket");
		return -1;
	}
	cout << "Socket Created..." << endl;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(stoi(argv[2], NULL, 0));
	serv.sin_addr.s_addr = INADDR_ANY;
	ret = connect(cliSock, reinterpret_cast<struct sockaddr*>(&serv), sizeof(serv));
	if (ret < 0) {
		perror("Failed to connect to server");
		return -1;
	}
	cout << "Successfully connected to server!" << endl;
	return 0;
}
