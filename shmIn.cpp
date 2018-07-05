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
	fd = shm_open(argv[1], O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
	//intialize semaphore
	ret = sem_init(&e->sem, 1, 1);
	if (ret < 0) {
		perror("Error initializing semaphore");
		return -1;
	}
	//obtain semaphore
	ret = sem_wait(&e->sem);
	if (ret < 0) {
		perror("Error obtaining semaphore");
		return -1;
	}
	//get data from user input and place in shared memory
	cout << "Please enter employee age: ";
	cin  >> e->age;
	cout <<  "Please enter employee years of experience: ";
	cin >> e->yrsExp;
	cout << "Please enter employee's position code(e, s, or p): ";
	cin  >> e->posn;
	cout << "Data input done! Notifying client..." << endl;
	//Release semaphore
	ret = sem_post(&e->sem);
	if (ret < 0) {
		perror("Error releasing semaphore");
		return -1;
	}
	//send signal to client
	int sendCode = 1;
	int sent = 0;
	do {
		ret = send(cliSock, &sendCode, sizeof(int), 0);
		if (ret < 0) {
			perror("Error notifying client via socket");
			return -1;
		}
		sent += ret;
	} while (sent < sizeof(int));
	//block this process while we wait for signal from client that calculations are done
	int recvCode = 0;
	int recvd = 0;
	do {
		ret = recv(cliSock, &recvCode, sizeof(int), 0);
		if (ret < 0) {
			if (ret == 0) {
				cout << "Client has closed the socket connection" << endl;
				return -1;
			}
			perror("Error recieving signal from client via socket");
			return -1;
		}
		recvd += ret;
	} while (recvd < sizeof(int));
	if (recvCode == 1) {
		//obtain semaphore again
		ret = sem_wait(&e->sem);
		if (ret < 0) {
			perror("Error obtaining semaphore");
			return -1;
		}
		//disp results from calculation
		cout << "Employee salary calculated to be: " << e->salary << endl;
		//release semaphore
		ret = sem_post(&e->sem);
		if (ret < 0) {
			perror("Error releasing semaphore");
			return -1;
		}
	}
	//done using semaphore so we can destroy
	ret = sem_destroy(&e->sem);
	if (ret < 0) {
		perror("Failed to unlink semaphore");
		return -1;
	}
	//done with sockets so we can close
	close(servSock);
	close(cliSock);
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
