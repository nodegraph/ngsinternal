from env import Env

class DesktopEnv(Env):
    def __init__(self):
        super(DesktopEnv,self).__init__()

    def setup_aliases(self):
        pass

    def setup_vars(self):
        pass

    # Compiler vars.
    def setup_compiler(self):
        # compiler version flags -- we use the system's compilers.
        self.vars["CXX"]="g++"
        self.vars["CC"]="gcc"
        # self.vars["LD"]="g++"

        self.vars["LDFLAGS"]=""
        self.vars["CFLAGS"]=""
        self.vars["CXXFLAGS"]=""

        # setup extra compiler aliases
        self.aliases['g++']='g++'
        self.aliases['gcc']='gcc'
        self.aliases['ar']='ar'
        self.aliases['ranlib']='ranlib'
        # self.aliases['ld']='g++'
        # self.aliases['python']='python3'

    def setup_paths(self):

        # cent os system executables.
        self.add_path(Env.BIN,"/usr/local/bin:/bin:/usr/bin:/usr/local/sbin:/usr/sbin")
        self.add_path(Env.LIB,"/usr/lib")
        self.add_path(Env.LIB,"/usr/lib64")

	# GMP 6.0.0
	# ./configure --prefix=/home/raindrop/dev/local/gmp-6.0.0
        self.add_path(Env.INC,"/home/raindrop/dev/local/gmp-6.0.0/include")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/gmp-6.0.0/lib")

	# MPFR 3.1.3
	# ./configure --prefix=/home/raindrop/dev/local/mpfr-3.1.3
        self.add_path(Env.INC,"/home/raindrop/dev/local/mpfr-3.1.3/include")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/mpfr-3.1.3/lib")

	# MPC 1.0.3
	# ./configure --prefix=/home/raindrop/dev/local/mpc-1.0.3
        self.add_path(Env.INC,"/home/raindrop/dev/local/mpc-1.0.3/include")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/mpc-1.0.3/lib")

	# GCC 4.9.3
	# ./configure --prefix=/home/raindrop/dev/local/gcc-4.9.3 --disable-multilib
	# change some makefiles to have the following
	# GMPLIBS = /home/raindrop/dev/local/mpc-1.0.3/lib/libmpc.so /home/raindrop/dev/local/mpfr-3.1.3/lib/libmpfr.so /home/raindrop/dev/local/gmp-6.0.0/lib/libgmp.so
	self.add_path(Env.BIN,"/home/raindrop/dev/local/gcc-4.9.3/bin")
        self.add_path(Env.INC,"/home/raindrop/dev/local/gcc-4.9.3/include")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/gcc-4.9.3/lib")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/gcc-4.9.3/lib64")

	# cmake
	# ./configure --prefix=/home/raindrop/dev/local/cmake-3.3.1 --qt-gui
	self.add_path(Env.BIN,"/home/raindrop/dev/local/cmake-3.3.1/bin")

	# git 2.5.0
	# ./configure --prefix=/home/raindrop/dev/local/git-2.5.0
        self.add_path(Env.BIN,"/home/raindrop/dev/local/git-2.5.0/bin")
        self.add_path(Env.BIN,"/home/raindrop/dev/local/git-2.5.0/libexec/git-core")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/git-2.5.0/lib64")

	# gdb
	# ./configure --prefix=/home/raindrop/dev/local/gdb-7.9
	self.add_path(Env.BIN,"/home/raindrop/dev/local/gdb-7.9/bin")
        self.add_path(Env.INC,"/home/raindrop/dev/local/gdb-7.9/include")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/gdb-7.9/lib")
         
        # qt 5.5 binaries
        self.add_path(Env.BIN, "/home/raindrop/dev/local/Qt5.5.0/5.5/gcc_64/bin")
        self.add_path(Env.INC, "/home/raindrop/dev/local/Qt5.5.0/5.5/gcc_64/include")
        self.add_path(Env.LIB, "/home/raindrop/dev/local/Qt5.5.0/5.5/gcc_64/lib")
        self.add_path(Env.PKG, "/home/raindrop/dev/local/Qt5.5.0/5.5/gcc_64/lib/pkgconfig")
        self.vars["QTDIR"]="/home/raindrop/dev/local/Qt5.5.0/5.5/gcc_64"

	# qtcreator from qt 5.5 binaries 
        self.add_path(Env.BIN,"/home/raindrop/dev/local/qt-creator-opensource-src-3.3.0/bin")
        self.add_path(Env.INC,"/home/raindrop/dev/local/qt-creator-opensource-src-3.3.0/include")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/qt-creator-opensource-src-3.3.0/lib")

	# glfw 3.1.2
	self.add_path(Env.INC,"/home/raindrop/dev/local/glfw-3.1.2/include")
        self.add_path(Env.LIB,"/home/raindrop/dev/local/glfw-3.1.2/lib")
	
        # glew
	# modify the make file to install in the right location
	# GLEW_PREFIX ?= /home/raindrop/dev/local/glew-1.13.0
	# GLEW_DEST ?= /home/raindrop/dev/local/glew-1.13.0
	# make all
	# make install.all --> this should install GLEWmx libraries also
        self.add_path(Env.INC, "/home/raindrop/dev/local/glew-1.13.0/include")
        self.add_path(Env.LIB, "/home/raindrop/dev/local/glew-1.13.0/lib64")
        self.add_path(Env.PKG, "/home/raindrop/dev/local/glew-1.13.0/lib64/pkgconfig")

        # freetype-2.5.2
	# we use this version because it doesn't use harfbuzz another library we would have to compile
	# starting from 2.5.3 it will harfbuzz
	#  ./configure --prefix=/home/raindrop/dev/local/freetype-2.5.2 --without-png --without-zlib --without-bzip2 --enable-shared
	# on linux, add this step:
	# sed -i 's/\-version\-info \$(version_info)/-avoid-version/' builds/unix/unix-cc.mk
	# if you are on a mac use the following line instead of the one above
	# sed -i '.orig' 's/\-version\-info \$(version_info)/-avoid-version/' builds/unix/unix-cc.mk

        self.add_path(Env.BIN, "/home/raindrop/dev/local/freetype-2.5.2/bin")
        self.add_path(Env.INC, "/home/raindrop/dev/local/freetype-2.5.2/include")
        self.add_path(Env.INC, "/home/raindrop/dev/local/freetype-2.5.2/include/freetype2")
        self.add_path(Env.LIB, "/home/raindrop/dev/local/freetype-2.5.2/lib")
        self.add_path(Env.PKG, "/home/raindrop/dev/local/freetype-2.5.2/lib/pkgconfig")

	# boost
	# we only use header libraries from boost

	# doxygen 1.8.10
	# make the local dir first and cd into it
	# then do: cmake -G "Unix Makefiles" /home/raindrop/dev/doxygen-1.8.10
	# then do: make
	self.add_path(Env.BIN,"/home/raindrop/dev/local/doxygen-1.8.10/bin")
	self.add_path(Env.LIB,"/home/raindrop/dev/local/doxygen-1.8.10/lib")

        ## libSodium
	## git clone https://github.com/jedisct1/libsodium.git
	## ./autogen.sh
	## ./configure --prefix=/home/raindrop/dev/local/libsodium
        #self.add_path(Env.INC, "/home/raindrop/dev/local/libsodium/include")
        #self.add_path(Env.LIB, "/home/raindrop/dev/local/libsodium/lib")

	# open node graph
	self.add_path(Env.BIN, "/home/raindrop/dev/opennodegraph-build/install/bin");
	self.add_path(Env.LIB, "/home/raindrop/dev/opennodegraph-build/install/lib");


# This creates and outputs the bash script to the shell.
env = DesktopEnv()
env.get_script()

