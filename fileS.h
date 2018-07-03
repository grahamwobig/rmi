#ifndef FILE_STR
#define FILE_STR

#include<cstdio>

class fileS {
	public:
	fileS(char* fName);
	fileS();
	~fileS();
	unsigned char* makeByte();
	FILE* readByte(unsigned char* in);
	int getSize();
	char* getName();
	private:
	int size;
	char* name;
	unsigned char* byte;
};

#endif
