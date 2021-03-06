find_package(Java REQUIRED)
include(UseJava)
set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/generator.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake"
    IMMEDIATE @ONLY)

add_custom_command(
    OUTPUT "MessageEnums.java"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_JAVA_COMPILE_FLAGS -g:none)
else ()
    set(CMAKE_JAVA_COMPILE_FLAGS -g)
endif()

add_jar(jcomm 
    SOURCES JComm.java MessageEnums.java WebDriverWrap.java FSWrap.java BaseMessage.java RequestMessage.java ResponseMessage.java InfoMessage.java
    INCLUDE_JARS
                # gson 
                "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson/gson-2.8.0.jar"
                # selenium
                "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/client-combined-3.3.1-nodeps.jar"
                "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/cglib-nodep-3.2.4.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/commons-codec-1.10.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/commons-exec-1.3.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/commons-io-2.5.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/commons-lang3-3.5.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/commons-logging-1.2.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/cssparser-0.9.21.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/gson-2.8.0.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/guava-21.0.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/hamcrest-core-1.3.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/hamcrest-library-1.3.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/htmlunit-2.24.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/htmlunit-core-js-2.23.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/htmlunit-driver-2.24.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/httpclient-4.5.2.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/httpcore-4.4.4.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/httpmime-4.5.2.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/javax.servlet-api-3.1.0.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/jetty-io-9.4.1.v20170120.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/jetty-util-9.4.1.v20170120.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/jna-4.1.0.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/jna-platform-4.1.0.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/junit-4.12.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/neko-htmlunit-2.24.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/phantomjsdriver-1.4.0.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/sac-1.3.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/serializer-2.7.2.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/websocket-api-9.2.20.v20161216.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/websocket-client-9.2.20.v20161216.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/websocket-common-9.2.20.v20161216.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/xalan-2.7.2.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/xercesImpl-2.11.0.jar"
				"${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/lib/xml-apis-1.4.01.jar"
   # OUTPUT_DIR ${PROJECT_BINARY_DIR}/install/bin
    )

get_target_property(jar_file jcomm JAR_FILE)
get_target_property(compile_dir jcomm CLASSDIR)


message(STATUS "jcomm jar file located at: ${jar_file}")
message(STATUS "jcomm classes compiled at: ${compile_dir}")

INSTALL(
	  FILES
	      "${CMAKE_CURRENT_BINARY_DIR}/jcomm.jar"
	  DESTINATION bin
	  COMPONENT jcomm
	)


