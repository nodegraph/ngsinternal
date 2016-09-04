add_subdirectory(background)
add_subdirectory(content)

# Install static chrome extension files.
INSTALL(FILES 
            css/contextmenu.css
            css/overlay.css
            css/popupdialog.css
            manifest.json
            require.js
        DESTINATION bin/chromeextension
        COMPONENT apps)
        
# Install chrome extension images.
INSTALL(DIRECTORY images
    DESTINATION bin/chromeextension
    COMPONENT apps)