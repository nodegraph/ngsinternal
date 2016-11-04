
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} "../../src" )

set(sources
    fbdemo.cpp
    main.cpp
)

qt5_wrap_cpp(moc_imps fbdemo.h helper.h)
list(APPEND sources ${moc_imps})

add_executable( facebookdemo ${sources} )
target_link_libraries( facebookdemo o2 Qt5::Core Qt5::Widgets Qt5::Network)

# Install.
INSTALL(TARGETS facebookdemo
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  )