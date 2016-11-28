# Jenkins Pipeline Definition.
#node {
#    bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_WINDOWS x64 D:\\src\\ngsinternal\\src\\scripts\\msys2\\build_window_release_x64.sh"
#}

echo "WINDOWS RELEASE X64";
export ARCH=ARCH_WINDOWS;
export ARCH_BITS=x64;
source /d/src/ngsinternal/src/scripts/msys2/setup_env.sh;
rm -fr /d/wr64;
mkdir /d/wr64;
cd /d/wr64;
git clone git@gitlab.com:nodegraph/ngsinternal.git;
mkdir build;
cd build;
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ../ngsinternal/src;
ninja install;
