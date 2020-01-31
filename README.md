# COMO [![Build Status](https://travis-ci.org/jingcao80/ccm.svg?branch=master)](https://travis-ci.org/jingcao80/ccm)

COMO is a C++ Component Model. It can:
1. support interface-oriented programming;
2. support C++ runtime reflection;

**cdlc** is the .cdl compiler.

**comort** is the como component runtime.

**libcore** is the core library.

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
1. <code>cd como</code>
2. <code>source build/envsetup.sh</code>
3. choose build target:
   + <code>comotools</code> to build tools
   + <code>como_linux_x64</code> to build como for linux x64
   + <code>como_android_aarch64</code> to build como for android aarch64
4. <code>build</code> or <code>build -j*n*</code> for release build

### Change build type
1. in any build target:
   + <code>debug</code> for debug build
   + <code>release</code> for release build

### Rebuild
1. <code>rebuild</code> or <code>rebuild -j*n*</code>

### Clobber
1. <code>clobber</code>

### Copy building results
if the build target is android aarch64, you need copy building results to the device.
+ <code>drop</code> or <code>drop all</code> to copy all modules
+ <code>drop core</code> to copy core modules
+ <code>drop test</code> to copy testcase modules

### Other commands
+ <code>root</code> to change to como root directory
+ <code>out</code> to change to out directory
+ <code>bin</code> to change to bin directory
