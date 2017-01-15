# Extract the ts files from tsconfig.json.
# Note that for cmake to pick up changes in tsconfig.json you need to touch this file.
get_tsconfig_files(cnt_ts)
message("content files are: ${cnt_ts}")


# Add our build dir prefix to the ts filenames.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" cnt_js "${cnt_ts}")

# Change the .ts suffix to .js in our ts filenames.
string(REGEX REPLACE "(\\.ts)" ".js" cnt_js "${cnt_js}")

# The name of the merged result. This is currently not used.
set(cnt_js_merged ${CMAKE_CURRENT_BINARY_DIR}/content.js)

# The merged and minified result.
set(cnt_js_min ${CMAKE_CURRENT_BINARY_DIR}/cnt.js)

# Custom build command.
add_custom_command(
    OUTPUT ${cnt_js} ${cnt_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/${tsconfig_json} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${cnt_js_merged}
        
    COMMAND java
    ARGS -jar ${closure_compiler_jar} 
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${msg_js_lib} ${cnt_js} 
         --js_output_file ${cnt_js_min}
        
    DEPENDS ${cnt_ts} tsconfig.json ${msg_js_lib}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(chrome_ext_content ALL DEPENDS ${cnt_js} ${cnt_js_min})

set(dest "bin/chromeextension")
if (${ARCH} STREQUAL ARCH_MACOS)
	set(dest "../build/smashbrowse.app/Contents/MacOS/chromeextension")
endif()

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${cnt_js_min}
		DESTINATION ${dest}
		COMPONENT apps
    	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)
