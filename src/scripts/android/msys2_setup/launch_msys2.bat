@echo off 
rem - this prevents the remarks and commands from showing

rem - -------------------------------------------------------------
rem - you can get the pathname of the batch script with %0
rem - paramater extensions can be applied to this
rem - %~dp0 will return the drive and path to the batch script eg: c:\scripts
rem - %~f0 will return the full pathname eg: c:\scripts\test.cmd
rem - %[number] will return the respective argument passed on the
rem   on the command line when calling this script
rem - -------------------------------------------------------------

rem - this batch file takes the ARCH as an argument.

rem - load the regular windows env setup batch file
call %~dp0/../../windows/env.bat

set ARCH=%1

rem - now start up msys
rem - call D:\installs\android\MinGW\msys\1.0\msys.bat
call D:\installs\windows\msys2\msys2_shell.bat
