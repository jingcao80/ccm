# COMO [![Build Status](https://travis-ci.org/jingcao80/ccm.svg?branch=master)](https://travis-ci.org/jingcao80/ccm)

COMO is a C++ Component Model. It can:
1. support interface-oriented programming;
2. support C++ runtime reflection;

**cdlc** is the .cdl compiler.

**runtime** is the component driver.

### Install cmake
1. <code>sudo apt-get install software-properties-common</code>
2. <code>sudo add-apt-repository ppa:george-edison55/cmake-3.x</code>
3. <code>sudo apt-get update</code>
4. <code>sudo apt-get install cmake</code>

### Install DBus
1. <code>sudo apt-get update</code>
2. <code>sudo apt-get install libdbus-1-dev</code>

### Install Unwind
1. <code>sudo apt-get install libunwind8-dev</code>

### Install ICU
1. <code>sudo apt-get install libicu-dev</code>

### How to build
1. <code>cd ccm</code>
2. <code>source build/envsetup.sh</code>
4. <code>build</code> or <code>build -j*n*</code> for release build

### Change build type
1. <code>release</code> for release build
    or
   <code>debug</code> for debug build

### Rebuild
1. <code>rebuild</code> or <code>rebuild -j*n*</code>

### Clobber
1. <code>clobber</code>

### Run sample
1. <code>cd ccm/bin</code>
2. <code>./demo</code>
