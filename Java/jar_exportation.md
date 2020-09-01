# JAR File Exportation/Execution (Eclipse)

This outlines the process of exporting a Java project through a .jar (Java Archive) file that can be run on any platform that has the JDK installed.

## Exportation Instructions (Eclipse)
1. Right click on the project, and click on the item labeled, "Export". This should open up a wizard.
2. Click on Java > JAR File, and then hit "Next".
3. Type in or browse to find the path where you want the exported JAR to end up.
4. Click next twice, you should end up at a page in the same wizard titled, "JAR Manifest Specification".
5. In the entry named "Main Class", locate the class that contains the entry method.
	* Look for this: `public static void main(String[] args)`
6. Click finish, and the file should be exported to the destination.

## Execution Instructions

#### Running through the filesystem
Simply double click the file and it should run. *Note that you cannot add any arguments and any output to a console will not be shown.*

#### Running through the command line
The command is the same for all platforms, since Java is a WORA (Write Once Run Anywhere) language

```
java -jar filename.jar arg1 arg2 ...
```