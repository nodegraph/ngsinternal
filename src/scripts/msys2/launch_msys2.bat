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

rem - this batch file takes the following arguments in the order specified
rem - 1) the ARCH as an argument.
rem - 2) the hardware bits 32 or 64
rem - 3) (optional) the script file to run in msys2 environment. If no file
rem      is specified then an interactive msys2 shell will be launched.
rem - 4) (optional) the arguments to the script file to run 

rem - load the regular windows env setup batch file
call %~dp0/../windows/env.bat %2

set ARCH=%1

rem - now start up msys
rem - call D:\installs\android\MinGW\msys\1.0\msys.bat
rem - call D:\installs\windows\msys2\msys2_shell.bat

if "%3"=="" goto INTERACTIVE

rem - otherwise we run the script instead of launching an interactive shell
D:\installs\windows\msys2\usr\bin\bash.exe -f %3 %4
echo "Done building in msys2 environment";
goto DONE

:INTERACTIVE
call D:\installs\windows\msys2\msys2_shell.bat

:DONE
