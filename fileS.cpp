#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdlib.h>

using namespace std;

class fileS {
	public:
	fileS(char* fName);
	fileS();
	~fileS();
	unsigned char* makeByte();
	void readByte(unsigned char* in);
	int getSize();
	char* getName();
	private:
	int size;
	char* name;
	unsigned char* byte;
};

unsigned char* fileS::makeByte() {
	ifstream ifs(name, ios::binary | ios::ate);
	if (ifs.is_open() == false) {
		cout << "Error opening input file!" << endl;
		exit(1);
	}
	size = ifs.tellg();
	ifs.seekg(ios::beg);
	int nameLen = strlen(name);
	byte = new unsigned char[size + sizeof(int) + nameLen];
	for (int i = 0; i < sizeof(int); i++) {
		byte[i] = size >> ((3 - i) * 8);
	}
	byte[4] = nameLen;
	for (int i = (sizeof(int) + 1); (i - (sizeof(int) + 1)) <  nameLen; i++) {
		byte[i] = static_cast<unsigned char>(name[i - sizeof(int) - 1]);
	} 
	ifs.read(reinterpret_cast<char*>((byte + sizeof(int)+ 1 + nameLen)), size);
	ifs.close();
	return byte;
}

void fileS::readByte(unsigned char* in) {
	size = (static_cast<int>(in[0]) << 24) + (static_cast<int>(in[1]) << 16) + (static_cast<int>(in[2]) << 8) + static_cast<int>(in[3]);
	int nameLen = static_cast<int>(in[4]);
	name = new char[nameLen];
	for (int i = 0; i < nameLen; i++) {
		name[i] = static_cast<char>(in[i + 5]);
	}
	ofstream ofs(name, ios::binary);
	if (ofs.is_open() == false) {
		cout << "Error creating the output file!" << endl;
		exit(1);
	}
	ofs.write(reinterpret_cast<char*>((in + sizeof(int) + 1 + nameLen)), size);
	if (ofs.tellp() != size) {
		cout << "Error! File size and number of bytes written don't match." << endl;
		exit(1);
	}
	ofs.close();
	delete[] name;
}

fileS::fileS(char* fName) {
	name = fName;
}

fileS::fileS() {}

fileS::~fileS() {
	delete[] byte;
}
int fileS::getSize() {
	return size;
}

char* fileS::getName() {
	return name;
}
