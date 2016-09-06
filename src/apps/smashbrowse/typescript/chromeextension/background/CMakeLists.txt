# Note the following files should match those specified in tsconfig.json.
set(bg_ts
    browserwrap.ts
    bgcomm.ts
    bgcommhandler.ts
    bgmain.ts
)
# Check these files match those specified in tsconfig.json.
check_ts_config("${bg_ts}")

# Form bg_js by adding our build dir prefix.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" bg_js "${bg_ts}")

# Form bg_js by changing .ts suffix to .js.
string(REGEX REPLACE "(\\.ts)" ".js" bg_js "${bg_js}")

# The merged result.
set(bg_js_merged ${CMAKE_CURRENT_BINARY_DIR}/background.js)

# The merged and minified result.
set(bg_js_min ${CMAKE_CURRENT_BINARY_DIR}/bg.js)

# Custom build command.
add_custom_command(
    OUTPUT ${bg_js} ${bg_js_merged} ${bg_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${bg_js_merged}
    
    COMMAND java
    ARGS -jar D:/installs/windows/closure_compiler/closure-compiler-v20160822.jar 
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${msg_js_lib} ${bg_js} 
         --js_output_file ${bg_js_min}
    
    DEPENDS ${bg_ts} tsconfig.json ${msg_js_lib}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(chrome_ext_bg ALL DEPENDS ${bg_js} ${bg_js_merged} ${bg_js_min})

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${bg_js_min}
		DESTINATION bin/chromeextension
		COMPONENT apps)
		
		