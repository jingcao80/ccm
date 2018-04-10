# ccm
CCM stands for C++ Component Model. It can:
1. support interface-oriented programming;
2. support C++ runtime reflection;

**ccdl** is the .cdl compiler.

**runtime** is the component driver.

### How to build
#### Build ccdl and runtime
step 1. <code>cd ccm</code>
step 2. <code>mkdir out</code>
step 3. <code>cd out</code>
step 4. <code>cmake ..</code>
step 5. <code>make</code>
step 6. <code>sudo make install</code>

#### Build sample
step 1. build ccdl and runtime
step 2. <code>cd ccm/src/sample/component</code>
step 3. <code>mkdir out</code>
step 4. <code>cd out</code>
step 5. <code>cmake ..</code>
step 6. <code>make</code>
step 7. <code>sudo make install</code>
step 7. <code>cd ccm/src/sample/client</code>
step 8. <code>mkdir out</code>
step 9. <code>cd out</code>
step 10. <code>cmake ..</code>
step 11. <code>make</code>
