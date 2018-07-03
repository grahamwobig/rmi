# Compiler Tools For Distributed Computing

This project aims to help developers to implement distributed applications.

### Prerequisites

 -- RedHat
 -- LLVM
 -- Clang

### Setting up the environment:

A step-by-step guide:

```
Build LLVM
```
Steps to build on top of 4.0.0
# working dir
	cd /rpc/llvm
	wget http://releases.llvm.org/4.0.0/llvm-4.0.0.src.tar.xz
	tar xf llvm-4.0.0.src.tar.xz

# this is clang, also need to be built later
	wget http://releases.llvm.org/4.0.0/cfe-4.0.0.src.tar.xz
	tar xvf cfe-4.0.0.src.tar.xz 

# http://llvm.org/docs/GettingStarted.html#local-llvm-configuration
	mkdir /rpc/llvm/llvm-install 
	
	mkdir -p llvm-4.0.0.src/build
	cd llvm-4.0.0.src/build/
	cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/rpc/llvm/llvm-install /rpc/llvm/llvm-4.0.0.src


# you want to do this on a fast machine...
	time make -j 30
	time make install
	
```
Build Clang
```
# http://clang.llvm.org/get_started.html

	cd cfe-4.0.0.src/build/
	cmake -G "Unix Makefiles" --disable-ld-version-script -DLLVM_CONFIG=/rpc/llvm/llvm-install/bin/llvm-config -			DCMAKE_INSTALL_PREFIX=/rpc/llvm/clang-install /rpc/llvm/clang-4.0.0.src

	time make -j 30
	time make install

#suggestion copy header files to / folder
	sudo cp -R /rpc/llvm/llvm-install/include/llvm /llvm
	sudo cp -R /rpc/llvm/llvm-install/include/llvm-c /llvm-c


## Installing and Compiling


## Tests


## Deployment


## Branches
	
	master  _______________________
		\
	devel	 \_____________________
	
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

## License


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
