# Our nodejs controller files.
set(cntrl_ts
    debugutils.ts
    fswrap.ts
    socketmessage.ts
    webdriverwrap.ts
    commrelay.ts
)

# Form cntrl_js by adding our build dir prefix.
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_BINARY_DIR}/\\1" cntrl_js "${cntrl_ts}")

# Form cntrl_js by changing .ts suffix to .js.
string(REGEX REPLACE "(\\.ts)" ".js" cntrl_js "${cntrl_js}")

# The merged result.
set(cntrl_js_merged ${CMAKE_CURRENT_BINARY_DIR}/controller.js)

# The merged and minified result.
set(cntrl_js_min ${CMAKE_CURRENT_BINARY_DIR}/controller.min.js)

# Typescript external declaration files.
set(cntrl_dts
    D:/dev/windows/DefinitelyTyped/node/node.d.ts
    D:/dev/windows/DefinitelyTyped/ws/ws.d.ts
    D:/dev/windows/DefinitelyTyped/selenium-webdriver/selenium-webdriver.d.ts
)

set(cntrl_externs ${CMAKE_CURRENT_BINARY_DIR}/externs.js)

add_custom_command(
    OUTPUT ${cntrl_js} ${cntrl_js_merged} ${cntrl_js_min}
    
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR} --outDir ${CMAKE_CURRENT_BINARY_DIR}
        --outFile ${cntrl_js_merged}
        
    #COMMAND uglifyjs  
    #ARGS --compress -- ${cntrl_js} -o ${CMAKE_CURRENT_BINARY_DIR}/typescript/controller/controller.js
    
    #COMMAND dts2externs  
    #ARGS ${cntrl_dts} -o ${cntrl_externs}
    
    COMMAND java
    ARGS -jar D:/installs/windows/closure_compiler/closure-compiler-v20160822.jar 
        #--compilation_level WHITESPACE_ONLY
        --compilation_level SIMPLE_OPTIMIZATIONS 
        #--compilation_level ADVANCED_OPTIMIZATIONS
        #--externs ${cntrl_externs}
        --js ${cntrl_js_merged} require.js
        --js_output_file ${cntrl_js_min}
    DEPENDS ${cntrl_ts} tsconfig.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(controller ALL DEPENDS ${cntrl_js} ${cntrl_js_merged} ${cntrl_js_min} )

		
# Install the nodejs controller app.
INSTALL(FILES 
            ${cntrl_js_min}
            cert.pem
            key.pem
            controllerbootstrap.js
            require.js
		DESTINATION bin
		COMPONENT apps)
		
		
		
