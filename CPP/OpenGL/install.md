# OpenGL Installation Process

#### Some Pointers
* Make sure everything you configure is set for **_x64_** in your IDE.
* *$(SolutionDir)* is the directory containing the *.sln* file for the Visual Studio Solution
* *$(ProjectDir)* is the directory containing the *.vcxproj* file for the Visual Studio Project

## Initial
1. Create project in IDE (These directions are for Visual Studio 2019)
2. Create directory named 'Linking' in *$(SolutionDir)
3. Set Project Properties
    * Linker -> Input
        * Additional Dependencies
            \+ opengl32.lib
    * VC++ Directories
        * Library Directories
            \+ $(SolutionDir)\Linking\lib;
        * Include Directories
            \+ $(SolutionDir)\Linking\include;
4. Create directories *src*, *lib*, and *assets* in *$(ProjectDir)*

## GLFW
1. Download pre-compiled binaries from [GLFW](https://www.glfw.org/download.html)
2. Unzip download and add files to linking directory
    * *GLFW\include* -> *$(SolutionDir)\Linking\include*
    * all *.lib* files from corresponding VS folder -> *Linking\lib\GLFW*
3. Add files to project directory
    * *glfw3.dll* from corresponding VS folder -> *$(ProjectDir)*
4. Set Project Properties
    * Linker -> Input
        * Additional Dependencies
            \+ GLFW\glfw3.lib

## GLAD
1. Download package (*.zip* file) from [their site](https://glad.dav1d.de/)
    * Only change these default settings:
        * API -> gl: Version 3.3
        * Options -> Generate a Loader: checked **(required)**
2. Unzip download and add files to project
	* *glad\include* -> *$(SolutionDir)\Linking\include*
		* this should add two folders (*glad*, *KHR*) to your include directory
	* *glad\src\glad.c* -> *$(ProjectDir)\lib*

## GLM
1. Download the latest package from [GLM](https://github.com/g-truc/glm/tags)
    * Click on the package and download the .zip file named *glm-x.x.x.x.zip*
2. Unzip download and add files to linking directory
	* Drag the *glm\glm* directory (containing many *.hpp* files) to *$(SolutionDir)\Linking\include*

## stb_image.h
1. Download raw file from [GitHub/nothings/stb](https://raw.githubusercontent.com/nothings/stb/master/stb_image.h)
2. Create directory *stb* in *$(SolutionDir)\Linking* directory
3. Move *stb_image.h* file that was just downloaded to *$(SolutionDir)\Linking\stb* directory
4. Create file *stb.cpp* in *$(ProjectDir)\lib* directory and add the following code:
```cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
```

## Open Asset Import Library (ASSIMP)
1. Make sure you have CMake installed ([Download Here](https://cmake.org/download))
2. Download the latest release ASSIMP code (.zip file) from [their site](https://assimp.org/index.php/downloads) or [GitHub](https://github.com/assimp/assimp/releases)
3. Unzip the download to some location on your computer (eg. *C:\src\libs*)
4. Open up the CMake GUI and input the following entries:
	* Where is the source code: *C:\src\libs\assimp-x.x.x*
	* Where to build the binaries: *C:\src\libs\assimp-x.x.x\build*
	* *note: replace C:\src\libs\assimp-x.x.x with the root of the installation (wherever you unzipped the library*
5. Push configure
	* If it asks to create a new directory, accept
	* For the version, choose the corresponding compiler you have on your computer (for this installation it is Visual Studio 16 2019)
	* If it gives errors, press configure then generate again
6. Go to the build directory (*C:\src\libs\assimp-x.x.x\build*) and open the *Assimp.sln* file in the *build* folder in Visual Studio
	* You should see several projects in the project explorer
	* Make sure the ALL_BUILD project is set as the startup project (it will be highlighted)
	* Click on Build > Build Solution (make sure you are in the **_x64_** configuration)
7. After this, go back to the build directory and go to *build/code/Debug* or *build/code/Release*
	* *build\code\Debug\assimp-vcxxx-mtd.dll* -> *$(ProjectDir)*
	* *build\code\Debug\assimp-vcxxx-mtd.lib* -> *$(SolutionDir)\linking\lib\assimp*
8. Adding the include directory
	* *assimp-x.x.x\include* -> *$(SolutionDir)\linking\include*
	* *assimp-x.x.x\build\include* -> *$(SolutionDir)\linking\include*
9. Configuring the OpenGL Project Properties
	* the include directory is already taken care of
	* Linker -> Input
        * Additional Dependencies
            \+ assimp\assimp-vcxxx-mtd.lib
            
## FreeType (Text Rendering Library)
1. Download the latest release (.zip file) from [their site](https://www.freetype.org/download.html)
2. Unzip the download to some location on your computer (eg. *C:\src\libs*)
3. To build the library, you can either use CMake or Visual Studio. These steps outline the process for Visual Studio.
4. Go into the unizpped folder and find the directory: *freetype-x.xx.x\builds\windows\vc2010* and open the .sln file in Visual Studio.
5. Once Visual Studio is open, make sure the build configuration is **_x64_** and then click on build (Ctrl + Shift + B)
6. Once that process is finished, find the directory *freetype-x.xx.x\objs\x64\Debug or Release*
	* *objs\x64\Debug\freetype.dll* -> *$(ProjectDir)*
	* *objs\x64\Debug\freetype.lib* -> *$(SolutionDir)\linking\lib\freetype*
7. Adding the include directory
	* *freetype-x.xx.x\include* -> *$(SolutionDir)\linking\include*
	* **IMPORTANT**: you should end up with the *ft2build.h* file in the root include that is alongside the freetype folder, **do not combine them or put them in another directory**
8. Configuring the OpenGL Project Properties
	* the include directory is already taken care of
	* Linker -> Input
		* Additional Dependencies
			\+ freetype\freetype.lib