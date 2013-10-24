%module(directors="1") exportvg

%{
#include <svgcanvas.h>
%}

%feature("director") GiCanvas;
%include <gicanvas.h>

%include <svgcanvas.h>