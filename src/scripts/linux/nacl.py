from env import Env

class PNaClEnv(Env):
    def __init__(self):
        super(PNaClEnv,self).__init__()

    def setup_aliases(self):
        #self.aliases['dev']='cd /usr/hobby/dev-android'
        pass

    def setup_vars(self):
	self.vars["NACL_PEPPER_VERSION"]="41"
	self.vars["NACL_HOST"]="linux_x86_glibc"  # linux_x86_glibc, linux_pnacl
	self.vars["NACL_BIN_PREFIX"]="x86_64"  # x86_64-nacl, pnacl
	self.vars["NACL_SDK_ROOT"]="/home/raindrop/dev2/nacl_sdk/pepper_"+self.vars["NACL_PEPPER_VERSION"]

    # Compiler vars.
    def setup_compiler(self):
        # compiler flags -- we use the system's compilers.

        ## setup extra compiler aliases
        self.aliases['g++']=self.vars['NACL_BIN_PREFIX']+'-g++'   # -clang++
        self.aliases['gcc']=self.vars['NACL_BIN_PREFIX']+'-gcc'   # '-clang
        self.aliases['ar']=self.vars['NACL_BIN_PREFIX']+'-ar'
        self.aliases['ranlib']=self.vars['NACL_BIN_PREFIX']+'-ranlib'
        self.aliases['ld']=self.vars['NACL_BIN_PREFIX']+'-ld'
        self.aliases['python']='python2.7'

        self.vars["CXX"]='"%s"' % self.aliases['g++']
        self.vars["CC"]='"%s"' % self.aliases['gcc']
        self.vars["LD"]='"%s"' % self.aliases['ld']
        self.vars["AR"]='"%s"' % self.aliases['ar']

        self.vars["LDFLAGS"]="\" --sysroot="+self.vars["NACL_SDK_ROOT"]+" -stdlib=libstdc++ -std=gnu++11\""  #-stdlib=libc++ 
        self.vars["CFLAGS"]=""
        self.vars["CXXFLAGS"]="\" --sysroot="+self.vars["NACL_SDK_ROOT"]+" -stdlib=libstdc++ -std=gnu++11\""

    def setup_paths(self):
        # cent os system executables.
        #self.add_path(Env.BIN,"/bin:/usr/bin")
	self.add_path(Env.BIN,"/usr/local/bin:/bin:/usr/bin:/usr/local/sbin:/usr/sbin")

	# qt - for native linux - we need this for cmake-gui below.
	# qt uses freetype from above
	# qt also needs python 2.7 to generate some files, so we need to build this early before we build python below.
	# note for qt, LD and ld must be aliased to g++, or just don't alias it at all.
	# ./configure -prefix /home/raindrop/dev/local/qt-everywhere-opensource-src-5.4.0
	# make -j8
	# make install
	# make module-qtscript
	# cd qtscript; make install
        self.add_path(Env.BIN, OPENSHAPESENV_LOCAL_DIR+"/qt-everywhere-opensource-src-5.4.0/bin")
        self.add_path(Env.INC, OPENSHAPESENV_LOCAL_DIR+"/qt-everywhere-opensource-src-5.4.0/include")
	# the following lets PythonQt without modifications
        self.add_path(Env.INC, OPENSHAPESENV_LOCAL_DIR+"/qt-everywhere-opensource-src-5.4.0/include/QtPrintSupport")         
        self.add_path(Env.LIB, OPENSHAPESENV_LOCAL_DIR+"/qt-everywhere-opensource-src-5.4.0/lib")
        self.add_path(Env.PKG, OPENSHAPESENV_LOCAL_DIR+"/qt-everywhere-opensource-src-5.4.0/lib/pkgconfig")
        self.vars["QTDIR"]=OPENSHAPESENV_LOCAL_DIR+"/qt-everywhere-opensource-src-5.4.0"

	# QtCreator
	# at the top level dir do:
	# qmake -r
	# make install INSTALL_ROOT=/home/raindrop/dev/local/qt-creator-opensource-src-3.3.0 -j8
        self.add_path(Env.BIN,OPENSHAPESENV_LOCAL_DIR+"/qt-creator-opensource-src-3.3.0/bin")
        self.add_path(Env.INC,OPENSHAPESENV_LOCAL_DIR+"/qt-creator-opensource-src-3.3.0/include")
        self.add_path(Env.LIB,OPENSHAPESENV_LOCAL_DIR+"/qt-creator-opensource-src-3.3.0/lib")

	# cmake
	# ./configure --prefix=/home/raindrop/dev/local/cmake-3.0.2 --qt-gui
        self.add_path(Env.BIN, OPENSHAPESENV_LOCAL_DIR+"/cmake-3.0.2/bin")

        # nacl bin dir
        self.add_path(Env.BIN,self.vars["NACL_SDK_ROOT"]+"/toolchain/"+self.vars["NACL_HOST"]+"/bin")

	# colorama - build for native linux usage - required to build naclposts
	# python setup.py install --prefix=/home/raindrop/dev2/local/colorama-0.3.3
	self.add_path(Env.PYT,"/home/raindrop/dev2/local/colorama-0.3.3/lib/python2.7/site-packages")

	# naclports
	# -- building boost also builds: glibc-compat, zlib, bzip2
	# -- modify lib/naclports/util.py -->  MIN_SDK_VERSION = 39  --> it comes in at 42
	# -- for glibc modify ports/boost/build.sh --> comment out  "link=static" in build args --> then both the static and shared should build.

	## -- for 32bit use i686
	## export NACL_SHARED=1
	## export NACL_GLIBC=1   <----- this will build the shared libraries
	#export NACL_SDK_ROOT=/home/raindrop/dev2/nacl_sdk/pepper_43
	#export NACL_ARCH=x86_64 
	#export NACL_LIBC=glibc
	#export TOOLCHAIN=glibc
	#export OS_JOBS=8
	#export PYTHONPATH=/home/raindrop/dev2/local/colorama-0.3.3/lib/python2.7/site-packages

	## for pnacl
	#export NACL_SDK_ROOT=/home/raindrop/dev2/nacl_sdk/pepper_43
	#export NACL_ARCH=pnacl
	#export TOOLCHAIN=pnacl
	#export OS_JOBS=8
	#export PYTHONPATH=/home/raindrop/dev2/local/colorama-0.3.3/lib/python2.7/site-packages

	## -- note rebuild all dependencies use --force-all
	## -- to rebuild just your specified target use --force
	## ./bin/naclports install boost --force-all
	## -- building freetype also builds: libpng 
	## ./bin/naclports install freetype --force-all

	## build boost
	## you will need to run ranlib on the pnacl version and possible others.
	## cd /home/raindrop/dev2/nacl_sdk/pepper_43/toolchain/linux_pnacl/le32-nacl/usr/lib
	## /home/raindrop/dev2/nacl_sdk/pepper_43/toolchain/linux_pnacl/bin/pnacl-ranlib *.a

	## install 32 system libraries for python-host, which is built with 32 bits.
	#yum install zlib-devel.i686
	#yum install readline-devel.i686
	#yum install bzip2-devel.i686
	#yum install openssl-devel.i686

	## now build the python libs in naclports
	#make python-host FORCE=1 V=1
	#make python-static FORCE=1 V=1
	#make boost FORCE=1 V=1
	#make freetype FORCE=1 V=1

	## for pnacl i did, before python-static
	## but it didn't seem to matter, it reports failing to build bzip2, readline etc for pnacl.
	## it's ok we don't use those anyway. it seems like naclports is configured on purpose this way.
	#make ncurses FORCE=1 V=1
	#make zlib FORCE=1 V=1
	#make readline FORCE=1 V=1
	#make glibc-compat FORCE=1 V=1
	#make libpng FORCE=1 V=1
	#make freetype FORCE=1 V=1
	#make nacl-spawn FORCE=1 V=1
	#make python-static FORCE=1 V=1



	## install selenium for nacl
	#PYTHONHOME=/home/raindrop/dev2/naclports/out/build/python-host/install_host
	#PATH=/home/raindrop/dev2/naclports/out/build/python-host/install_host/bin:$PATH
	#export PYTHONPATH=/home/raindrop/dev2/nacl_sdk/pepper_41/toolchain/linux_x86_glibc/x86_64-nacl/usr/extras/lib/python2.7/site-packages/
	#python setup.py install --prefix=/home/raindrop/dev2/nacl_sdk/pepper_41/toolchain/linux_x86_glibc/x86_64-nacl/usr/extras

	## install selenium for pnacl
	#PYTHONHOME=/home/raindrop/dev2/naclports/out/build/python-host/install_host
	#PATH=/home/raindrop/dev2/naclports/out/build/python-host/install_host/bin:$PATH
	#export PYTHONPATH=/home/raindrop/dev2/nacl_sdk/pepper_41/toolchain/linux_pnacl/le32-nacl/usr/extras/lib/python2.7/site-packages/
	#python setup.py install --prefix=/home/raindrop/dev2/nacl_sdk/pepper_41/toolchain/linux_pnacl/le32-nacl/usr/extras

	# zlib
	self.add_path(Env.INC,"/home/raindrop/dev2/local/zlib_1.2.8/include")
	self.add_path(Env.LIB,"/home/raindrop/dev2/local/zlib_1.2.8/lib")
	self.add_path(Env.PKG,"/home/raindrop/dev2/local/zlib_1.2.8/lib/pkgconfig")
	
	# png
	self.add_path(Env.INC,"/home/raindrop/dev2/local/libpng_1.6.12/include")
	self.add_path(Env.LIB,"/home/raindrop/dev2/local/libpng_1.6.12/lib")
	self.add_path(Env.PKG,"/home/raindrop/dev2/local/libpng_1.6.12/lib/pkgconfig")

	# bzip2
	self.add_path(Env.INC,"/home/raindrop/dev2/local/bzip2_1.0.6/include")
	self.add_path(Env.LIB,"/home/raindrop/dev2/local/bzip2_1.0.6/lib")	

	# freetype
	self.add_path(Env.INC,"/home/raindrop/dev2/local/freetype_2.5.3/include")
	self.add_path(Env.LIB,"/home/raindrop/dev2/local/freetype_2.5.3/lib")
	self.add_path(Env.PKG,"/home/raindrop/dev2/local/freetype_2.5.3/lib/pkgconfig")

	# boost
        self.add_path(Env.INC,"/home/raindrop/dev2/local/boost_1_55_0/include")
        self.add_path(Env.LIB,"/home/raindrop/dev2/local/boost_1_55_0/lib")

# This creates and outputs the bash script to the shell.
env = PNaClEnv()
env.get_script()

