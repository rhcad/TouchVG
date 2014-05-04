#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Function:   Create a plugin project based on DemoCmds project.

Usage:      python newproj.py prjname
            or double click the file 'newproj.py'.

            prjname: name of the new project, the default value is 'EduCmds'.

Creator:    Zhang Yungui <rhcad@hotmail.com>
Date:       2014.5.4
"""

import os, sys, re, uuid

def multi_replace(text, adict):
    rx = re.compile('|'.join(map(re.escape, adict)))
    def xlat(match):
        return adict[match.group(0)]
    return rx.sub(xlat, text)

def copyfiles(srcdir, dstdir, pairs, callback):
    if srcdir.endswith(".git") or not os.path.exists(srcdir): return
    
    for fn in os.listdir(srcdir):
        srcfile = os.path.join(srcdir, fn)
        dstfile = os.path.join(dstdir, multi_replace(fn, pairs))
        
        if os.path.isdir(srcfile):
            copyfiles(srcfile, dstfile, pairs, callback)
            continue
        
        if not os.path.exists(dstfile) and callback(fn, pairs):
            if not os.path.exists(dstdir): os.makedirs(dstdir)
            open(dstfile, "wb").write(open(srcfile, "rb").read())
            isutf8 = False
            
            try:
                text = open(dstfile).read()
            except UnicodeDecodeError:
                try:
                    text = open(dstfile,'r',-1,'utf-8').read()
                    isutf8 = True
                except UnicodeDecodeError:
                    print("* Fail to read '%s' as utf-8 encoding." % (dstfile,))
                    continue
                
            newtext = multi_replace(text, pairs)
            if newtext != text:
                try:
                    if (isutf8):
                        open(dstfile, 'w',-1,'utf-8').write(newtext)
                        print('[replaced] %s [utf-8]' % (dstfile,))
                    else:
                        open(dstfile, 'w').write(newtext)
                        print('[replaced] %s' % (dstfile,))
                except UnicodeDecodeError:
                    open(dstfile, 'w',-1,'utf-8').write(newtext)
                    print('[replaced] %s [utf-8]' % (dstfile,))
            else:
                print('[created] %s' % (dstfile,))

def makeproj(prjname):
    srcdir   = os.path.abspath('DemoCmds')
    dstdir   = os.path.abspath(prjname)
    
    if prjname == '':
        print("Need input the project name.")
        return
    if not os.path.exists(srcdir):
        print("\nPlease input a valid exists template project name."
              "\n\'%s\' does not exist." % (srcdir,))
        return

    pairs = {'DemoCmds':prjname, 'democmds':prjname.lower(), 'DEMOCMDS':prjname.upper() }
    
    def matchfile(filename, pairs):
        return True
    copyfiles(srcdir, dstdir, pairs, matchfile)

if __name__=="__main__":
    def inputparam(index, prompt, default=''):
        if len(sys.argv) > index: ret = sys.argv[index]
        else: ret = input(prompt)
        if ret == '': ret = default
        return ret
    
    prjname = inputparam(1, 'New project name: ', 'EduCmds')
    
    makeproj(prjname)
    if len(sys.argv) < 2: input("Press <ENTER> to end.")
