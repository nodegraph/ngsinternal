# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source ~/src/ngsinternal/src/scripts/msys2/build_macos_git_pull.sh"
#}

echo "MACOS GIT PULL"
cd ~/src/ngsinternal
git pull origin master
