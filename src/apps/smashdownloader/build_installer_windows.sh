# To create the repo and installer do the following.
# The installer will be in the package dir.

# source build_installer_windows.sh package [release]
# source build_installer_windows.sh create_repo
# source build_installer_windows.sh create_installer



NGSINTERNAL=/d/src/ngsinternal/src/apps/smashdownloader/installer
INSTALL=/d/b1/install

PACKAGES_ROOT=/d/smashdownloader_pack
PACKAGES=$PACKAGES_ROOT/packages
REPOSITORY=/d/smashdownloader_repo

if [ $# -eq 0 ]
  then
    echo "one of the following arguments required: package, create_repo, update_repo or create_installer"
fi

if [[ -z "$2" && ($2 = "release") ]]; then
	RELEASE=1
else 
	RELEASE=0
fi

if [ $1 = "package" ]; then
	package
elif [ $1 = "create_repo" ]; then
	create_repo
elif [ $1 = "update_repo" ]; then
	update_repo
elif [ $1 = "create_installer" ]; then
	create_installer
else
  echo "incorrect arguments given."
fi

# -------------------------------------------------------------------------
# Create our packages.
# -------------------------------------------------------------------------
package ()
{
	echo "packaging.."
	rm -fr $PACKAGES_ROOT

	# Go to the install dir created from ninja install.
	cd $INSTALL;
	
	# Create the package dirs.
	mkdir -p $PACKAGES/com.smashdownloader.primary/data/bin
	mkdir -p $PACKAGES/com.smashdownloader.secondary/data/bin
	
	# Most dirs correspond one to one with their packages.
	# However the bin dir is split into 2 packages.
	cd bin
	
	# The primary is our ngs libraries and executables.
	cp -fr +(smash*|jcomm.jar|ngs*|test*) $PACKAGES/com.smashdownloader.primary/data/bin
	
	# The secondary is the third party libraries and executables.
	cp -fr !(smash*|jcomm.jar|ngs*|test*) $PACKAGES/com.smashdownloader.secondary/data/bin
	
	# Now copy in the xml files from ngsinternal.
	cp -fr $NGSINTERNAL/. $PACKAGES_ROOT
	
	# Modify the config.xml with property repository url.
	if [ $RELEASE -eq 1 ]; then
		sed -i -e 's/REPOSITORY_URL/https:\/\/www.smashdownloader.com\/repository/g' $PACKAGES_ROOT/config/config.xml
	else
		sed -i -e 's/REPOSITORY_URL/file:\/\/\/D:\/smashdownloader_repo/g' $PACKAGES_ROOT/config/config.xml
	fi
}

# -------------------------------------------------------------------------
# Create our repository.
# -------------------------------------------------------------------------
create_repo ()
{
	echo "creating repo.."
	rm -fr $REPOSITORY
	cd $PACKAGES_ROOT
	repogen -p packages $REPOSITORY
}

# -------------------------------------------------------------------------
# Update our repository.
# -------------------------------------------------------------------------
update_repo ()
{
	echo "updating repo.."
	cd $PACKAGES_ROOT
	repogen --update-new-components -p packages $REPOSITORY
}

# -------------------------------------------------------------------------
# Create our installer.
# -------------------------------------------------------------------------
create_installer ()
{
	echo "creating installer.."
	cd $PACKAGES_ROOT
	binarycreator --online-only -c 'config/config.xml' -p packages smashdownloader
}

