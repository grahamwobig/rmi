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
	
int main(int argc, char *argv[]) {
	int cliSock, ret, fd;
	struct sockaddr_in cli;
	struct sockaddr_in serv;
	//check for usage
	if (argc < 3) {
		cout << "Not enough arguments. Usage:./shmc /filename portno." << endl;
		return -1;
	}
	//open shared memory
	fd = shm_open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
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
	//block process until notifcation from server
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
		//obtain semaphore
		ret = sem_wait(&e->sem);
		if (ret < 0) {
			perror("Error obtaining semaphore");
			return -1;
		}
		//calculate salary
		cout << "Signal recieved from server, calculating salary..." << endl;
		if (e->posn == 'e') {
			e->salary = (e->yrsExp + e->age) * 10;
		}
		else if (e-> posn == 's') {
			e->salary = (e->yrsExp + e->age) * 20;
		}
		else {
			e->salary = (e->yrsExp + e->age) * 30;
		}
		//release semaphore
		ret = sem_post(&e->sem);
		if (ret < 0) {
			perror("Error releasing semaphore");
			return -1;
		}
	}
	//notify server
	cout << "Calculations complete... notifying server." << endl;
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
	//no longer need socket
	close(cliSock);
	return 0;
}
