#!/bin/bash

# This scripts has the following arguments, in this strict order.
# 1) The dmg file to update.
# 2) The new dmg file to produce.
# 3) The icon to use as both the volume icon and the app/bundle icon.
# 4) The app name.

app_name=$4

# See if the dmg is mounted possibly multiple times.
mounted_devices=`hdiutil info | grep "build:${app_name}" | sed 's/[[:space:]][[:space:]]*.*$//g'`

# Unmount each mounted instance.
for device in $mounted_devices; do
    echo unmounting device $device
    hdiutil detach $device
done

# Now mount our dmg in shadow mode because the dmg is readonly.
echo hdiutil attach -owners on $1 -shadow
hdiutil attach -owners on $1 -shadow
mount_device=`hdiutil info | grep "build:${app_name}" | sed 's/[[:space:]][[:space:]]*.*$//g'`
mount_dir=`hdiutil info | grep "build:${app_name}" | sed 's/^.*[[:space:]][[:space:]]*//g'`

# Make the link to the applications directory.
if ! test -z "${mount_dir}/Applications"; then
	echo creating Applications link
	ln -s /Applications "${mount_dir}/Applications"
fi

# Add the volume icon.
if ! test -z "${mount_dir}/.VolumeIcon.icns"; then
	echo copying volume icon
	cp $3 "${mount_dir}/.VolumeIcon.icns"
	echo setting custom icon bit
	SetFile -a C "${mount_dir}"
	echo done with volume icon
fi

# Add the bundle icon.
if ! test -z "${mount_dir}/${app_name}.app/Contents/Resources/app.icns"; then
	echo copying bundle icon
	cp $3 "${mount_dir}/${app_name}.app/Contents/Resources/app.icns"
	defaults write ${mount_dir}/${app_name}.app/Contents/Info.plist CFBundleIconFile -string app.icns
	echo setting custom icon bit
	SetFile -a BC "${mount_dir}/${app_name}.app"
	echo done with volume icon
fi

# Unmount the device.
hdiutil detach $mount_device

# Remove the previous pimped dmg.
rm -f $2

# Convert the shadowed dmg into the new pimped dmg.
hdiutil convert -format UDZO -o $2 $1 -shadow