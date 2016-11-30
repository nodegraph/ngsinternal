# Extract the ts files from tsconfig.json.
# Note that for cmake to pick up changes in tsconfig.json you need to touch this file.
get_tsconfig_files(msg_ts)

# Add our build dir prefix to the ts filenames.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" msg_js "${msg_ts}")





# Change the .ts suffix to .js in our ts filenames.
string(REGEX REPLACE "(\\.ts)" ".js" msg_js "${msg_js}")

# The name of the merged result. This is currently not used.
set(msg_js_merged ${CMAKE_CURRENT_BINARY_DIR}/msg.js)

# The merged and minified result.
set(msg_js_min ${CMAKE_CURRENT_BINARY_DIR}/m.js)

# The main lib that other's will use.
# Make msg_js_merged available to other dirs besides the message dir.
set(msg_js_lib ${CMAKE_CURRENT_BINARY_DIR}/message.js PARENT_SCOPE)


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
    OUTPUT ${msg_js} ${msg_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/${tsconfig_json} --outDir ${CMAKE_CURRENT_BINARY_DIR} #--outFile ${msg_js_merged}
    
    COMMAND java
    ARGS -jar ${closure_compiler_jar}
         #--compilation_level WHITESPACE_ONLY
         --compilation_level SIMPLE_OPTIMIZATIONS 
         #--compilation_level ADVANCED_OPTIMIZATIONS
         --js ${msg_js}
         --js_output_file ${msg_js_min}
    
    DEPENDS ${msg_ts} tsconfig.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(msg_js ALL DEPENDS ${msg_js} ${msg_js_min})
