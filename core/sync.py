#!/usr/bin/env python
# -*- coding: utf-8 -*-
# sync.py: Find and copy source files.
# Usage:
#   Type './sync.py' to copy files from this repo to vgcore and vgios.
#   Type './sync.py -reset' to copy files from vgcore and vgios to this repo.
# Author: rhcad <github.com/rhcad>

import os, sys, shutil, filecmp

def replacefiles(srcdir, dstdir, fn):
    if fn=='build' or fn=='Pods' or fn.endswith('.tmp'):
        return
    srcfile = os.path.join(srcdir, fn)
    dstfile = os.path.join(dstdir, fn.lower())
    if os.path.isfile(dstfile) and os.path.exists(dstfile) \
        and (fn.endswith('.cpp') or fn.endswith('.h') or fn.endswith('.mm') or fn.endswith('.m')) \
        and not filecmp.cmp(srcfile, dstfile):
        os.remove(dstfile)
        shutil.copy(srcfile, os.path.join(dstdir, fn))
        print(os.path.join(dstdir, fn))
        return
    for fn2 in os.listdir(dstdir):
        if fn2=='build' or fn2.endswith('.tmp'):
            continue
        dstfile = os.path.join(dstdir, fn2)
        if os.path.isdir(dstfile):
            replacefiles(srcdir, dstfile, fn)

def syncdir(srcdir, dstdir):
    for fn in os.listdir(srcdir):
        if fn=='build' or fn.endswith('.tmp'):
            continue
        srcfile = os.path.join(srcdir, fn)
        if os.path.isfile(srcfile):
            replacefiles(srcdir, dstdir, fn)
        else:
            syncdir(srcfile, dstdir)

if __name__=="__main__":
    if len(sys.argv) > 1 and sys.argv[1]=='-reset':
        syncdir(os.path.abspath('../../vgcore/core'), os.path.abspath('.'))
        syncdir(os.path.abspath('../../vgios'), os.path.abspath('../ios'))
        syncdir(os.path.abspath('../../vgandroid'), os.path.abspath('../android'))
    else:
        syncdir(os.path.abspath('.'), os.path.abspath('../../vgcore/core'))
        syncdir(os.path.abspath('../ios'), os.path.abspath('../../vgios'))
        syncdir(os.path.abspath('../android'), os.path.abspath('../../vgandroid'))
