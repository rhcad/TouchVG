%module(directors="1") graph

%{
#include <mgbox.h>
#include <mgmat.h>
#include <mgbnd.h>
#include <mgbase.h>
#include <mgcurv.h>
#include <mglnrel.h>
#include <mgnear.h>

#include <gigraph.h>
#include <gicanvas.h>
#include <gipath.h>
%}

%include <mgdef.h>
%include <mgtol.h>
%include <mgvec.h>
%include <mgpnt.h>
%include <mgmat.h>
%include <mgbox.h>
%include <mgbnd.h>

%include <mgbase.h>
%include <mgcurv.h>
%include <mglnrel.h>
%include <mgnear.h>

%feature("director") GiCanvas;
%include <gicanvas.h>

%include <gicolor.h>
%include <gicontxt.h>
%include <gixform.h>
%include <gipath.h>
%include <gigraph.h>
