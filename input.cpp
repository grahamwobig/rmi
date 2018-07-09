#include <iostream>

using namespace std;
	
struct Employee {
	int age, sal, yrs;
	char posn;
};

__attribute__((annotate("client"))) void calculate(Employee);	

int main(int argc, char *argv[]) {
	__attribute__((annotate("shared"))) Employee e;
	cout << "Enter employee age: ";
	cin >> e.age;
	cout << "Enter employee's years with company: ";
	cin >> e.yrs;
	cout << "Enter employee's posn character code (e, s, or p): ";
	cin >> e.posn;
	calculate(e);
	cout << "Employee salary calculated to be $" << e.sal;
	return 0;
}

void calculate(Employee e) {
	if (e.posn == 'e') {
		e.sal = e.age * e.yrs * 10;
	}	
	else if (e.posn == 's') {
		e.sal = e.age * e.yrs * 20;
	}
	else {
		e.sal = e.age * e.yrs * 30;
	}
}
