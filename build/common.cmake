
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
    set(GTEST_LIBS libgtest.a libgtest_main.a pthread)
endmacro()

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected (options are: Debug Release), default to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()

set(COMMON_CXX_FLAGS
    "-std=c++11 -fPIC -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections")

set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CXX_FLAGS} -fvisibility=hidden -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_CXX_FLAGS} -O0 -g")

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -fPIC -Wl,--gc-sections -Wl,--no-undefined,--no-undefined-version -Wl,--hash-style=gnu -Wl,--strip-all")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Bdynamic -pie -Wl,--gc-sections -Wl,-z,nocopyreloc -Wl,--strip-all")
else()
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -fPIC -Wl,--gc-sections -Wl,--no-undefined,--no-undefined-version -Wl,--hash-style=gnu")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Bdynamic -pie -Wl,--gc-sections -Wl,-z,nocopyreloc")
endif()
