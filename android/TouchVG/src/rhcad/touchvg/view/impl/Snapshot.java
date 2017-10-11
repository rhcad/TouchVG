// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.impl;

import java.io.FileOutputStream;

import rhcad.touchvg.IViewHelper;
import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiContext;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.MgShape;
import rhcad.touchvg.core.MgShapeBit;
import rhcad.touchvg.core.MgShapes;
import rhcad.touchvg.view.BaseGraphView;
import rhcad.touchvg.view.internal.LogHelper;
import rhcad.touchvg.view.internal.BaseViewAdapter.StringCallback;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.util.Log;

public class Snapshot {
    private Snapshot() {
    }

    public static Bitmap snapshot(BaseGraphView view, boolean transparent) {
        if (view == null) {
            return null;
        }

        final GiCoreView v = view.coreView();
        int doc, gs;

        if (v == null) {
            return null;
        }
        synchronized (v) {
            doc = v.acquireFrontDoc();
            gs = v.acquireGraphics(view.viewAdapter());
        }
        try {
            final LogHelper log = new LogHelper();
            final Bitmap bitmap = view.snapshot(doc, gs, transparent);
            log.r(bitmap != null ? bitmap.getByteCount() : 0);
            return bitmap;
        } finally {
            GiCoreView.releaseDoc(doc);
            v.releaseGraphics(gs);
        }
    }

    public static Bitmap extentSnapshot(BaseGraphView view, int spaceAround, boolean transparent) {
        if (view == null) {
            return null;
        }

        final GiCoreView v = view.coreView();
        int doc, gs;

        if (v == null) {
            return null;
        }
        synchronized (v) {
            doc = v.acquireFrontDoc();
            gs = v.acquireGraphics(view.viewAdapter());
        }
        try {
            return extentSnapshot(view, doc, gs, spaceAround, transparent);
        } finally {
            GiCoreView.releaseDoc(doc);
            v.releaseGraphics(gs);
        }
    }

    public static Bitmap extentSnapshot(BaseGraphView view, int doc, int gs, int spaceAround, boolean transparent) {
        final LogHelper log = new LogHelper();
        final Rect extent = getDisplayExtent(view);

        if (!extent.isEmpty()) {
            extent.inset(-spaceAround, -spaceAround);
            extent.intersect(0, 0, view.getView().getWidth(), view.getView().getHeight());
        }
        if (extent.isEmpty()) {
            return null;
        }

        final Bitmap viewBitmap = view.snapshot(doc, gs, transparent);
        if (viewBitmap == null) {
            return null;
        }

        if (extent.width() == view.getView().getWidth()
                && extent.height() == view.getView().getHeight()) {
            log.r(viewBitmap.getByteCount());
            return viewBitmap;
        }

        final Bitmap realBitmap = Bitmap.createBitmap(viewBitmap, extent.left, extent.top,
                extent.width(), extent.height());

        viewBitmap.recycle();
        log.r(realBitmap.getByteCount());
        return realBitmap;
    }

    public static Bitmap snapshotWithShapes(IViewHelper hlp, IViewHelper tmphlp, int sid,
            int width, int height) {
        tmphlp.createDummyView(hlp.getContext(), width, height);
        final MgShapes srcs = hlp.cmdView().shapes();
        final MgShapes dests = tmphlp.cmdView().shapes();

        synchronized (hlp.coreView()) {
            final MgShape sp = srcs.findShape(sid);
            if (sp != null) {
                MgShape newsp = dests.addShape(sp);
                if (newsp != null) {
                    newsp.shape().setFlag(MgShapeBit.kMgHideContent, false);
                    if (newsp.context().getLineAlpha() > 0 && newsp.context().getLineAlpha() < 20) {
                        final GiContext ctx = newsp.context();
                        ctx.setLineAlpha(20);
                        newsp.setContext(ctx, GiContext.kLineAlpha);
                    }
                }
            }
        }

        tmphlp.zoomToExtent();
        Bitmap bitmap = tmphlp.snapshot(false);
        tmphlp.close();
        return bitmap;
    }

