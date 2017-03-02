# Activate bash globbing syntax.
shopt -s extglob

NGSINTERNAL=~raindrop/src/ngsinternal/src/apps/smashdownloader/installer
INSTALL=~raindrop/dev/macos/debug/apps/smashdownloader/smashdownloader.app

PACKAGES_ROOT=~raindrop/temp
PACKAGES=$PACKAGES_ROOT/packages
REPOSITORY=~raindrop/repositories/smashdownloader

DATA_PREFIX=data/smashdownloader.app/Contents

if [ $# -eq 0 ]
  then
    echo "one of the following arguments required: package, create_repo, update_repo or create_installer"
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
	mkdir -p $PACKAGES/com.smashdownloader.primary/${DATA_PREFIX}/Resources/
	mkdir -p $PACKAGES/com.smashdownloader.primary/${DATA_PREFIX}/Resources/bin
	mkdir -p $PACKAGES/com.smashdownloader.primary/${DATA_PREFIX}/MacOS/
	mkdir -p $PACKAGES/com.smashdownloader.primary/${DATA_PREFIX}/Frameworks
	
	mkdir -p $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/Resources/
	mkdir -p $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/Resources/bin
	mkdir -p $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/MacOS/
	mkdir -p $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/PlugIns
	mkdir -p $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/Frameworks
	echo 'finished making dirs'
	
	# Most dirs correspond one to one with their packages.
	# All the other files are split into 2 packages.
	# The primary is our ngs libraries and executables.
	# The secondary is the third party libraries and executables.
	
	cd Contents/Resources
	cp -fr qml $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/Resources
	cp -fr qt.conf $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/Resources
	cd bin
	cp -fr +(jcomm.jar|ngs_*) $PACKAGES/com.smashdownloader.primary/${DATA_PREFIX}/Resources/bin
	cp -fr !(jcomm.jar|ngs_*) $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/Resources/bin
	cd ..
	echo 'finished copying Resources dir'
	
	# Elements in Plugins.
	cd ../PlugIns
	cp -fr * $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/PlugIns
	echo 'finished copying Plugins dir'
	
	# Elements in MacOS.
	cd ../MacOS
	cp -fr * $PACKAGES/com.smashdownloader.primary/${DATA_PREFIX}/MacOS
	echo 'finished copying MacOs dir'
	
	# Elements in Frameworks.
	cd ../Frameworks
	cp -fr +(libngs_*) $PACKAGES/com.smashdownloader.primary/${DATA_PREFIX}/Frameworks
	cp -fr !(libngs_*) $PACKAGES/com.smashdownloader.secondary/${DATA_PREFIX}/Frameworks
	echo 'finished copying Frameworks dir'
	
	# Now copy in the xml files from ngsinternal.
	cp -fr $NGSINTERNAL/. $PACKAGES_ROOT
	echo 'finished copying package xml files'
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

