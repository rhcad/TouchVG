%module(directors="1") json

%{
#include <mgstoragebs.h>
#include <mgjsonstorage.h>
%}

%include <mgvector.h>
%template(Ints) mgvector<int>;
%template(Floats) mgvector<float>;
%template(Chars) mgvector<char>;

%feature("director") MgStorageBase;
%include <mgstorage.h>
%include <mgstoragebs.h>

%include <mgjsonstorage.h>