    public static Bitmap snapshotWithShapes(IViewHelper hlp, IViewHelper tmphlp, int width, int height) {
        tmphlp.createDummyView(hlp.getContext(), width, height);
        final MgShapes srcs = hlp.cmdView().shapes();
        final MgShapes dests = tmphlp.cmdView().shapes();

        synchronized (hlp.coreView()) {
            dests.copyShapes(srcs, false);
        }

        tmphlp.zoomToExtent();
        Bitmap bitmap = tmphlp.snapshot(false);
        tmphlp.close();
        return bitmap;
    }

    public static boolean exportExtentAsPNG(BaseGraphView view, String filename, int spaceAround) {
        return savePNG(extentSnapshot(view, spaceAround, true), filename);
    }

    public static boolean exportPNG(BaseGraphView view, String filename, boolean transparent) {
        return savePNG(snapshot(view, transparent), filename);
    }

    public static boolean exportPNG(BaseGraphView view, String filename) {
        return savePNG(snapshot(view, true), filename);
    }

    public static boolean savePNG(Bitmap bmp, String filename) {
        boolean ret = false;
        final LogHelper log = new LogHelper();

        if (bmp != null && filename != null && FileUtil.createDirectory(filename, false)) {
            synchronized (bmp) {
                try {
                    final FileOutputStream os = new FileOutputStream(FileUtil.addExtension(filename, ".png"));
                    ret = bmp.compress(Bitmap.CompressFormat.PNG, 100, os);
                    Log.d(FileUtil.TAG, "savePNG: " + filename + ", " + ret + ", " + bmp.getWidth()
                            + "x" + bmp.getHeight());
                } catch (Exception e) {
                    Log.e(FileUtil.TAG, "savePNG fail", e);
                }
            }
        }

        return log.r(ret);
    }

    public static boolean exportSVG(BaseGraphView view, String filename) {
        if (view == null) {
            return false;
        }
        final LogHelper log = new LogHelper();
        return log.r(view.coreView().exportSVG(view.viewAdapter(),
                FileUtil.addExtension(filename, ".svg")) > 0);
    }

    public static int importSVGPath(BaseGraphView view, int sid, String d) {
        if (view != null && d != null) {
            int newid = view.coreView().importSVGPath(view.coreView().backShapes(), sid, d);
            if (newid != 0) {
                view.viewAdapter().regenAll(true);
                return newid;
            }
        }
        return 0;
    }

    public static String exportSVGPath(BaseGraphView view, int sid) {
        final StringCallback c = new StringCallback();
        if (view != null) {
            view.coreView().exportSVGPath2(c, view.coreView().backShapes(), sid);
        }
        return c.toString();
    }

    public static Rect getViewBox(BaseGraphView view) {
        final Floats box = new Floats(4);
        return toBox(box, view != null && view.coreView().getViewModelBox(box));
    }

    public static Rect getModelBox(BaseGraphView view) {
        final Floats box = new Floats(4);
        return toBox(box, view != null && view.coreView().getModelBox(box));
    }

    public static Rect getDisplayExtent(BaseGraphView view) {
        final Floats box = new Floats(4);
        return toBox(box, view != null && view.coreView().getDisplayExtent(box));
    }

    public static Rect getDisplayExtent(BaseGraphView view, int doc, int gs) {
        final Floats box = new Floats(4);
        return toBox(box, view != null && view.coreView().getDisplayExtent(doc, gs, box));
    }

    public static Rect getBoundingBox(BaseGraphView view) {
        final Floats box = new Floats(4);
        return toBox(box, view != null && view.coreView().getBoundingBox(box));
    }

    private static Rect toBox(Floats box, boolean ret) {
        return !ret ? new Rect() : new Rect(Math.round(box.get(0)), Math.round(box.get(1)),
                Math.round(box.get(2)), Math.round(box.get(3)));
    }

}
