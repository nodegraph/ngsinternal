
NGSINTERNAL=/d/src/ngsinternal/src/apps/smashbrowse/installer
INSTALL=/d/b1/install

PACKAGES_ROOT=/d/temp/
PACKAGES=$PACKAGES_ROOT/packages
REPOSITORY=/d/repositories/smashbrowse

if [ $# -eq 0 ]
  then
    echo "one of the following arguments required: package, update_repo or create_installer"
fi

if [ $1 = "package" ]; then
	echo "packaging.."
	package
elif [ $1 = "update_repo" ]; then
	echo "updating repo.."
	update_repo
elif [ $1 = "create_installer" ]; then
	echo "creating installer.."
	create_installer
else
  echo "incorrect arguments given."
fi

# -------------------------------------------------------------------------
# Create our packages.
# -------------------------------------------------------------------------
package ()
{
	# Go to the install dir created from ninja install.
	cd $INSTALL;
	
	# Create the package dirs.
	mkdir -p $PACKAGES/com.smashbrowse.chromeextension/data
	mkdir -p $PACKAGES/com.smashbrowse.gson/data
	mkdir -p $PACKAGES/com.smashbrowse.html/data
	mkdir -p $PACKAGES/com.smashbrowse.jre/data
	mkdir -p $PACKAGES/com.smashbrowse.selenium/data
	mkdir -p $PACKAGES/com.smashbrowse.primary/data/bin
	mkdir -p $PACKAGES/com.smashbrowse.secondary/data/bin
	
	# Most dirs correspond one to one with their packages.
	cp -fr chromeextension $PACKAGES/com.smashbrowse.chromeextension/data
	cp -fr gson $PACKAGES/com.smashbrowse.gson/data
	cp -fr html $PACKAGES/com.smashbrowse.html/data
	cp -fr jre $PACKAGES/com.smashbrowse.jre/data
	cp -fr selenium $PACKAGES/com.smashbrowse.selenium/data
	
	
	# However the bin dir is split into 2 packages.
	cd bin
	
	# The primary is our ngs libraries and executables.
	cp -fr +(smash*|jcomm.jar|ngs*|test*) $PACKAGES/com.smashbrowse.primary/data/bin
	
	# The secondary is the third party libraries and executables.
	cp -fr !(smash*|jcomm.jar|ngs*|test*) $PACKAGES/com.smashbrowse.secondary/data/bin
	
	# Now copy in the xml files from ngsinternal.
	cp -fr $NGSINTERNAL/. $PACKAGES_ROOT
}

# -------------------------------------------------------------------------
# Update our repository.
# -------------------------------------------------------------------------
update_repo ()
{
	cd $PACKAGES_ROOT
	repogen --update-new-components -p packages $REPOSITORY
}

# -------------------------------------------------------------------------
# Create our installer.
# -------------------------------------------------------------------------
create_installer ()
{
	cd $PACKAGES_ROOT
	binarycreator --online-only -c 'config/config.xml' -p packages smashbrowse
}

