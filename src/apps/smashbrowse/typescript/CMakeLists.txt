# Function used to check that the files in tsconfig.json match
# those specified in cmake. Used by CMakeLists in our sub dirs.
function(check_ts_config cmake_ts_files)
    # Read tsconfig file.
    set(tsconfig_file "${CMAKE_CURRENT_SOURCE_DIR}/tsconfig.json")
    file(STRINGS ${tsconfig_file} lines REGEX "(\".*\")")
    
    # Extract the tsconfig files.
    unset(tsconfig_files)
    foreach(line ${lines})
      if("${line}" MATCHES "\"files\":")
          #message("line is ${line}")
          string(REGEX MATCHALL "([^ ,:;\"[]+)" tsconfig_files ${line})
          list(REMOVE_ITEM tsconfig_files "files")
      endif()
    endforeach()
    #message("the tsconfig file are: ${tsconfig_files}")
    #message("the cmake ts files are: ${cmake_ts_files}")
    if ("${tsconfig_files}" STREQUAL "${cmake_ts_files}") 
        #message(STATUS "tscconfig.json and cmake match verified")
    else()
        message(FATAL_ERROR "The files specified in tsconfig.json and cmake don't match. ${cmake_ts_files}")
    endif()
endfunction()


# Sub Directories.
add_subdirectory(message) # This injects ${msg_js_lib} into this dir scope, so the following dirs can use it.
add_subdirectory(chromeextension)
add_subdirectory(commhub)


