# Extract the ts files from tsconfig.json.
# Note that for cmake to pick up changes in tsconfig.json you need to touch this file.
get_tsconfig_files(ch_ts)

# Form ch_js by adding our build dir prefix.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" ch_js "${ch_ts}")

# Form ch_js by changing .ts suffix to .js.
string(REGEX REPLACE "(\\.ts)" ".js" ch_js "${ch_js}")

# The merged result.
set(ch_js_merged ${CMAKE_CURRENT_BINARY_DIR}/commhub_merged.js)

# The merged and minified result.
set(ch_js_min ${CMAKE_CURRENT_BINARY_DIR}/ch.js)

# The source map.
set(ch_js_min_map ${CMAKE_CURRENT_BINARY_DIR}/ch_map.js)

# Custom build command.
add_custom_command(
    OUTPUT ${ch_js_merged} ${ch_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR} 
         --outDir ${CMAKE_CURRENT_BINARY_DIR}
         --outFile ${ch_js_merged}
         
    #COMMAND echo
    #ARGS "building with ${msg_js_lib} ${ch_js_merged} require.js"
    
    COMMAND java
    ARGS -jar D:/installs/windows/closure_compiler/closure-compiler-v20160822.jar 
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --create_source_map ${ch_js_min_map}
         --js ${msg_js_lib} ${ch_js_merged}
         --js_output_file ${ch_js_min}
         
    DEPENDS ${ch_ts} tsconfig.json msg_js ${msg_js_lib}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(commhub ALL DEPENDS ${ch_js_merged} ${ch_js_min})

INSTALL(FILES 
            ${ch_js_min}
            cert.pem # certs for https server
            key.pem # certs for https server
            chb.js # commhub bootstrap file
		DESTINATION bin
		COMPONENT apps)
		
		
		
