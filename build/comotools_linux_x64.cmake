
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)

set(CMAKE_SHARED_LIBRARY_PREFIX_CXX "" CACHE STRING "" FORCE)
set(CMAKE_STATIC_LIBRARY_PREFIX_CXX "" CACHE STRING "" FORCE)

set(COMMON_C_FLAGS
    "-fPIC -ffunction-sections -fdata-sections")

set(COMMON_CXX_FLAGS
    "${COMMON_C_FLAGS} -std=c++14 -fno-exceptions -fno-rtti")

set(COMMON_SHARED_LINKER_FLAGS
    "-shared -fPIC ${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--no-undefined,--no-undefined-version -Wl,--hash-style=gnu")

set(COMMON_EXE_LINKER_FLAGS
    "-Bdynamic -pie ${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,-z,nocopyreloc")

if($ENV{VERSION} STREQUAL "rls")
    set(CMAKE_C_FLAGS
        "${COMMON_C_FLAGS} -fvisibility=hidden -Os" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS
        "${COMMON_CXX_FLAGS} -fvisibility=hidden -Os" CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS
        "${COMMON_SHARED_LINKER_FLAGS} -Wl,--strip-all" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS
        "${COMMON_EXE_LINKER_FLAGS} -Wl,--strip-all" CACHE STRING "" FORCE)
else()
    set(CMAKE_C_FLAGS
        "${COMMON_C_FLAGS} -O0 -g" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS
        "${COMMON_CXX_FLAGS} -O0 -g" CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS
        "${COMMON_SHARED_LINKER_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS
        "${COMMON_EXE_LINKER_FLAGS}" CACHE STRING "" FORCE)
endif()
