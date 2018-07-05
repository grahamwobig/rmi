#ifndef EMPLOY_H
#define EMPLOY_H

#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

struct Employee {
	sem_t sem;
	int age, yrsExp, salary;
	char posn;
};

#endif
