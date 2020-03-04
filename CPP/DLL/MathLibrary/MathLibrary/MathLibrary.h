#ifndef MATHLIBRARY_H
#define MATHLIBRARY_H

// excludes rarely used things in windows header
#define WIN32_LEAN_AND_MEAN
// windows header file
#include <windows.h>

// dll macros
#ifdef MATHLIBRARY_EXPORTS
#define MATHLIBRARY_API __declspec(dllexport)
#else
#define MATHLIBRARY_API __declspec(dllimport)
#endif

extern "C" MATHLIBRARY_API int add(int a, int b);
extern "C" MATHLIBRARY_API void printId();

class MATHLIBRARY_API Function {
private:
	Function();

public:
	void print();
	static Function generate();
};

#endif