#include <iostream>

#include <MathLibrary.h>

int main() {
	std::cout << "Hello, world!" << std::endl;

	std::cout << add(5, 3) << std::endl;
	printId();
	printId();

	Function f = Function::generate();
	f.print();

	return 0;
}