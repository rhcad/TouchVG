#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

def rminc(dir):
    for fn in os.listdir(dir):
        sfile = os.path.join(dir, fn)
        if os.path.isdir(sfile):
            rminc(sfile)
            continue
        if fn.endswith('.h') or fn.endswith('.m') or fn.endswith('.mm'):
            text = open(sfile).read()
            newtext = text.replace('#import <UIKit/UIKit.h>', '')
            newtext = newtext.replace('#import <Foundation/Foundation.h>', '')
            newtext = newtext.replace('#import <QuartzCore/QuartzCore.h>', '')
            if (newtext != text):
                open(sfile, 'w').write(newtext)

if __name__=="__main__":
    rminc(os.path.abspath('..'))
