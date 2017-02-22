# Extract the ts files from tsconfig.json.
# Note that for cmake to pick up changes in tsconfig.json you need to touch this file.
get_tsconfig_files(bg_ts)

# Add our build dir prefix to the ts filenames.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" bg_js "${bg_ts}")

# Change the .ts suffix to .js in our ts filenames.
string(REGEX REPLACE "(\\.ts)" ".js" bg_js "${bg_js}")

# The name of the merged result. This is currently not used.
set(bg_js_merged ${CMAKE_CURRENT_BINARY_DIR}/background.js)

# The merged and minified result.
set(bg_js_min ${CMAKE_CURRENT_BINARY_DIR}/bg.js)

# Custom build command.
add_custom_command(
    OUTPUT ${bg_js} ${bg_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/${tsconfig_json} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${bg_js_merged}
    
    COMMAND java
    ARGS -jar ${closure_compiler_jar}
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${msg_js_lib} ${bg_js} 
         --js_output_file ${bg_js_min}
    
    DEPENDS ${bg_ts} tsconfig.json ${msg_js_lib}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(chrome_ext_bg ALL DEPENDS ${bg_js} ${bg_js_min})

set(dest "bin/chromeextension")

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${bg_js_min}
		DESTINATION ${dest}
		COMPONENT chrome_ext_background
    	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)
		
		
