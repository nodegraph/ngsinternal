find_package(Java REQUIRED)
include(UseJava)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

add_jar(intercom Intercom.java)

get_target_property(jar_file intercom JAR_FILE)
get_target_property(compile_dir intercom CLASSDIR)


message(STATUS "intercom jar file located at: ${jar_file}")
message(STATUS "intercom classes compiled at: ${compile_dir}")


