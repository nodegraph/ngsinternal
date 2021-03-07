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
         
	# gafferDependencies
	self.add_path(Env.BIN,"/home/raindrop/gaffer-dev/gafferDependencies-0.19.0.0-linux/bin")
	self.add_path(Env.LIB,"/home/raindrop/gaffer-dev/gafferDependencies-0.19.0.0-linux/lib")

	# build gaffer as follows;
	# scons BUILD_DIR=/home/raindrop/gaffer-dev/gafferDependencies-0.19.0.0-linux RMAN_ROOT=/home/raindrop/apps/3delight-12.0.19/Linux-x86_64 ARNOLD_ROOT=/home/raindrop/apps/arnold-4.2.11.0 build --jobs=48

	self.aliases['build_gaffer']='scons BUILD_DIR=/home/raindrop/gaffer-dev/gafferDependencies-0.19.0.0-linux RMAN_ROOT=/home/raindrop/apps/3delight-12.0.19/Linux-x86_64 ARNOLD_ROOT=/home/raindrop/apps/arnold-4.2.11.0'
	# arnold
	self.add_path(Env.BIN,"/home/raindrop/apps/arnold-4.2.11.0/bin")
	self.add_path(Env.LIB,"/home/raindrop/apps/arnold-4.2.11.0/bin")
	self.add_path(Env.PYT,"/home/raindrop/apps/arnold-4.2.11.0/python")

# This creates and outputs the bash script to the shell.
env = DesktopEnv()
env.get_script()

