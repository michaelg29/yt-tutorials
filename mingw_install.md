# MinGW Installation/Use

MinGW = Minimalist GNU for Windows

It provides many tools, but this outlines the process to download the GCC or GNU C Compiler.

## Installation Instructions

#### Installer Installation
1. Go to [MinGW's home page](http://mingw.org/).
2. Click on the downloads tab.
3. In the package list, click on the *show* button next to *MinGW Installation Manager*.
4. In the dropdown, click *show* on the latest version.
5. Then click on the link to download the executable instructor, *mingw-get-setup.exe*.

#### Installing the Compiler
6. Run the downloaded .exe file.
7. Click *install*.
8. Input the desired path (the default is fine), but make sure it has sufficient permissions.
    1. Make sure that all the options are ticked.
    2. Then click *continue*.
9. Click *Installation* and *continue*.
10. This will open up the MinGW installation manager.
    1. Mark *mingw32-gcc-g++-bin* for installation.
    2. Go into *All Packages* on the left and scroll down to find *mingw32-gdb-bin* and mark it for installation too.
    3. Then in the toolbar, click *Installation* > *Apply Changes*.
    4. Click *Apply*.

#### Setting System Variables
11. Open up the control panel and search *environment variables* and click on *Edit the system environment variables*.
12. Click *Environment Variables* at the bottom of the dialogue.
13. In *System Variables*, select *PATH* and click *Edit*.
14. Click *New* and input the installation path for MinGW\bin (ie *C:\MinGW\bin*).
15. To verify the path, open up a command prompt and run the command, *gcc --version*.

## Execution Instructions

***Note that if you want to run C++ files, simply replace the *gcc* with *g++*.

#### For Running Files

1. Open up the command prompt.
2. Get a list of all the source files involved in your program, you do not need to compile the header files.
```
gcc main.c file2.c ...
a.exe
```

#### For Debugging Programs

1. Open up the command prompt
2. Get a list of all the source files involved in your program, you do not need to compile the header files.
```
gcc -g main.c file2.c ...
gdb a.exe
```
3. The GNU Debugger (gdb) has many commands and a comprehensive list can be found [here](https://users.ece.utexas.edu/~adnan/gdb-refcard.pdf). These are a few of the basic ones:
    1. Running the current program: `run arg1 arg2 ...`
    2. Adding a breakpoint: `break [file_name]:[line_number]`
    3. Retargeting the debugger: `target exec a.exe`
    4. List commands: `list`
    5. Quit the debugger: `quit`
4. And during execution (at a breakpoint), here are some other commands:
    1. Print a variable: `print i`
    2. Continue: `c` or `continue`
    3. Next: `n` or `next`, executes entire next line as single instruction
    4. Step: `s` or `step`, will step into functions
    5. Stop debugging: `kill`
5. You can also run basic C/C++ commands:
    1. eg. `print sizeof(double)` will print `8`
