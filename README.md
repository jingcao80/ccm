# ccm
CCM stands for C++ Component Model. It can:
1. support interface-oriented programming;
2. support C++ runtime reflection;

**ccdl** is the .cdl compiler.

**runtime** is the component driver.

### How to build
#### Build ccdl and runtime
1. <code>cd ccm</code>
2. <code>mkdir out</code>
3. <code>cd out</code>
4. <code>cmake ..</code>
5. <code>make</code>
6. <code>sudo make install</code>

#### Build sample
1. build ccdl and runtime
2. <code>cd ccm/src/sample/component</code>
3. <code>mkdir out</code>
4. <code>cd out</code>
5. <code>cmake ..</code>
6. <code>make</code>
7. <code>sudo make install</code>
7. <code>cd ccm/src/sample/client</code>
8. <code>mkdir out</code>
9. <code>cd out</code>
10. <code>cmake ..</code>
11. <code>make</code>
12. <code>sudo make install</code>

#### Run sample
1. <code>cd ccm/bin</code>
2. <code>./demo</code>
