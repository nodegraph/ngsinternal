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

rem - launch the visual studio native 64 bit shell
rem - if you want to use the 32 to 64 bit cross compiler, use x86_amd64
rem - call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64

if "%1"=="x64" (
	echo "setting up vs2015 for 64 bits"
	set ARCH_BITS=x64
	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
) 
if "%1"=="x86" (
	echo "setting up vs2015 for 32 bits"
	set ARCH_BITS=x86
 	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
)
if [%1] == [] echo "Error: ARCH_BITS was not set!"


rem - add our msys2 scripts to the path.
rem - you can start it with msys2_shell.bat
rem - then you can use our setup_env.sh script as follows:
rem - source setup_env.sh ARCH_XXX 

set PATH="C:\Program Files\Git";%PATH%
set PATH="C:\Program Files\Git\bin;";%PATH%

set PATH="D:\installs\windowsunpack\ninja-win";%PATH%

set PATH="C:\Program Files (x86)\CMake\bin";%PATH%

set PATH=D:\installs\windows\msys2;%PATH%
set PATH=D:\installs\windows\Perl64\bin;%PATH%

set PATH=D:\installs\windows\QtIFW2.0.3\bin;%PATH%

rem set PATH="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin";%PATH%
rem set PATH="D:\installs\windowsunpack\nasm-2.12.01";%PATH%


