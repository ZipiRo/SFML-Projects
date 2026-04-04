@echo off
setlocal

set PROJECT_DIR=%1

if not exist %PROJECT_DIR%\.project_type (
    echo The '.project_type' file from the %PROJECT_DIR%\ directory could not be found.
    goto :Exit
)

set /p PROJECT_TYPE=<%PROJECT_DIR%\.project_type

if "%PROJECT_TYPE%"=="imgui" (
    echo ========= ImGui Build ========= 
    call Build_ImGui %PROJECT_DIR%
)

if "%PROJECT_TYPE%"=="default" (
    echo ========= Default Build =========
    call Build_Default %PROJECT_DIR%
)

:Exit