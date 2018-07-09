#input.cpp
	This is a sample input file created for our employee example. This should eventually
	be run through the tool and generate two files that will match shmIn.cpp and shmCalc.cpp
	
	I am unsure if the usage of __attribute__((annotate(""))) is correct in this example 
	input file. However, using clang++ -ast-dump input.cpp we can view an AST dump of the 
	input file which can be help guide us as we move forward
