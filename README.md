This are SFML projects I made.

## Dependencies
- SFML 3.0.0 + Dear ImGui >= 1.91.1, < 1.92.0 + ImGui-SFML  (For ImGui Template) 
- SFML 3.0.2 (For Default Template)
- MinGW 14.2.0

To use my build batch file (B.bat) you will need to change the SFML and MinGW path. 

Otherwise it will not work, because SFML 3.0.2 uses a certain C++ compiler.

You can get SFML from here: https://www.sfml-dev.org/download/sfml/3.0.2

There you can find the compiler needed for your certain SFML build.

## How To Use
When you want to build something your current directory needs to be the one with the batch file.

In CMD, PowerShell, etc. use these:
- To build a project use B.bat 'DirectoryName'
- To run a project use Run.bat 'DirectoryName'
- To Create a new project use CProject.bat 'NewProjectName''Project Type' [Default, ImGui]
