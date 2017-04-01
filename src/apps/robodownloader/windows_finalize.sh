# -------------------------------------------------------------------------
# Main Logic.
# -------------------------------------------------------------------------
if [ "$#" -ne 1 ]; then
    echo "incorrect arguments were given: "
    echo "[1]: the cmake build dir root"
    exit 1
fi


echo "cmake build root set to: " $1
CMAKE_BUILD_ROOT=$1

# Copy the install dir to Robo_Downloader.
rm -fr ${CMAKE_BUILD_ROOT}/Robo_Downloader
cp -r ${CMAKE_BUILD_ROOT}/install ${CMAKE_BUILD_ROOT}/Robo_Downloader

# Copy a shortcut into Robo_Downloader to help users find the right executable.


# Use 7-zip to compress the install directory.
"C:/Program Files/7-Zip/7z.exe" a ${CMAKE_BUILD_ROOT}/Robo_Downloader_X.X.X.X.exe -t7z -m0=lzma2 -mx=9 -mmt=32 -ms=on -sfx7z.sfx ${CMAKE_BUILD_ROOT}/Robo_Downloader

# Sign the installer with our EV certificate.
# We don't have one yet.
