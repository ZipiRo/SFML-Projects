@echo off
setlocal

set PROJECT_NAME=%1
set TEMPLATE_TYPE=%2

mkdir %PROJECT_NAME%
robocopy /MIR Templates\%TEMPLATE_TYPE% %PROJECT_NAME% > nul