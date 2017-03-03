# To create the repo and installer do the following.
# The installer will be in the package dir.

# smashbrowse_installer_windows.sh package [release/debug]
# smashbrowse_installer_windows.sh create_repo [release/debug]
# smashbrowse_installer_windows.sh create_installer [release/debug]

# Activate bash globbing syntax.
shopt -s extglob

PACK_STRUCTURE=/d/src/ngsinternal/src/apps/smashbrowse/installer
BUNDLE=/d/b1/install

PACK=/d/smashbrowse_pack
REPO=/d/smashbrowse_repo



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
	mkdir -p ${PACK}/packages/com.smashbrowse.chromeextension/data
	mkdir -p ${PACK}/packages/com.smashbrowse.gson/data
	mkdir -p ${PACK}/packages/com.smashbrowse.html/data
	mkdir -p ${PACK}/packages/com.smashbrowse.jre/data
	mkdir -p ${PACK}/packages/com.smashbrowse.selenium/data
	mkdir -p ${PACK}/packages/com.smashbrowse.primary/data/bin
	mkdir -p ${PACK}/packages/com.smashbrowse.secondary/data/bin
	
	# Most dirs correspond one to one with their packages.
	cp -fr chromeextension ${PACK}/packages/com.smashbrowse.chromeextension/data
	cp -fr gson ${PACK}/packages/com.smashbrowse.gson/data
	cp -fr html ${PACK}/packages/com.smashbrowse.html/data
	cp -fr jre ${PACK}/packages/com.smashbrowse.jre/data
	cp -fr selenium ${PACK}/packages/com.smashbrowse.selenium/data
	
	
	# However the bin dir is split into 2 packages.
	cd bin
	
	# The primary is our ngs libraries and executables.
	cp -fr +(smash*|jcomm.jar|ngs*|test*) ${PACK}/packages/com.smashbrowse.primary/data/bin
	
	# The secondary is the third party libraries and executables.
	cp -fr !(smash*|jcomm.jar|ngs*|test*) ${PACK}/packages/com.smashbrowse.secondary/data/bin
	
	# Now copy in the xml files from ngsinternal.
	cp -fr $PACK_STRUCTURE/. $PACK
	
	# Modify the config.xml with property repository url.
	if [ $RELEASE -eq 1 ]; then
		sed -i -e 's/REPOSITORY_URL/http:\/\/www.smashbrowse.com\/windows\/smashbrowse_repo_release/g' $PACK/config/config.xml
	else
		sed -i -e 's/REPOSITORY_URL/file:\/\/\/D:\/smashbrowse_repo_debug/g' $PACK/config/config.xml
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
	binarycreator --online-only -c 'config/config.xml' -p packages smashbrowse
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

