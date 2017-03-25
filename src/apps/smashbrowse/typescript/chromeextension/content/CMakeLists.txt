# Extract the ts files from tsconfig.json.
# Note that for cmake to pick up changes in tsconfig.json you need to touch this file.
get_tsconfig_files(ts_files)
message("content files are: ${ts_files}")

# Add our build dir prefix to the ts filenames.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" js_files "${ts_files}")

# Change the .ts suffix to .js in our ts filenames.
string(REGEX REPLACE "(\\.ts)" ".js" js_files "${js_files}")

# The name of the merged result. This is currently not used.
set(merged ${CMAKE_CURRENT_BINARY_DIR}/content.js)

# The merged and minified result.
set(minimized ${CMAKE_CURRENT_BINARY_DIR}/cnt.js)

# Custom build command.
add_custom_command(
    OUTPUT ${js_files} ${minimized}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/${tsconfig_json} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${merged}
        
    COMMAND java
    ARGS -jar ${closure_compiler_jar} 
         ${TSC_DEBUG_ARGS}
         --formatting=PRETTY_PRINT
         #--use_types_for_optimization false
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${js_files} 
         --js_output_file ${minimized}
        
    DEPENDS ${ts_files} tsconfig.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(chrome_ext_content ALL DEPENDS ${js_files} ${minimized})

set(dest "chromeextension")

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${minimized}
		DESTINATION ${dest}
		COMPONENT chrome_ext_content
    	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)
