#include "MathLibrary.h"

#include <iostream>

static int ID = 3141592654;

int add(int a, int b) {
	return a + b;
}

void printId() {
	std::cout << ID << std::endl;
	ID++;
}

Function::Function() {
	std::cout << "constructor" << std::endl;
}

void Function::print() {
	std::cout << "print function" << std::endl;
}

Function Function::generate() {
	std::cout << "gen function" << std::endl;
	return Function();
}