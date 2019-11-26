
macro(COPY target file dest)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND
            "${CMAKE_COMMAND}"
            -E copy_if_different ${file} ${dest})
endmacro()

macro(MKDIR target dir)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND
            "${CMAKE_COMMAND}"
            -E make_directory ${dir})
endmacro()

macro(IMPORT_LIBRARY target)
    add_library(${target} UNKNOWN IMPORTED)
    set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${BIN_DIR}/${target})
endmacro()

macro(IMPORT_GTEST)
    add_library(libgtest.a UNKNOWN IMPORTED)
    set_property(TARGET libgtest.a PROPERTY IMPORTED_LOCATION $ENV{OUT_PATH}/lib/gtest.a)
    add_library(libgtest_main.a UNKNOWN IMPORTED)
    set_property(TARGET libgtest_main.a PROPERTY IMPORTED_LOCATION $ENV{OUT_PATH}/lib/gtest_main.a)
    include_directories(${EXTERNAL_DIR}/googletest/include)
if($ENV{PLATFORM} STREQUAL "linux")
    set(GTEST_LIBS libgtest.a libgtest_main.a pthread)
elseif($ENV{PLATFORM} STREQUAL "android")
    set(GTEST_LIBS libgtest.a libgtest_main.a)
endif()
endmacro()

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected (options are: Debug Release), default to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()
