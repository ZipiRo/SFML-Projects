@echo off
setlocal

set PROJECT_DIR=%1
set SFML=C:\SFML-3.0.2
set SFML_LIB=%SFML%\lib
set SFML_BIN=%SFML%\bin
set SFML_INC=%SFML%\include
set SFML_GPP=%SFML%\mingw64\bin\g++.exe

%SFML_GPP% --version

if not exist bin (
    mkdir bin
    copy %SFML%\bin bin\ > nul
    copy %SFML%\mingw64\bin\libstdc++-6.dll bin\ > nul
)

if not exist "%PROJECT_DIR%\build" (
    mkdir %PROJECT_DIR%\build
)
 
set BUILD_DIR=%PROJECT_DIR%\build

cd %PROJECT_DIR%

%SFML_GPP% -c main.cpp -I"%SFML_INC%" || goto :Exit
%SFML_GPP% main.o -o Build\main -L"%SFML_LIB%" ^ -lsfml-system -lsfml-window -lsfml-graphics || goto :Exit
del main.o

cd ..
Run %PROJECT_DIR%

:Exit