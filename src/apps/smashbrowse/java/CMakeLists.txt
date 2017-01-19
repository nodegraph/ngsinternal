find_package(Java REQUIRED)
include(UseJava)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")


add_jar(jcomm 
    SOURCES BrowserController.java
    INCLUDE_JARS "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson/gson-2.8.0.jar"
    OUTPUT_DIR ${PROJECT_BINARY_DIR}/install/bin
    )

get_target_property(jar_file jcomm JAR_FILE)
get_target_property(compile_dir jcomm CLASSDIR)


message(STATUS "jcomm jar file located at: ${jar_file}")
message(STATUS "jcomm classes compiled at: ${compile_dir}")

# You can run the java program as follows.
# java -cp /d/installs/srcdeps/ngsexternal/java/gson/gson-2.8.0.jar:./jcomm.jar BrowserController

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson/gson-2.8.0.jar"
  DESTINATION bin
  COMPONENT thirdparty
)