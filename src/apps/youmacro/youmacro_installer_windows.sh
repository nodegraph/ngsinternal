# To create the repo and installer do the following.
# The installer will be in the package dir.

# youmacro_installer_windows.sh package build_root
# youmacro_installer_windows.sh create_repo cmake_build_root
# youmacro_installer_windows.sh create_installer cmake_build_root


# Activate bash globbing syntax.
shopt -s extglob

PACK_STRUCTURE=/d/src/ngsinternal/src/apps/youmacro/installer



# -------------------------------------------------------------------------
# Create our packages.
# -------------------------------------------------------------------------
package ()
{
	echo "packaging.."
	rm -fr $PACK

	# Go to the install dir created from ninja install.
	cd $CMAKE_BUILD_ROOT/install;
	
	# We only create one package with actual files for simplicity.
	mkdir -p ${PACK}/packages/com.youmacro.main/data
	
	# Copy all of the files in install over to the package data.
	cp -fr * ${PACK}/packages/com.youmacro.main/data
	
	# Now copy in the xml files from PACK_STRUCTURE.
	cp -fr $PACK_STRUCTURE/. $PACK
	
	# We no longer use an online repo.
	# We only do offline installers.
	
	## Replace RESPOSITY_URL in the config.xml.
	#if [ $RELEASE -eq 1 ]; then
	#	sed -i -e 's/REPOSITORY_URL/https:\/\/www.youmacro.com\/windows\/youmacro_repo/g' $PACK/config/config.xml
	#else
	#	# Format the repo path for windows.
	#	LOC=`echo "$REPO" | sed 's/^\///' | sed 's/^./\0:/'`
	#	echo "LOC is: " $LOC
	#	sed -i -e "s#REPOSITORY_URL#file:///$LOC#g" $PACK/config/config.xml
	#fi
	
	# Replace LAUNCH_PROGRAM in the config.xml.
	sed -i -e 's/LAUNCH_PROGRAM/bin\/youmacro.exe/g' $PACK/config/config.xml
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
	echo "creating installer with name ${installer_base_name}"
	cd $PACK
	binarycreator --offline-only -c 'config/config.xml' -p packages ${installer_base_name}.exe
	
	# Code sign the installer
	"C:/Program Files (x86)/Windows Kits/8.1/bin/x64/signtool.exe" sign //debug //tr http:////timestamp.digicert.com //td sha256 //fd sha256 //f "c:\shingo\critical\code_sign_certs\my_cert.pfx" //p ${PASSWORD} "${installer_base_name}.exe"
	#"C:/Program Files (x86)/Windows Kits/8.1/bin/x64/signtool.exe" sign //tr http:////timestamp.digicert.com //td sha256 //fd sha256 //a "${installer_base_name}.exe"
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

