# Function used to grab the ts files from tsconfig.json.
# Note having the files in tsconfig.json allows Visual Studio Code to parse typescript properly.
function(get_tsconfig_files ts_files)
    # Read tsconfig file.
    set(tsconfig_file "${CMAKE_CURRENT_SOURCE_DIR}/${tsconfig_json}")
    file(STRINGS ${tsconfig_file} lines REGEX "(\".*\")")
    
    # Extract the tsconfig files.
    unset(tsconfig_files)
    foreach(line ${lines})
      if("${line}" MATCHES "\"files\":")
          #message("line is ${line}")
          string(REGEX MATCHALL "([^ ,:;\"[]+)[^\\.][^d]\\.ts" tsconfig_files ${line})
          list(REMOVE_ITEM tsconfig_files "files")
      endif()
    endforeach()
    #message("tsconfig_files: ${tsconfig_files}")
    
    # Copy the tsconfig files into the return value argument.
    # Note that ${ts_files} will be empty in this scope, but not in our calling parent scope.
    SET(${ts_files} ${tsconfig_files} PARENT_SCOPE)
endfunction()

set(active_window_js_lib "D:/installs/srcdeps/ngsexternal/nodejs2/node_modules/active-window/index.js")

# Sub Directories.
add_subdirectory(message) # This injects ${msg_js_lib} into this dir scope, so the following dirs can use it.
add_subdirectory(chromeextension)
add_subdirectory(commhub)


