%module(directors="1") shapedoc

%{
#include <mgbox.h>
#include <mgmat.h>
#include <mgbase.h>
#include <mgcurv.h>
#include <mglnrel.h>
#include <mgnear.h>

#include <gigraph.h>
#include <gicanvas.h>
#include <mgpath.h>

#include <mgshapes.h>
#include <mgbasesp.h>
#include <mgcomposite.h>
#include <mggrid.h>
#include <mgshapetype.h>
#include <mgstorage.h>
#include <mgspfactory.h>

#include <mglayer.h>
#include <mgshapedoc.h>
%}

%include <mgdef.h>
%include <mgtol.h>
%include <mgvec.h>
%include <mgpnt.h>
%include <mgmat.h>
%include <mgbox.h>

%include <mgbase.h>
%include <mgcurv.h>
%include <mglnrel.h>
%include <mgnear.h>

%feature("director") GiCanvas;
%include <gicanvas.h>

%include <gicolor.h>
%include <gicontxt.h>
%include <gixform.h>
%include <mgpath.h>
%include <gigraph.h>

%include <mgobject.h>
%include <mgshape.h>
%include <mgspfactory.h>
%include <mgshapes.h>
%include <mgbasesp.h>
%include <mgrect.h>
%include <mgcomposite.h>
%include <mggrid.h>
%include <mgshapetype.h>
%include <mgstorage.h>
%include <mgspfactory.h>

%include <mglayer.h>
%include <mgshapedoc.h>
