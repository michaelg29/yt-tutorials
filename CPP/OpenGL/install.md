# OpenGL Installation Process

Following instructions starting at [Learn OpenGL](https://learnopengl.com/Getting-started/OpenGL).  
Make sure everything you configure is set for **_x64_** in your IDE.

## Initial
1. Create directory named 'Linking' in Solution Directory
    * eg. $(SolutionDir)\Linking
2. Set Properties
    * Linker -> Input
        * Additional Dependencies
            \+ opengl32.lib
    * VC++ Directories
        * Library Directories
            \+ $(SolutionDir)\Linking\lib;
        * Include Directories
            \+ $(SolutionDir)\Linking\include;

## GLFW
1. Download package from [GLFW](https://www.glfw.org/download.html)
2. Unzip download and add files to linking directory
    * GLFW\include -> Linking\include
    * all .lib files from corresponding VS folder -> Linking\lib\GLFW
3. Add files to project directory
    * glfw3.dll from corresponding VS folder -> $(ProjectDir)
4. Set Properties
    * Linker -> Input
        * Additional Dependencies
            \+ GLFW\glfw3.lib

## GLAD
1. Download package from [GLAD](https://glad.dav1d.de/)
    * Only change these default settings:
        * API -> gl: Version 3.3
        * Options -> Generate a Loader: checked **(required)**
