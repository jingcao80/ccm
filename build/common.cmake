
add_compile_options(
    -std=c++11
    -fPIC
    -fvisibility=hidden
    -fno-exceptions
    -fno-rtti
    -ffunction-sections
    -fdata-sections
    -funwind-tables)

set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -fPIC -Wl,--gc-sections -Wl,--no-undefined,--no-undefined-version -Wl,--hash-style=gnu")

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Bdynamic -pie -Wl,--gc-sections -Wl,-z,nocopyreloc")
