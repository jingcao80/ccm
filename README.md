# ccm [![Build Status](https://travis-ci.org/jingcao80/ccm.svg?branch=master)](https://travis-ci.org/jingcao80/ccm)

CCM stands for C++ Component Model. It can:
1. support interface-oriented programming;
2. support C++ runtime reflection;

**ccdl** is the .cdl compiler.

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

### How to build
1. <code>cd ccm</code>
2. <code>source build/envsetup.sh</code>
2. <code>mkdir out</code>
3. <code>cd out</code>
4. <code>cmake ..</code>
5. <code>make</code>

#### Rebuild
1. <code>make clobber</code>
2. <code>make</code>

### Run sample
1. <code>cd ccm/bin</code>
2. <code>./demo</code>
