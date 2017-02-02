add_subdirectory(background)
add_subdirectory(content)

set(dest "bin/chromeextension")

# Install static chrome extension files.
INSTALL(FILES 
            css/contextmenu.css
            css/overlay.css
            css/popupdialog.css
            manifest.json
        DESTINATION ${dest}
        COMPONENT chrome_ext_content
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE
        )
        
# Install chrome extension images.
INSTALL(DIRECTORY images
    DESTINATION ${dest}
    COMPONENT chrome_ext_content
    PATTERN "*"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE
    )