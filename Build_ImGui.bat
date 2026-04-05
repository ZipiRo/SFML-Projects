@echo off
setlocal

set PROJECT_DIR=%1
set SFML=C:\SFML-3.0.0
set SFML_LIB=%SFML%\lib
set SFML_BIN=%SFML%\bin
set SFML_INC=%SFML%\include
set SFML_IMGUI=%SFML%\imgui
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

%SFML_GPP% -c main.cpp "%SFML_IMGUI%\imgui.cpp" "%SFML_IMGUI%\imgui_draw.cpp" "%SFML_IMGUI%\imgui_tables.cpp" "%SFML_IMGUI%\imgui_widgets.cpp" "%SFML_IMGUI%\imgui-SFML.cpp" -I"%SFML_INC%" -I"%SFML_IMGUI%" || goto :Exit

%SFML_GPP% main.o imgui.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui-SFML.o -o Build\main -L"%SFML_LIB%" ^ -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio -lopengl32 || goto :Exit

del main.o
del imgui.o del imgui_draw.o del imgui_tables.o del imgui_widgets.o del imgui-SFML.o

cd ..
Run %PROJECT_DIR%

:Exit