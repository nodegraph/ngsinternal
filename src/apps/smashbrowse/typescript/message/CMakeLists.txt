set(msg_ts
    message.ts
)

# Form msg_js by adding our build dir prefix.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" msg_js "${msg_ts}")

# Form msg_js by changing .ts suffix to .js.
string(REGEX REPLACE "(\\.ts)" ".js" msg_js "${msg_js}")

# The merged result.
set(msg_js_merged ${CMAKE_CURRENT_BINARY_DIR}/msg.js)

# The merged and minified result.
set(msg_js_min ${CMAKE_CURRENT_BINARY_DIR}/m.js)

# The main lib that other's will use.
# Make msg_js_merged available to other dirs besides the message dir.
set(msg_js_lib ${CMAKE_CURRENT_BINARY_DIR}/message.js PARENT_SCOPE)

add_custom_command(
    OUTPUT ${msg_js} ${msg_js_merged} ${msg_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${msg_js_merged}
    
    COMMAND java
    ARGS -jar D:/installs/windows/closure_compiler/closure-compiler-v20160822.jar 
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${msg_js}
         --js_output_file ${msg_js_min}
    
    DEPENDS ${msg_ts} tsconfig.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(msg_js ALL DEPENDS ${msg_js} ${msg_js_merged} ${msg_js_min})
