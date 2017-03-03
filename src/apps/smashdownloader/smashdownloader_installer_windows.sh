# To create the repo and installer do the following.
# The installer will be in the package dir.

# smashdownloader_installer_windows.sh package [release/debug]
# smashdownloader_installer_windows.sh create_repo [release/debug]
# smashdownloader_installer_windows.sh create_installer [release/debug]

# Activate bash globbing syntax.
shopt -s extglob

PACK_STRUCTURE=/d/src/ngsinternal/src/apps/smashdownloader/installer
BUNDLE=/d/b1/install

PACK=/d/smashdownloader_pack
REPO=/d/smashdownloader_repo



# -------------------------------------------------------------------------
# Create our packages.
# -------------------------------------------------------------------------
package ()
{
	echo "packaging.."
	rm -fr $PACK

	# Go to the install dir created from ninja install.
	cd $BUNDLE;
	
	# Create the package dirs.
	mkdir -p ${PACK}/packages/com.smashdownloader.primary/data/bin
	mkdir -p ${PACK}/packages/com.smashdownloader.secondary/data/bin
	
	# Most dirs correspond one to one with their packages.
	# However the bin dir is split into 2 packages.
	cd bin
	
	# The primary is our ngs libraries and executables.
	cp -fr +(smash*|jcomm.jar|ngs*|test*) ${PACK}/packages/com.smashdownloader.primary/data/bin
	
	# The secondary is the third party libraries and executables.
	cp -fr !(smash*|jcomm.jar|ngs*|test*) ${PACK}/packages/com.smashdownloader.secondary/data/bin
	
	# Now copy in the xml files from ngsinternal.
	cp -fr $PACK_STRUCTURE/. $PACK
	
	# Modify the config.xml with property repository url.
	if [ $RELEASE -eq 1 ]; then
		sed -i -e 's/REPOSITORY_URL/https:\/\/www.smashdownloader.com\/repository/g' $PACK/config/config.xml
	else
		sed -i -e 's/REPOSITORY_URL/file:\/\/\/D:\/smashdownloader_repo_debug/g' $PACK/config/config.xml
	fi
}

# -------------------------------------------------------------------------
# Create our repository.
# -------------------------------------------------------------------------
create_repo ()
{
	echo "creating repo.."
	rm -fr $REPO
	cd $PACK
	repogen -p packages $REPO
}

# -------------------------------------------------------------------------
# Update our repository.
# -------------------------------------------------------------------------
update_repo ()
{
	echo "updating repo.."
	cd $PACK
	repogen --update-new-components -p packages $REPO
}

# -------------------------------------------------------------------------
# Create our installer.
# -------------------------------------------------------------------------
create_installer ()
{
	echo "creating installer.."
	cd $PACK
	binarycreator --online-only -c 'config/config.xml' -p packages smashdownloader
}

# -------------------------------------------------------------------------
# Main Logic.
# -------------------------------------------------------------------------
if [ "$#" -ne 2 ]; then
    echo "2 arguments are required: "
    echo "[1]: package, create_repo, update_repo or create_installer"
    echo "[2]: debug, release"
    exit 1
fi

if [ $2 = "release" ]; then
	RELEASE=1
	PACK=${PACK}_release
	REPO=${REPO}_release
else 
	RELEASE=0
	PACK=${PACK}_debug
	REPO=${REPO}_debug
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
