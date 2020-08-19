# Java/Eclipse Installation

## JDK
1. Download the most recent JDK for your system from [Oracle](https://www.oracle.com/java/technologies/javase-downloads.html)
2. Run the downloaded installer and follow the steps.
	* Take note of the installation path (on Windows, it should be something like *C:\Program Files\Java\jdk-x.x.x*)
3. Setup the **system** environment variables
	* *JAVA_HOME* = *path\to\jdk-x.x.x*
	* *PATH* += *%JAVA_HOME%\bin*
4. Verify the installation open in the command line with the command: *java -version*

## Eclipse
1. Download the installer for your system from [Eclipse](https://www.eclipse.org/downloads/packages/installer)
2. Run the installer and install your desired option
	* For most purposes, the basic option: Eclipse IDE for Java Developers, should be enough.
	* Accept any licenses that come up
3. Run Eclipse
4. Set the workspace to the folder where you want to house your Java projects