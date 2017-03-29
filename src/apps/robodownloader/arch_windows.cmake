# Install.
INSTALL(TARGETS robodownloader
  RUNTIME DESTINATION bin
  COMPONENT robodownloader
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
)

# Install the html files.
INSTALL(DIRECTORY html
		DESTINATION .
		COMPONENT robodownloader)