from env import Env 

class AndroidEnv(Env):
    def __init__(self):
        super(AndroidEnv,self).__init__()

    def setup_aliases(self):
        pass

    def setup_vars(self):
        self.vars["ANDROID_SDK_ROOT"]="/home/raindrop/devandroid/local/android-sdk-linux"
        self.vars["ANDROID_NDK_ROOT"]="/home/raindrop/devandroid/local/android-ndk-r10e"
        self.vars["ANDROID_API_LEVEL"]="android-21"
        self.vars["ANDROID_TARGET_ARCH"]="arch-arm"
        self.vars["ANDROID_SYS_ROOT"]=self.vars["ANDROID_NDK_ROOT"]+"/platforms/"+self.vars["ANDROID_API_LEVEL"]+"/"+self.vars["ANDROID_TARGET_ARCH"]
        self.vars["QTDIR"]="/home/raindrop/devandroid/local/Qt5.5.0/5.5/android_armv7"

    def setup_compiler(self):
        # setup compiler aliases
        self.aliases['g++']='arm-linux-androideabi-g++ --sysroot='+self.vars["ANDROID_SYS_ROOT"]+" "
        self.aliases['cxx']=self.aliases['g++']
        self.aliases['c++']=self.aliases['g++']
        self.aliases['gcc']='arm-linux-androideabi-gcc --sysroot='+self.vars["ANDROID_SYS_ROOT"]+" "
        self.aliases['cc']=self.aliases['gcc']
        self.aliases['ar']='arm-linux-androideabi-ar --sysroot='+self.vars["ANDROID_SYS_ROOT"]+" "
        self.aliases['ranlib']='arm-linux-androideabi-ranlib --sysroot='+self.vars["ANDROID_SYS_ROOT"]+" "
        self.aliases['ld']='arm-linux-androideabi-ld --sysroot='+self.vars["ANDROID_SYS_ROOT"]+" "
        self.aliases['python']='python2.7'

	# setup env vars
        self.vars["CXX"]='"%s"' % self.aliases['g++']
        self.vars["CC"]='"%s"' % self.aliases['gcc']
        self.vars["LD"]='"%s"' % self.aliases['ld']
        self.vars["LDFLAGS"]="" #"-lgnustl_shared"
        self.vars["CFLAGS"]="" #"-lgnustl_shared"
        self.vars["CXXFLAGS"]="" #"-lgnustl_shared"

    def setup_paths(self):

        # cent os system executables.
        self.add_path(Env.BIN,"/bin:/usr/bin")

	# gl and other header includes.
	#self.add_path(Env.INC, self.vars["ANDROID_SYS_ROOT"]+"/usr/include")

	# GCC 4.9.3 
	# cmake needs the std++ lib from this.
        self.add_path(Env.LIB,"/home/raindrop/dev/local/gcc-4.9.3/lib64")

        # cmake 3.3.1
        self.add_path(Env.BIN,"/home/raindrop/dev/local/cmake-3.3.1/bin")
	# cmake-gui needs qt libs to run
	self.add_path(Env.LIB, "/home/raindrop/dev/local/Qt5.5.0/5.5/gcc_64/lib")

	# git 2.5.0
	self.add_path(Env.BIN,"/home/raindrop/dev/local/git-2.5.0/bin")

        # QtCreator from Qt5.5.0
        self.add_path(Env.BIN,"/home/raindrop/dev/local/Qt5.5.0/Tools/QtCreator/bin")

        # android ndk tools
        self.add_path(Env.BIN,"/home/raindrop/devandroid/local/android-ndk-r10e")

        # android make and python
        self.add_path(Env.BIN,"/home/raindrop/devandroid/local/android-ndk-r10e/prebuilt/linux-x86_64/bin")

        # android sdk tools like adb
        self.add_path(Env.BIN,"/home/raindrop/devandroid/local/android-sdk-linux/platform-tools")
	# GLEW
	self.add_path(Env.INC,"/home/raindrop/devandroid/local/android-ndk-r10e/platforms/android-21/arch-arm/usr/include")

        # android compilers
        self.add_path(Env.BIN,"/home/raindrop/devandroid/local/android-ndk-r10e/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin")

        # android stl
        self.add_path(Env.INC,"/home/raindrop/devandroid/local/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/include")
        self.add_path(Env.INC,"/home/raindrop/devandroid/local/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include")
        self.add_path(Env.INC,"/home/raindrop/devandroid/local/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include/tr1")

	# android stl libs
        self.add_path(Env.LIB,"/home/raindrop/devandroid/local/android-ndk-r10e/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a")

        # android native apps
        self.add_path(Env.INC,"/home/raindrop/devandroid/local/android-ndk-r10e/sources/android/native_app_glue")


	# zlib
	# ./configure --prefix=/home/raindrop/devandroid/local/zlib-1.2.8
	# After installing you need to make sure only libz.so exists in the lib not libz.so.1 but qtandroiddeploy won't take libs with numbers on the end.
        # self.add_path(Env.INC,"/home/raindrop/devandroid/local/zlib-1.2.8/include")
        # self.add_path(Env.LIB,"/home/raindrop/devandroid/local/zlib-1.2.8/lib")

	# libpng
	# ./configure --prefix=/home/raindrop/devandroid/local/libpng-1.6.18 --host=arm-linux-androideabi 
        # self.add_path(Env.BIN,"/home/raindrop/devandroid/local/libpng-1.6.18/bin")
       	# self.add_path(Env.INC,"/home/raindrop/devandroid/local/libpng-1.6.18/include")
        # self.add_path(Env.LIB,"/home/raindrop/devandroid/local/libpng-1.6.18/lib")
        # self.add_path(Env.PKG,"/home/raindrop/devandroid/local/libpng-1.6.18/lib/pkgconfig")

        # freetype-2.5.2
	# we use this version because it doesn't use harfbuzz another library we would have to compile
	# starting from 2.5.3 it will harfbuzz
	#  ./configure --prefix=/home/raindrop/devandroid/local/freetype-2.5.2 --host=arm-linux-androideabi --without-png --without-zlib --enable-shared
	# on linux, add this step:
	# sed -i 's/\-version\-info \$(version_info)/-avoid-version/' builds/unix/unix-cc.mk
	# if you are on a mac use the following line instead of the one above
	# sed -i '.orig' 's/\-version\-info \$(version_info)/-avoid-version/' builds/unix/unix-cc.mk

        self.add_path(Env.BIN,"/home/raindrop/devandroid/local/freetype-2.5.2/bin")
        self.add_path(Env.INC,"/home/raindrop/devandroid/local/freetype-2.5.2/include")
        self.add_path(Env.LIB,"/home/raindrop/devandroid/local/freetype-2.5.2/lib")

	# glew is not needed on android 

        # qt 5.5
        self.add_path(Env.BIN,"/home/raindrop/devandroid/local/Qt5.5.0/5.5/android_armv7/bin")
        self.add_path(Env.INC,"/home/raindrop/devandroid/local/Qt5.5.0/5.5/android_armv7/include")
        self.add_path(Env.LIB,"/home/raindrop/devandroid/local/Qt5.5.0/5.5/android_armv7/lib")

# This creates and outputs the bash script to the shell.
env = AndroidEnv()
env.get_script()

