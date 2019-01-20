#!/bin/bash

export ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

export RT_PATH=$ROOT/bin
export COMPONENT_PATH=$ROOT/bin
export CLASS_PATH=$ROOT/bin/libcore.so

if [ -z "$VERSION" ]; then
    export VERSION=rls
fi

function clean_bin()
{
    if [ -d "$ROOT/bin" ]; then
        cd $ROOT
        rm -fr $ROOT/bin
    fi

    mkdir $ROOT/bin
}

function clean_out()
{
    if [ -d "$ROOT/out" ]; then
        cd $ROOT
        rm -fr $ROOT/out
    fi

    mkdir $ROOT/out
}

function debug()
{
    echo "Change to debug build"
    if [ -f "$ROOT/out/CMakeCache.txt" ]; then
        clean_out
    fi
    cd $ROOT/out
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    export VERSION=dbg
}

function release()
{
    echo "Change to release build"
    if [ -f "$ROOT/out/CMakeCache.txt" ]; then
        clean_out
    fi
    cd $ROOT/out
    cmake -DCMAKE_BUILD_TYPE=Release ..
    export VERSION=rls
}

function build()
{
    cd $ROOT/out
    if [ ! -f "$ROOT/out/CMakeCache.txt" ]; then
        if [ "$VERSION" == "rls" ]; then
            release
        else
            debug
        fi
    fi
    if [ "$VERSION" == "rls" ]; then
        echo "Building release version"
    else
        echo "Building debug version"
    fi
    make $1
}

function clobber()
{
    clean_bin
    clean_out
    cd $ROOT/out
}

function rebuild()
{
    clobber
    cd $ROOT/out
    if [ "$VERSION" == "rls" ]; then
        release
    else
        debug
    fi
    build $1
}

clean_out
cd $ROOT/out
if [ "$VERSION" == "rls" ]; then
    release
else
    debug
fi
