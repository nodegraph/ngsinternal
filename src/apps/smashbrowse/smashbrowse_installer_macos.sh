# To create the repo and installer do the following.
# The installer will be in the package dir.

# smashbrowse_installer_macos.sh package [release/debug]
# smashbrowse_installer_macos.sh create_repo [release/debug]
# smashbrowse_installer_macos.sh create_installer [release/debug]

# Activate bash globbing syntax.
shopt -s extglob

PACK_STRUCTURE=~raindrop/src/ngsinternal/src/apps/smashbrowse/installer
BUNDLE=~raindrop/dev/macos/debug/apps/smashbrowse/smashbrowse.app

PACK=~raindrop/smashbrowse_pack
REPO=~raindrop/smashbrowse_repo

DATA_PREFIX=data/smashbrowse.app/Contents



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
	mkdir -p ${PACK}/packages/com.smashbrowse.chromeextension/${DATA_PREFIX}/Resources/
	mkdir -p ${PACK}/packages/com.smashbrowse.gson/${DATA_PREFIX}/Resources/
	mkdir -p ${PACK}/packages/com.smashbrowse.html/${DATA_PREFIX}/Resources/
	mkdir -p ${PACK}/packages/com.smashbrowse.jre/${DATA_PREFIX}/Resources/
	mkdir -p ${PACK}/packages/com.smashbrowse.selenium/${DATA_PREFIX}/Resources/
	
	mkdir -p ${PACK}/packages/com.smashbrowse.primary/${DATA_PREFIX}/Resources/
	mkdir -p ${PACK}/packages/com.smashbrowse.primary/${DATA_PREFIX}/Resources/bin
	mkdir -p ${PACK}/packages/com.smashbrowse.primary/${DATA_PREFIX}/MacOS/
	mkdir -p ${PACK}/packages/com.smashbrowse.primary/${DATA_PREFIX}/Frameworks
	
	mkdir -p ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Resources/
	mkdir -p ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Resources/bin
	mkdir -p ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/MacOS/
	mkdir -p ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/PlugIns
	mkdir -p ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Frameworks
	echo 'finished making dirs'
	
	# Most dirs correspond one to one with their packages.
	cd Contents/Resources
	cp -fr chromeextension ${PACK}/packages/com.smashbrowse.chromeextension/${DATA_PREFIX}/Resources
	cp -fr gson ${PACK}/packages/com.smashbrowse.gson/${DATA_PREFIX}/Resources
	cp -fr html ${PACK}/packages/com.smashbrowse.html/${DATA_PREFIX}/Resources
	cp -fr jre ${PACK}/packages/com.smashbrowse.jre/${DATA_PREFIX}/Resources
	cp -fr selenium ${PACK}/packages/com.smashbrowse.selenium/${DATA_PREFIX}/Resources
	
	
	
	# All the other files are split into 2 packages.
	# The primary is our ngs libraries and executables.
	# The secondary is the third party libraries and executables.
	
	# Left over elements in Resources.
	cp -fr qml ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Resources
	cp -fr qt.conf ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Resources
	# Copy the app bundle icon.
	cp -fr octopus_blue.icns ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Resources
	# Copy the bin dir.
	cd bin
	cp -fr +(jcomm.jar|ngs_*) ${PACK}/packages/com.smashbrowse.primary/${DATA_PREFIX}/Resources/bin
	cp -fr !(jcomm.jar|ngs_*) ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Resources/bin
	# Copy Contents/Info.plist
	cd ../../
	cp -fr Info.plist ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}
	echo 'finished copying Resources dir'
	
	# Elements in Plugins.
	cd PlugIns
	cp -fr * ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/PlugIns
	echo 'finished copying Plugins dir'
	
	# Elements in MacOS.
	cd ../MacOS
	cp -fr * ${PACK}/packages/com.smashbrowse.primary/${DATA_PREFIX}/MacOS
	echo 'finished copying MacOs dir'
	
	# Elements in Frameworks.
	cd ../Frameworks
	cp -fr +(libngs_*) ${PACK}/packages/com.smashbrowse.primary/${DATA_PREFIX}/Frameworks
	cp -fr !(libngs_*) ${PACK}/packages/com.smashbrowse.secondary/${DATA_PREFIX}/Frameworks
	echo 'finished copying Frameworks dir'
	
	# Now copy in the xml files from PACK_STRUCTURE.
	cp -fr $PACK_STRUCTURE/. $PACK
	# Remove the vc++ runtime package on macos.
	rm -fr $PACK/packages/com.smashbrowse.vc
	echo 'finished copying package xml files'
	
	# Modify the config.xml with property repository url.
	if [ $RELEASE -eq 1 ]; then
		sed -i -e 's/REPOSITORY_URL/https:\/\/www.smashbrowse.com\/macos/repository/g' $PACK/config/config.xml
	else
		sed -i -e 's/REPOSITORY_URL/file:\/\/\/Users\/raindrop\/smashbrowse_repo_debug/g' $PACK/config/config.xml
	fi
	
	# Change modern style to mac style.
	sed -i -e 's/Modern/Mac/g' $PACK/config/config.xml
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
