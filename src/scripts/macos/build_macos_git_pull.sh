# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source ~/src/ngsinternal/src/scripts/macos/build_macos_git_pull.sh"
#}

echo "MACOS GIT PULL"
cd ~/src/ngsinternal
git pull origin master
