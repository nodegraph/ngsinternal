# To install do.
# ninja install
# ninja fill_smashbrowse

# If smashbrowse.app doesn't seem to be updating then do.
# ninja wipe_smashbrowse
# ninja install
# ninja fill_smashbrowse

# Todo.
# fill_smashbrowse is time consuming and it's mostly copying files which don't change
# make another fill command which copies the most frequently changing files like jcomm.jar

# Command to install dependent libraries into the the app.
set(app ${CMAKE_CURRENT_BINARY_DIR}/smashbrowse.app/Contents)
add_custom_command(
	OUTPUT copy_other_files
	#COMMAND rm -fr ${app}/Frameworks/*
	#COMMAND rm -fr ${app}/PlugIns/*
	#COMMAND rm -fr ${app}/Resources/*
	COMMAND ${QT5_DIR}/bin/macdeployqt ARGS ${CMAKE_CURRENT_BINARY_DIR}/smashbrowse.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -verbose=3 
	#
	COMMAND cp -fRL ${CMAKE_CURRENT_SOURCE_DIR}/html ${app}/Resources
	COMMAND mkdir -p ${app}/Resources/jre
	COMMAND cp -fRL /Users/raindrop/installs/macosunpacks/jre1.8.0_121.jre/Contents/Home/* ${app}/Resources/jre
	COMMAND mkdir -p ${app}/Resources/selenium
	COMMAND cp -fRL ${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.0.1/* ${app}/Resources/selenium
	#
	COMMAND cp -fRL "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson" ${app}/Resources/gson
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/chromeextension ${app}/Resources/chromeextension
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/bin/* ${app}/MacOS
)


# Custom target which fills out smashbrowse with other dependencies and files.
add_custom_target (fill_smashbrowse
   DEPENDS copy_other_files
)		

add_custom_command(
	OUTPUT wipe_app_bundle
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/smashbrowse.app
)

add_custom_target (wipe_smashbrowse
   DEPENDS wipe_app_bundle
)
