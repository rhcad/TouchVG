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
prog = re.compile(r'SWIGEXPORT (void|j).*{')
funp = re.compile(r'Java_.*\(')

funcstr = '''#include <android/log.h>
struct TmpLogFunc {
    const char* name;
    TmpLogFunc(const char* name) : name(name) {
        __android_log_print(ANDROID_LOG_VERBOSE,"vgstack","%s enter",name);
    }
    ~TmpLogFunc() {
        __android_log_print(ANDROID_LOG_VERBOSE,"vgstack","%s exit",name);
    }
};'''

text = text.replace('struct TmpJOBJ {',
                    funcstr + '\nstruct TmpJOBJ {')

while True:
    match = prog.search(text)
    if match:
        pos = match.end() + 1
        mfn = funp.search(text)
        name = text[mfn.start():mfn.end()-1]
        blks.append(text[:pos])
        blks.append(r'  TmpLogFunc logf("' + name + '\");\n')
        text = text[pos:]
    else:
        blks.append(text)
        break

text = ''.join(blks)
open(file, 'w').write(text)
