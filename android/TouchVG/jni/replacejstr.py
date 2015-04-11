#!/usr/bin/env python
import os
file = os.path.abspath('touchvg_java_wrap.cpp')
text = open(file).read()

text = text.replace('swig_disconnect_director_self("swigDirectorDisconnect");', \
                    '//swig_disconnect_director_self("swigDirectorDisconnect");')
open(file, 'w').write(text)
