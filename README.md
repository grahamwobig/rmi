# Compiler Tools For Distributed Computing

This branch is devoted to the addition of new features. 
Please keep in mind that the code here may be unstable.

## Deployment


## Branches
	
	master  _______________________
		\
	*devel*	 \_____________________
	
	unshared ______________________
	
## Contributing

1. Branch from devel and add your features
2. Merge new features with devel

## Versioning

To be defined

## Authors

* **Graham Wobig** - *Initial work* - [GrahamWobig](https://github.com/grahamwobig)

* **Kibur Girum** 

* **Breno Cruz** - ** - [Brenodan](https://github.com/brenodan)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## Current project files

# shmIn.cpp (need to start this process prior to following process)
	-to compile: g++ -o shmi -lrt -std=c++11 shmIn.cpp
	-implementation: ./shm /sharedmemfilename portno
	-contains server side logic for socket communication
	-sets up the POSIX shared memory object
	-for proof of concept, user inputs employee data
	-then, signal is sent via socket to client process to perform calculations
	-last, signal from client that calculations are done is recieved 
	-server prints out employee info
# shmCalc.cpp
	-to compile: g++ -o shmc -lrt -std=c++11 shmCalc.cpp
	-implementation: ./shmc /sharedmemfilename portno  
		-portno and filename should be same as those used when executing ./shmi
	-contains client side logic for socket communication
	-awaits signal from server, then runs calculations on data 
	-signals server to display the results
# employee.h
	-contains the struct for our proof of concept
	-has includes for using posix shared memory
	-This is being run in two seperate processes on the same virtual machine
