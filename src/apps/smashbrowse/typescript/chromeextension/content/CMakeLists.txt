# Extract the ts files from tsconfig.json.
get_tsconfig_files(cnt_ts)

# Form cnt_js by adding our build dir prefix.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" cnt_js "${cnt_ts}")

# Form cnt_js by changing .ts suffix to .js.
string(REGEX REPLACE "(\\.ts)" ".js" cnt_js "${cnt_js}")

# The merged result.
set(cnt_js_merged ${CMAKE_CURRENT_BINARY_DIR}/content.js)

# The merged and minified result.
set(cnt_js_min ${CMAKE_CURRENT_BINARY_DIR}/cnt.js)

# Custom build command.
add_custom_command(
    OUTPUT ${cnt_js} ${cnt_js_merged} ${cnt_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${cnt_js_merged}
        
    COMMAND java
    ARGS -jar D:/installs/windows/closure_compiler/closure-compiler-v20160822.jar 
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${msg_js_lib} ${cnt_js} 
         --js_output_file ${cnt_js_min}
        
    DEPENDS ${cnt_ts} tsconfig.json ${msg_js_lib}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(chrome_ext_content ALL DEPENDS ${cnt_js} ${cnt_js_merged} ${cnt_js_min})

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${cnt_js_min}
		DESTINATION bin/chromeextension
		COMPONENT apps)
