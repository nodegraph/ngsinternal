
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} "../../src" )

set(sources
    main.cpp
    tweeter.cpp
)

qt5_wrap_cpp(moc_imps tweeter.h helper.h)
list(APPEND sources ${moc_imps})

add_executable( twitterdemo ${sources} )
target_link_libraries( twitterdemo o2 Qt5::Core Qt5::Widgets Qt5::Network)

# Install.
INSTALL(TARGETS twitterdemo
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  )