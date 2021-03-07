# Extract the ts files from tsconfig.json.
# Note that for cmake to pick up changes in tsconfig.json you need to touch this file.
get_tsconfig_files(ts_files)

# Add our build dir prefix to the ts filenames.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" js_files "${ts_files}")

# Change the .ts suffix to .js in our ts filenames.
string(REGEX REPLACE "(\\.ts)" ".js" js_files "${js_files}")

# The name of the merged result. This is currently not used.
set(merged ${CMAKE_CURRENT_BINARY_DIR}/msg.js)

# The merged and minified result.
set(minimized ${CMAKE_CURRENT_BINARY_DIR}/m.js)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/generator.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake"
    IMMEDIATE @ONLY)

add_custom_command(
    OUTPUT "messagetypes.ts" "messagetypes.d.ts"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Custom build command.
add_custom_command(
    OUTPUT ${js_files} ${minimized}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/${tsconfig_json} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${merged}
    
    COMMAND java
    ARGS -jar ${closure_compiler_jar}
        ${TSC_DEBUG_ARGS}
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${js_files}
         --js_output_file ${minimized}
    
    DEPENDS ${ts_files} tsconfig.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(message ALL DEPENDS ${js_files} ${minimized})

set(dest "chromeextension")

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${minimized}
		DESTINATION ${dest}
		COMPONENT chrome_ext_background
    	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)
