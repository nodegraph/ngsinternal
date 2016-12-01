# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source ~/src/ngsinternal/src/scripts/msys2/build_macos_git_pull.sh"
#}

echo "MACOS DEBUG"
export ARCH=ARCH_MACOS
export ARCH_BITS=x64
source ~/src/ngsinternal/src/scripts/macos/setup_env.sh
cd ~/src/ngsinternal
git pull origin master
