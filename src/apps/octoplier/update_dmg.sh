#!/bin/bash

# This scripts has the following arguments, in this strict order.
# 1) The dmg file to update.
# 2) The new dmg file to produce.
# 3) The volume icon to use.

# See if the dmg is already mounted.
mount_device=`hdiutil info | grep 'GUID_partition_scheme'| sed 's/[[:space:]]*GUID_partition_scheme[[:space:]]*//'`

# If it's mounted unmount it, just to start everything fresh.
if [ -n "$mount_device" ]; then
	hdiutil detach $mount_device
fi

# Now mount it in shadow mode because the dmg is readonly.
hdiutil attach -owners on $1 -shadow
mount_device=`hdiutil info | grep 'GUID_partition_scheme'| sed 's/[[:space:]]*GUID_partition_scheme[[:space:]]*//'`
mount_dir=`hdiutil info | grep '/Volumes'| sed 's/^.*[[:space:]]*\/Volumes\///'`
mount_dir=/Volumes/${mount_dir}

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
if ! test -z "${mount_dir}/octoplier.app/Contents/Resources/octoplier.icns"; then
	echo copying bundle icon
	cp $3 "${mount_dir}/octoplier.app/Contents/Resources/octoplier.icns"
	defaults write ${mount_dir}/octoplier.app/Contents/Info.plist CFBundleIconFile -string octoplier.icns
	echo setting custom icon bit
	SetFile -a BC "${mount_dir}/octoplier.app"
	echo done with volume icon
fi

# Unmount the device.
hdiutil detach $mount_device

# Remove the previous pimped dmg.
rm -f $2

# Convert the shadowed dmg into the new pimped dmg.
hdiutil convert -format UDZO -o $2 $1 -shadow