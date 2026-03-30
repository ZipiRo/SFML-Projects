@echo off
setlocal

set PROJECT_NAME=%1

mkdir %PROJECT_NAME%
robocopy /MIR Template %PROJECT_NAME% > nul