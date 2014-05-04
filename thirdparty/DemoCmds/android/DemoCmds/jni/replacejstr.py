#!/usr/bin/env python
import os
file = os.path.abspath('democmds_java_wrap.cpp')
text = open(file).read()

text = text.replace('jstring jname = 0 ;', 'jstring jname = 0; TmpJOBJ jname_(jenv, &jname);')
text = text.replace('jstring jtext = 0 ;', 'jstring jtext = 0; TmpJOBJ jtext_(jenv, &jtext);')
text = text.replace('jstring jvalue = 0 ;', 'jstring jvalue = 0; TmpJOBJ jvalue_(jenv, &jvalue);')
text = text.replace('jstring jerrdesc = 0 ;', 'jstring jerrdesc = 0; TmpJOBJ jerrdesc_(jenv, &jerrdesc);')

text = text.replace('swig_disconnect_director_self("swigDirectorDisconnect");', \
                    '//swig_disconnect_director_self("swigDirectorDisconnect");')
open(file, 'w').write(text)
