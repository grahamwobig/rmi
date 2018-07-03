#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include "employee.h"
#define BACK_LOG 1

using namespace std;

int main(int argc, char *argv[]) {
	int fd, ret, cliSock, servSock;
	struct sockaddr_in serv;
	//check for proper usage
	if (argc < 3) {
		cout << "Not enough argmuents. Usage:./shmi /filename portno." << endl;
		return -1;
	}
	//create shared memory
	fd = shm_open(argv[1], O_CREAT | O_EXCL | O_RDWR, 777);
	if (fd < 0) {
		perror( "Error creating shared memory block");
		return -1;
	}
	ret = ftruncate(fd, sizeof(Employee));
	if (ret < 0) {
		perror("Error allocating shared memory block");
		return -1;
	}
	//map shared memory
	Employee* e = reinterpret_cast<Employee*> (mmap(NULL, sizeof(Employee), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
	if (reinterpret_cast<void*>(e) == MAP_FAILED) {
		perror("Failed to map shared memory");
		return -1;
	}
	//close shared mem fd, not necessary to read or write in shared memory
	close(fd);
	cout << "Shared Memory initialized..." << endl;
	//create socket
	servSock = socket(AF_INET, SOCK_STREAM, 0);
	if (servSock < 0) {
		perror("Failed to create socket");
		return -1;
	}
	cout << "Socket created..." << endl;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(stoi(argv[2], NULL, 0));
	serv.sin_addr.s_addr = INADDR_ANY;
	//bind socket to port
	ret= bind(servSock, reinterpret_cast<struct sockaddr*>(&serv), sizeof(serv));
	if (ret < 0) {
		perror("Failed to bind socket");
		return -1;
	}
	cout << "Socket successfully binded..." << endl;
	ret = listen(servSock, BACK_LOG);
	if (ret < 0) {
		perror("Faied to listen for connection");
		return -1;
	}
	cout << "Listneing for connections..." << endl;
	int len = sizeof(serv);
	cliSock = accept(servSock, reinterpret_cast<struct sockaddr*>(&serv), reinterpret_cast<socklen_t*>(&len));
	if (cliSock < 0) {
		perror("Failed to connect with Client");
		return -1;
	}
	cout << "Successfully connected to Client!" << endl;
	cout << "Please enter employee age: ";
	cin  >> e->age;
	cout <<  "Please enter employee years of experience: ";
	cin >> e->yrsExp;
	cout << "Please enter employee's position code(e, s, or p): ";
	cin  >> e->posn;
	cout << "Data input done! Notifying client..." << endl;
	//unmap shared memory
	ret = munmap(reinterpret_cast<void*>(e), sizeof(Employee));
	if (ret < 0) {
		perror("Error unmapping shared memory");
		return -1;
	}
	//unlink shared memory
	ret = shm_unlink(argv[1]);
	if (ret < 0) {
                perror("Error unlinking shared memory");
                return -1;
        }	
	return 0;
}
