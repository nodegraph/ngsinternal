Building
--------

* This project aims to be cross-platform.
* Supported desktop operating systems : windows, linux
* Supported mobile operating systems: android, winrt
* mac os and ios will be coming soon.

Building on Windows
-------------------
* We'll only be describing how to build on windows because it's the easiest.
* We currently use visual studio 2015 with Qt 5.6. 

Requirements
------------

* Make sure you the following already installed.
    * SourceTree from Atlassian (a nice windows gui for git)
    * Visual Studio 2015 Community Edition or Commercial Edition (make to install the Universal Windows Platform Tools)
    * CMake
    * Ninja
    * Eclipse Neon (optional, but if you install it you'll want to update CDT (c++ indexer) to a nightly build, there is a bug in the neon package)

Create the Dev Directory
------------------------

* Create a directory that you will use for all your development.
* Add an environment variable called DEV_ROOT that points to this directory.
* Note on windows, variables are referenced as %DEV_ROOT% with the % character.

Fork The Repos
--------------

* Fork the nodegraphexternal and nodegraphsoftware repos in BitBucket.
* Now clone them locally to your machine, into the %DEV_ROOT% directory.

Setup Your Enivronment
----------------------

* There is a premade env.bat file in nodegraphsoftware/scripts/windows, which will setup your development environment.
* It uses the %DEV_ROOT% enviroment variable you setup earlier.
* Create a desktop short which opens a windows shell (cmd) and runs this script.
* In the shortcut gui, the "target" should be set to the following: 
* %comspec% /k ""%DEV_ROOT%\nodegraphsoftware\scripts\windows\env.bat""

Create Your IDE Project
-----------------------

* Double click the desktop shortcut to start a shell with our enviroment.
* Start "cmake-gui" from it.
* The source directory should point to the "nodegraphsoftware" directory.
* Name the build dir, "nodegraphsoftware-build" and then press "configure".
* It should ask which IDE/builder to generate files for. Choose one of:
    * Eclipse CDT4 - Ninja
    * Visual Studo 12 2013 Win64
* Next it will show some errors.
* Just change the "ARCH" variable to "DESKTOP_WINDOWS".
* Hit the "configure" button again. It should succeed.
* Now use visual studio or eclipse to open the generated project files in the "nodegraphsoftware-build" directory.


