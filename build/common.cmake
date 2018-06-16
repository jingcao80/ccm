
macro(COPY target file dest)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND
            "${CMAKE_COMMAND}"
            -E copy ${file} ${dest})
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
    set_property(TARGET libgtest.a PROPERTY IMPORTED_LOCATION ${EXTERNAL_DIR}/gtest/lib/libgtest.a)
    add_library(libgtest_main.a UNKNOWN IMPORTED)
    set_property(TARGET libgtest_main.a PROPERTY IMPORTED_LOCATION ${EXTERNAL_DIR}/gtest/lib/libgtest_main.a)
    include_directories(${EXTERNAL_DIR}/gtest/inc)
    set(GTEST_LIBS libgtest.a libgtest_main.a pthread)
endmacro()

add_compile_options(
    -std=c++11
    -fPIC
    -fvisibility=hidden
    -fno-exceptions
    -fno-rtti
    -ffunction-sections
    -fdata-sections
    -funwind-tables)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -fPIC -Wl,--gc-sections -Wl,--no-undefined,--no-undefined-version -Wl,--hash-style=gnu")

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Bdynamic -pie -Wl,--gc-sections -Wl,-z,nocopyreloc")
