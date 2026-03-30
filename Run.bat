@echo off
setlocal

set PROJECT_DIR=%1
set BUILD_DIR=%PROJECT_DIR%\build

if exist build rmdir /S /Q build

mkdir build

copy bin\ build\ > nul

copy %BUILD_DIR% build\ > nul

mkdir build\resources
copy %BUILD_DIR%\resources build\resources\ > nul

cd build

start main.exe

cd ..