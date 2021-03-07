#!/bin/bash

# This assumes all the binaries have been built.
# Typically this is done by running all the jenkins jobs.

# Generally the site is deployed as follows.
# deploy_site.sh build_site
# deploy_site.sh gather_binaries
# deploy_site.sh deploy_site

echo 'whoami: '
echo `whoami`


SRC_ROOT=/d/src/
WINDOWS_BUILD_ROOT=/d/wr64
MACOS_BUILD_ROOT="//PACMAN/dev/macos/macos_release_jenkins"

build_site () 
{
	cd ${SRC_ROOT}/youmacro_site;
	jekyll build
}

# Copy the repos and installers into the jekyll built site.
gather_binaries ()
{
	# Clean the windows gathering dir.
	rm -fr ${SRC_ROOT}/youmacro_binaries/windows/*
	
	# Clean the macos gathering dir.
	rm -fr ${SRC_ROOT}/youmacro_binaries/macos/*

	# Copy the windows installers.
	cp -fr ${WINDOWS_BUILD_ROOT}/youmacro_pack/youmacro_*.exe ${SRC_ROOT}/youmacro_binaries/windows/.
	
	# Copy the macos installers.
	cp -fr ${MACOS_BUILD_ROOT}/youmacro_pack/youmacro_*.dmg ${SRC_ROOT}/youmacro_binaries/macos/.
}

# Deploy the jekyll site to firebase hosting.
deploy_site ()
{
	cd ${SRC_ROOT}/youmacro_site
	firebase deploy
}

if [ $1 = "build_site" ]; then
	build_site
elif [ $1 = "gather_binaries" ]; then
	gather_binaries
elif [ $1 = "deploy_site" ]; then
	deploy_site
else
  echo "incorrect arguments given."
fi
