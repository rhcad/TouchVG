#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

def utf8togbk(dir):
    for fn in os.listdir(dir):
        sfile = os.path.join(dir, fn)
        if os.path.isdir(sfile):
            utf8togbk(sfile)
            continue
        if '.htm' in fn:
            try:
                text = open(sfile,'r',-1,'utf-8').read()
            except UnicodeDecodeError:
                continue
            except TypeError:
                text = open(sfile).read()
                try:
                    u = text.decode('utf-8')
                    text = u.encode('gbk')
                except UnicodeDecodeError:
                    continue
            newtext = text.replace('html;charset=UTF-8', 'html;charset=gbk')
            if (newtext != text):
                open(sfile, 'w').write(newtext)
                print(sfile)

if __name__=="__main__":
    utf8togbk(os.path.abspath('..'))
