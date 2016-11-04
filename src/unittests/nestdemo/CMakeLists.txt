
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} "../../src" )

set(sources
    o2nest.cpp
    fbdemo.cpp
    main.cpp
)

qt5_wrap_cpp(moc_imps o2nest.h fbdemo.h helper.h)
list(APPEND sources ${moc_imps})

add_executable( nestdemo ${sources} )
target_link_libraries( nestdemo o2 Qt5::Core Qt5::Widgets Qt5::Network)

# Install.
INSTALL(TARGETS nestdemo
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  )