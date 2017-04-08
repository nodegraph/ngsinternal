# To create the repo and installer do the following.
# The installer will be in the package dir.


# Note if we have structured the internal dirs of our app properly,
# then the codesigning process should yield files with no extended attributes.
# To check if any files have extended attributes do:
# find . -type f -print0 | xargs -0 ls -l@ | grep -e "-.*@"

# This also works.
# xattr -rl ../path/to/app


#xattr -cr /Applications/DBeaver.app
#sudo codesign --force --strict --sign - /Applications/DBeaver.app

# youmacro_installer_macos.sh package [release/debug]
# youmacro_installer_macos.sh create_repo [release/debug]
# youmacro_installer_macos.sh create_installer [release/debug]

# Activate bash globbing syntax.
shopt -s extglob

PACK_STRUCTURE=~raindrop/src/ngsinternal/src/apps/youmacro/installer

sign_shallow () {
	if [ 1 -gt "$#" ]; then
	    echo "Error: sign_shallow requires a target to sign"
	    echo "eg. sign_shallow best_app.app"
	    echo "your call: '$@'"
	    exit 1
	fi
	echo "signing: " + "$@"
	codesign --force --strict --sign "Developer ID Application: Shingo Takagi" $@
}

sign_deep () {
	if [ 1 -gt "$#" ]; then
	    echo "Error: sign_deep requires a target to sign"
	    echo "eg. sign_deep best_app.app"
	    echo "your call: '$@'"
	    exit 1
	fi
	
	codesign --force --strict --deep --sign "Developer ID Application: Shingo Takagi" $@
}

verify_executable() {
	if [ 1 -gt "$#" ]; then
	    echo "Error: verify_executable requires a target to verify"
	    echo "eg. verify_executable best_app.app"
	    exit 1
	fi

	# Mimic what GateKeeper does and verify app.
	codesign --verify --strict --deep --verbose=1 $@
	
	# Check if GateKeeper will accept signature.
	spctl --verbose=4 -a -t exec -vv $@
	
	# The app should have any extended attributes.
	xattr -rl $@
}

verify_dmg() {
	if [ 1 -gt "$#" ]; then
	    echo "Error: verify_dmg requires a target to verify"
	    echo "eg. verify_dmg best_app.dmg"
	    exit 1
	fi

	# Mimic what GateKeeper does and verify app.
	codesign --verify --strict --deep --verbose=2 $@
	
	# Check if GateKeeper will accept signature when opening.
	spctl -a -t open --context context:primary-signature -v $@
}

codesign_app () 
{
	# Do Frameworks first.
	sign_shallow $1/Contents/Frameworks/*
	
	# Do Plugins next.
	sign_shallow $1/Contents/Plugins/bearer/*
	sign_shallow $1/Contents/Plugins/imageformats/*
	sign_shallow $1/Contents/Plugins/platforms/*
	sign_shallow $1/Contents/Plugins/printsupport/*
	sign_shallow $1/Contents/Plugins/quick/*
	
	# Do MacOS next.
	sign_shallow $1/Contents/MacOS/*
	
	# Although we have the JRE in Resources, we don't sign anything in Resources because it should be 
	# considered as non executable data for codesigning's sake.
	
	# Finally sign the app.
	sign_shallow $1
	
	# First sign will put extended attributes on jre files in Resources.
	# We remove them.
	xattr -cr $1
	
	# Verify app.
	verify_executable $1
}	

# -------------------------------------------------------------------------
# Create our packages.
# -------------------------------------------------------------------------
package () {
	echo "packaging.."
	
	rm -fr $PACK

	# Go to the build dir containing our app.
	cd $CMAKE_BUILD_ROOT/apps/youmacro;
	
	# Create the package dir.
	mkdir -p ${PACK}/packages/com.youmacro.main/data
	
	# Copy app over into the data dir.
	CpMac -r  youmacro.app ${PACK}/packages/com.youmacro.main/data
	
	# Codesign it.
	codesign_app ${PACK}/packages/com.youmacro.main/data/youmacro.app
		
	# Now copy in the xml files from PACK_STRUCTURE.
	cp -fr $PACK_STRUCTURE/. $PACK
	
	# Remove the vc++ runtime package on macos.
	rm -fr $PACK/packages/com.youmacro.vc
	echo 'finished copying package xml files'
	
	# Replace RESPOSITY_URL in the config.xml.
	#if [ $RELEASE -eq 1 ]; then
	#	sed -i -e 's#REPOSITORY_URL#https://www.youmacro.com/macos/youmacro_repo#g' $PACK/config/config.xml
	#else
	#	sed -i -e "s#REPOSITORY_URL#file://${REPO}#g" $PACK/config/config.xml
	#fi
	
	# Replace LAUNCH_PROGRAM in the config.xml.
	sed -i -e 's#LAUNCH_PROGRAM#youmacro.app/Contents/MacOS/youmacro#g' $PACK/config/config.xml
	
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
	mkdir dmg_folder
	binarycreator --offline-only -c 'config/config.xml' -p packages dmg_folder/${installer_base_name}
	
	# Code sign the installer.
	sign_shallow dmg_folder/${installer_base_name}.app
	
	# Verify installer.
	verify_executable dmg_folder/${installer_base_name}.app
	
	# Create DMG.
	rm -f ${installer_base_name}.dmg
	hdiutil create -volname ${installer_base_name} -srcfolder ./dmg_folder -format UDZO ${installer_base_name}.dmg
	#hdiutil convert /path/to/rwimage.dmg -format UDRO -o /path/to/readonlyimage.dmg
	
	# Code sign the dmg.
	xattr -rc ${installer_base_name}.dmg
	codesign --deep --sign "Developer ID Application: Shingo Takagi" ${installer_base_name}.dmg
	
	# Verify the dmg.
	verify_dmg ${installer_base_name}.dmg
	
	# Let's try a zip archive.
	rm -f ${installer_base_name}.zip
	zip -r ${installer_base_name}.zip ./dmg_folder
	
	# Code sign the zip.
	xattr -rc ${installer_base_name}.zip
	codesign --deep --sign "Developer ID Application: Shingo Takagi" ${installer_base_name}.zip
	
	# Verify the zip.
	verify_dmg ${installer_base_name}.zip
	
}

# -------------------------------------------------------------------------
# Main Logic.
# -------------------------------------------------------------------------
if [ "$#" -ne 2 ]; then
    echo "at least 2 arguments are required: "
    echo "[1]: package, create_repo, update_repo or create_installer"
    echo "[2]: the cmake build dir root"
    exit 1
fi

echo "cmake build root set to: " $2
CMAKE_BUILD_ROOT=$2
PACK=${CMAKE_BUILD_ROOT}/youmacro_pack
REPO=${CMAKE_BUILD_ROOT}/youmacro_repo

source ${CMAKE_BUILD_ROOT}/ngsversion.sh

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
