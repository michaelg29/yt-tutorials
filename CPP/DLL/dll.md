# How to create and use a library in C/C++ using Visual Studio 2019

Will be using the example of creating a math library.

## To create library project
1. In VS2019, go to create new project
2. Set language to C++
3. Select Windows Desktop Wizard
4. Set name/location as you wish and click create
5. In the dialog that pops up, set the following parameters:
    1. Application type: Dynamic Link Library (.dll)
    2. Check 'empty project'

## To add library files
1. Create general header file, *MathLibrary.h*
	1. Add the following code:
	```c++
    #ifndef MATH_LIBRARY_H
    #define MATH_LIBRARY_H

	// Exclude rarely-used stuff from Windows headers
    #define WIN32_LEAN_AND_MEAN
    // Windows Header Files
    #include <windows.h>

    #ifdef MATHLIBRARY_EXPORTS
    #define MATHLIBRARY_API __declspec(dllexport)
    #else
    #define MATHLIBRARY_API __declspec(dllimport)
    #endif

    #endif // MATH_LIBRARY_H
    ```
2. Create dll entry source file, *dllmain.cpp*
	1. Add the following code:
	```c++
    // dllmain.cpp : Defines the entry point for the DLL application.
    #include "MathLibrary.h"

    BOOL APIENTRY DllMain( HMODULE hModule,
    			   DWORD  ul_reason_for_call,
    			   LPVOID lpReserved
    			   )
    {
      switch (ul_reason_for_call)
      {
      case DLL_PROCESS_ATTACH:
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      case DLL_PROCESS_DETACH:
      	break;
      }
      return TRUE;
    }
    ```
3. Start creating your classes and methods that will be included with your library.
	1. You must include "MathLibrary.h" in source and header files
	2. To specify *extern* functions, do the following:
		1. In header file:
        ```c++
        extern "C" MATHLIBRARY_API int add(int a, int b);
        ```
        2. In source file that includes this header file:
        ```c++
        int add(int a, int b) {
        	return a + b;
        }
        ```
	3. For static (dll internal state) variables, declare them in a source file:
	```c++
    static int no_calls;
    ```
    4. For classes, do the following:
    	1. In header file:
    	```c++
        class MATHLIBRARY_API Function {
        public:
            Function();

            void print();

            static Function generate(const char* arg);
        };
        ```
        2. In source file that includes this header file:
        ```c++
        Function::Function() { ... }
        
        void Function::print() { ... }
        
        Function Function::generate(const char* arg) { ... }
        ```
        *note that you can use dll static variables in these classes if they are declared in
        scope.*
5. Build your project, you should get a .dll and a .lib file in the output folder
        
## To link library to project
*Mind the version you are creating these settings for, it is necessary to align a library's target platform with your project.
1. Create your project and add a main.cpp file
2. Open up the properties page (right click project, at bottom of dropdown menu)
	1. C/C++ -> General
		1. Additional Include Directories:
			add *directory that contains MathLibrary.h*
            * this is usually the *include* directory that ships with third party .dll libraries
    2. Linker -> General
    	1. Additional Library Directories:
    		add *output directory of dll project, contains .dll and .lib files*
   	2. Linker -> Input
   		1. Additional Dependencies:
   			add *name of your project.lib* (name can be found in output directory)
	3. Build Events -> Post Build Event
		1. Command Line:
			add 
			```
			xcopy /y /d "PATH/TO/DLL/MathLibrary.dll" "$(OutDir)"
			```
3. Go ahead and include header files from library and use functions and classes in the library
