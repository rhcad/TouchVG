#!/usr/bin/env python
# addlog.py: Add logging entry for each JNI function
# author: Zhang Yungui <github.com/rhcad>
# Usage:
# 1. Call `python addlog.py` in jni/build.sh
# 2. Then type `./build.sh -swig` to re-generate touchvg_java_wrap.cpp

import os, re

file = os.path.abspath('touchvg_java_wrap.cpp')
text = open(file).read()
blks = []
prog = re.compile(r'(SWIGEXPORT (void|j).*{)|( SwigDirector_.*{)')
funp = re.compile(r'( Java_.*\()|( SwigDirector_.*\()')
# SWIGEXPORT void JNICALL Java_rhcad_touchvg_core_touchvgJNI_MgMotion_1pressDrag_1set(...) {
# bool SwigDirector_MgBaseShape::draw2(...) const {

while True:
    match = prog.search(text)
    if match:
        pos = match.end() + 1
        mfn = funp.search(text)
        name = text[mfn.start()+1:mfn.end()-1]
        blks.append(text[:pos])
        blks.append(r'  TmpLogFunc logf("' + name + '\");\n')
        text = text[pos:]
    else:
        blks.append(text)
        break

text = ''.join(blks)
open(file, 'w').write(text)
