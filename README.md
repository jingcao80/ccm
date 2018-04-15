# ccm
CCM stands for C++ Component Model. It can:
1. support interface-oriented programming;
2. support C++ runtime reflection;

**ccdl** is the .cdl compiler.

**runtime** is the component driver.

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
