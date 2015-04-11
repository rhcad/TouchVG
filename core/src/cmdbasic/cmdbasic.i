%module(directors="1") cmdbasic

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

#include <mgvector.h>
#include <mgsnap.h>
#include <mgselect.h>
#include <mgaction.h>
#include <mgcmd.h>
#include <cmdsubject.h>

#include <mgdrawarc.h>
#include <mgdrawrect.h>

#include <cmdbasic.h>
#include <mgdrawellipse.h>
#include <mgdrawcircle.h>
#include <mgdrawdiamond.h>
#include <mgdrawfreelines.h>
#include <mgdrawgrid.h>
#include <mgdrawline.h>
#include <mgdrawlines.h>
#include <mgdrawparallel.h>
#include <mgdrawpolygon.h>
#include <mgdrawquadrangle.h>
#include <mgdrawsplines.h>
#include <mgdrawsquare.h>
#include <mgdrawtriang.h>
#include <mgbasicspreg.h>
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

%include <mgvector.h>
%template(Ints) mgvector<int>;
%template(ConstShapes) mgvector<const MgShape*>;
%template(Shapes) mgvector<MgShape*>;

%include <mgaction.h>
%include <mgsnap.h>
%include <mgselect.h>
%include <mgview.h>

%feature("director") MgCommand;
%include <mgcmd.h>

%feature("director") CmdObserverDefault;
%include <cmdobserver.h>
%include <cmdsubject.h>

%feature("director") MgCommandDraw;
%feature("director") MgCmdDrawRect;
%include <mgcmddraw.h>
%include <mgdrawarc.h>
%include <mgdrawrect.h>

%include <cmdbasic.h>
%include <mgdrawellipse.h>
%include <mgdrawcircle.h>
%include <mgdrawdiamond.h>
%include <mgdrawfreelines.h>
%include <mgdrawgrid.h>
%include <mgdrawline.h>
%include <mgdrawlines.h>
%include <mgdrawparallel.h>
%include <mgdrawpolygon.h>
%include <mgdrawquadrangle.h>
%include <mgdrawsplines.h>
%include <mgdrawsquare.h>
%include <mgdrawtriang.h>
%include <mgbasicspreg.h>
