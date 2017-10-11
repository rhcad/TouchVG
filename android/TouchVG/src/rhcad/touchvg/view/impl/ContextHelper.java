// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.impl;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiContext;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.MgFindImageCallback;
import rhcad.touchvg.core.MgOptionCallback;
import rhcad.touchvg.view.internal.BaseViewAdapter;
import rhcad.touchvg.view.internal.BaseViewAdapter.StringCallback;
import rhcad.touchvg.view.internal.ImageCache;
import rhcad.touchvg.view.internal.LogHelper;
import rhcad.touchvg.view.internal.ResourceUtil;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;

public class ContextHelper {
    private static final String KEY_BAKFILE = "bakFile";
    private static final String KEY_READONLY = "readOnly";
    private static final String KEY_PLAYFILE = "playFile";

    private ContextHelper() {
    }

    public static String getCommand(ViewCreator vc) {
        final BaseViewAdapter adapter = vc.getMainAdapter();
        return adapter != null ? adapter.getCommand() : "";
    }

    public static boolean isCommand(ViewCreator vc, String name) {
        return vc.isValid() && vc.coreView().isCommand(name);
    }

    public static boolean setCommand(ViewCreator vc, String name) {
        boolean ret = false;

        if (vc.isValid()) {
            ret = vc.coreView().setCommand(name);
            Log.d(FileUtil.TAG, "setCommand " + name + ": " + ret);
        }
        return ret;
    }

    public static boolean setCommand(ViewCreator vc, String name, String params) {
        boolean ret = false;

        if (vc.isValid()) {
            ret = vc.coreView().setCommand(name, params);
            Log.d(FileUtil.TAG, "setCommand(" + name + "): " + ret + params);
        }
        return ret;
    }

    public static boolean switchCommand(ViewCreator vc) {
        return vc.isValid() && vc.coreView().switchCommand();
    }

    private static class OptionCallback extends MgOptionCallback {
        private Map<String, String> dict;

        public OptionCallback(Map<String, String> dict) {
            this.dict = dict;
        }

        @Override
        public void onGetOptionBool(String name, boolean value) {
            dict.put(name, value ? "1" : "0");
        }

        @Override
        public void onGetOptionInt(String name, int value) {
            dict.put(name, String.valueOf(value));
        }

        @Override
        public void onGetOptionFloat(String name, float value) {
            dict.put(name, String.valueOf(value));
        }

        @Override
        public void onGetOptionString(String name, String text) {
            dict.put(name, text);
        }
    }

    public static Map<String, String> getOptions(ViewCreator vc) {
        Map<String, String> options = new HashMap<String, String>();
        if (vc.isValid()) {
            final OptionCallback c = new OptionCallback(options);
            vc.coreView().traverseOptions(c);
            c.onGetOptionBool("zoomEnabled",
                    vc.coreView().isZoomEnabled(vc.getGraphView().viewAdapter()));
            c.onGetOptionBool("contextActionEnabled",
                    vc.getMainAdapter().getContextActionEnabled());
        }
        return options;
    }

    public static int getLineWidth(ViewCreator vc) {
        return vc.isValid() ? Math.round(vc.coreView().getContext(false).getLineWidth()) : 0;
    }

    public static void setLineWidth(ViewCreator vc, int w) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setLineWidth(w, true);
            vc.coreView().setContext(GiContext.kLineWidth);
        }
    }

    public static int getStrokeWidth(ViewCreator vc) {
        if (!vc.isValid()) {
            return 0;
        }
        float w = vc.coreView().getContext(false).getLineWidth();
        if (w < 0) {
            return Math.round(-w);
        }
        return Math.round(vc.coreView().calcPenWidth(vc.getGraphView().viewAdapter(), w));
    }

    public static void setStrokeWidth(ViewCreator vc, int w) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setLineWidth(-Math.abs(w), true);
            vc.coreView().setContext(GiContext.kLineWidth);
        }
    }

    public static int getLineStyle(ViewCreator vc) {
        return vc.isValid() ? vc.coreView().getContext(false).getLineStyle() : 0;
    }

    public static void setLineStyle(ViewCreator vc, int style) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setLineStyle(style);
            vc.coreView().setContext(GiContext.kLineStyle);
        }
    }

    public static int getStartArrowHead(ViewCreator vc) {
        return vc.isValid() ? vc.coreView().getContext(false).getStartArrayHead() : 0;
    }

    public static void setStartArrowHead(ViewCreator vc, int style) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setStartArrayHead(style);
            vc.coreView().setContext(GiContext.kLineArrayHead);
        }
    }

    public static int getEndArrowHead(ViewCreator vc) {
        return vc.isValid() ? vc.coreView().getContext(false).getEndArrayHead() : 0;
    }

    public static void setEndArrowHead(ViewCreator vc, int style) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setEndArrayHead(style);
            vc.coreView().setContext(GiContext.kLineArrayHead);
        }
    }

    public static int getLineColor(ViewCreator vc) {
        return vc.isValid() ? vc.coreView().getContext(false).getLineColor().getARGB() : 0;
    }

    public static void setLineColor(ViewCreator vc, int argb) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setLineARGB(argb);
            vc.coreView().setContext(argb == 0 ? GiContext.kLineARGB : GiContext.kLineRGB);
        }
    }

    public static int getLineAlpha(ViewCreator vc) {
        return vc.isValid() ? vc.coreView().getContext(false).getLineColor().getA() : 0;
    }

    public static void setLineAlpha(ViewCreator vc, int alpha) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setLineAlpha(alpha);
            vc.coreView().setContext(GiContext.kLineAlpha);
        }
    }

    public static int getFillColor(ViewCreator vc) {
        return vc.isValid() ? vc.coreView().getContext(false).getFillColor().getARGB() : 0;
    }

    public static void setFillColor(ViewCreator vc, int argb) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setFillARGB(argb);
            vc.coreView().setContext(argb == 0 ? GiContext.kFillARGB : GiContext.kFillRGB);
        }
    }

    public static int getFillAlpha(ViewCreator vc) {
        return vc.isValid() ? vc.coreView().getContext(false).getFillColor().getA() : 0;
    }

    public static void setFillAlpha(ViewCreator vc, int alpha) {
        if (vc.isValid()) {
            vc.coreView().getContext(true).setFillAlpha(alpha);
            vc.coreView().setContext(GiContext.kFillAlpha);
        }
    }

    public static void setContextEditing(ViewCreator vc, boolean editing) {
        if (vc.isValid()) {
            vc.coreView().setContextEditing(editing);
        }
    }

    public static int addShapesForTest(ViewCreator vc) {
        final LogHelper log = new LogHelper();
        synchronized (vc.coreView()) {
            return log.r(vc.coreView().addShapesForTest());
        }
    }

    public static void clearCachedData(ViewCreator vc) {
        if (vc.isValid()) {
            vc.coreView().clearCachedData();
        }
    }

    public static boolean zoomToExtent(ViewCreator vc) {
        return vc.isValid() && vc.coreView().zoomToExtent();
    }

    public static boolean zoomToExtent(ViewCreator vc, float margin) {
        return vc.isValid() && vc.coreView().zoomToExtent(margin);
    }

    public static boolean zoomToModel(ViewCreator vc, float x, float y, float w, float h) {
        return vc.isValid() && vc.coreView().zoomToModel(x, y, w, h);
    }

    public static boolean zoomToModel(ViewCreator vc, float x, float y, float w, float h, float margin) {
        return vc.isValid() && vc.coreView().zoomToModel(x, y, w, h, margin);
    }

    public static boolean zoomPan(ViewCreator vc, float dxPixel, float dyPixel) {
        return vc.isValid() && vc.coreView().zoomPan(dxPixel, dyPixel);
    }

    public static boolean setViewScale(ViewCreator vc, float scale) {
        if (scale > 0 && vc.cmdView().xform().zoomScale(scale)) {
            vc.cmdView().regenAll(false);
            return true;
        }
        return false;
    }

    public static PointF displayToModel(ViewCreator vc, float x, float y) {
        final Floats pt = new Floats(x, y);
        if (vc.isValid() && vc.coreView().displayToModel(pt)) {
            return new PointF(pt.get(0), pt.get(1));
        }
        return null;
    }

    public static RectF displayToModel(ViewCreator vc, RectF rect) {
        final Floats box = new Floats(rect.left, rect.top, rect.right, rect.bottom);
        if (vc.isValid() && vc.coreView().displayToModel(box)) {
            return new RectF(box.get(0), box.get(1), box.get(2), box.get(3));
        }
        return rect;
    }

    public static String getContent(ViewCreator vc) {
        int doc = acquireFrontDoc(vc);
        if (doc == 0) {
            return null;
        }

        final LogHelper log = new LogHelper();
        final StringCallback c = new StringCallback();

        vc.coreView().getContent(doc, c);
        GiCoreView.releaseDoc(doc);

        return log.r(c.toString());
    }

    public static boolean setContent(ViewCreator vc, String content) {
        if (!vc.isValid()) {
            return false;
        }
        final LogHelper log = new LogHelper();
        synchronized (vc.coreView()) {
            vc.getGraphView().getImageCache().clear();
            return log.r(vc.coreView().setContent(content));
        }
    }

    public static boolean loadFromFile(ViewCreator vc, String vgfile, boolean readOnly) {
        if (!vc.isValid()) {
            return false;
        }
        final LogHelper log = new LogHelper();
        synchronized (vc.coreView()) {
            vc.getGraphView().getImageCache().clear();
            return log.r(vc.coreView().loadFromFile(FileUtil.addExtension(vgfile, ".vg"), readOnly));
        }
    }

    public static boolean saveToFile(ViewCreator vc, String vgfile) {
        if (!vc.isValid() || vgfile == null) {
            return false;
        }

        int doc = acquireFrontDoc(vc);
        boolean ret = saveToFile(vc, vgfile, doc);
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    private static boolean saveToFile(ViewCreator vc, String vgfile, int doc) {
        final LogHelper log = new LogHelper();
        if (vc.coreView().getShapeCount(doc) == 0) {
            final File f = new File(vgfile);
            return log.r(!f.exists() || f.delete());
        } else {
            return log.r(FileUtil.createDirectory(vgfile, false)
                    && vc.coreView().saveToFile(doc, FileUtil.addExtension(vgfile, ".vg")));
        }
    }

    public static void clearShapes(ViewCreator vc, boolean showMessage) {
        if (vc.isValid()) {
            synchronized (vc.coreView()) {
                vc.getGraphView().getImageCache().clear();
                if (showMessage) {
                    vc.coreView().clear();
                } else {
                    vc.coreView().loadShapes(null);
                }
            }
        }
    }

    public static void eraseView(ViewCreator vc) {
        if (vc.isValid()) {
            synchronized (vc.coreView()) {
                vc.coreView().setCommand("erasewnd");
            }
        }
    }

    public static int acquireFrontDoc(ViewCreator vc) {
        if (!vc.isValid()) {
            return 0;
        }
        synchronized (vc.coreView()) {
            return vc.coreView().acquireFrontDoc();
        }
    }

    private static int getRawResID(ViewCreator vc, String name) {
        return ResourceUtil.getResIDFromName(vc.getContext(), "raw", name);
    }

    public static int insertSVGFromResource(ViewCreator vc, String name) {
        if (!vc.isValid()) {
            return 0;
        }

        int id = getRawResID(vc, name);
        final String sname = ImageCache.SVG_PREFIX + name;
        final Drawable d = vc.getGraphView().getImageCache().addSVG(vc.getContext().getResources(), id, sname);

        synchronized (vc.coreView()) {
            return d == null ? 0 : vc.coreView().addImageShape(sname, ImageCache.getWidth(d),
                    ImageCache.getHeight(d));
        }
    }

    public static int insertSVGFromResource(ViewCreator vc, int id) {
        return insertSVGFromResource(vc, ResourceUtil.getResName(vc.getContext(), id));
    }

    public static int insertSVGFromResource(ViewCreator vc, String name, int xc, int yc) {
        if (!vc.isValid()) {
            return 0;
        }

        int id = getRawResID(vc, name);
        final String sname = ImageCache.SVG_PREFIX + name;
        final Drawable d = vc.getGraphView().getImageCache().addSVG(vc.getContext().getResources(), id, sname);

        synchronized (vc.coreView()) {
            return d == null ? 0 : vc.coreView().addImageShape(sname, xc, yc,
                    ImageCache.getWidth(d), ImageCache.getHeight(d), 0);
        }
    }

    public static int insertSVGFromResource(ViewCreator vc, int id, int xc, int yc) {
        return insertSVGFromResource(vc, ResourceUtil.getResName(vc.getContext(), id), xc, yc);
    }

    public static int insertBitmapFromResource(ViewCreator vc, String name) {
        if (!vc.isValid()) {
            return 0;
        }

        int id = ResourceUtil.getDrawableIDFromName(vc.getContext(), name);
        final String sname = ImageCache.BITMAP_PREFIX + name;
        final Drawable d = vc.getGraphView().getImageCache().addBitmap(vc.getContext().getResources(), id, sname);

        synchronized (vc.coreView()) {
            return d == null ? 0 : vc.coreView().addImageShape(sname, ImageCache.getWidth(d),
                    ImageCache.getHeight(d));
        }
    }

    public static int insertBitmapFromResource(ViewCreator vc, int id) {
        return insertBitmapFromResource(vc, ResourceUtil.getResName(vc.getContext(), id));
    }

    public static int insertBitmapFromResource(ViewCreator vc, String name, int xc, int yc) {
        if (!vc.isValid()) {
            return 0;
        }

        int id = ResourceUtil.getDrawableIDFromName(vc.getContext(), name);
        final String sname = ImageCache.BITMAP_PREFIX + name;
        final Drawable d = vc.getGraphView().getImageCache().addBitmap(vc.getContext().getResources(), id, sname);

        synchronized (vc.coreView()) {
            return d == null ? 0 : vc.coreView().addImageShape(sname, xc, yc,
                    ImageCache.getWidth(d), ImageCache.getHeight(d), 0);
        }
    }

    public static int insertBitmapFromResource(ViewCreator vc, int id, int xc, int yc) {
        return insertBitmapFromResource(vc, ResourceUtil.getResName(vc.getContext(), id), xc, yc);
    }

    public static int insertImageFromFile(ViewCreator vc, String filename) {
        if (!vc.isValid()) {
            return 0;
        }
        return insertImageFromFile(vc, filename,
                vc.getView().getWidth() / 2, vc.getView().getHeight() / 2, 0);
    }

    public static int insertImageFromFile(ViewCreator vc, String filename, int xc, int yc, int tag) {
        final BaseViewAdapter adapter = vc.getMainAdapter();

        if (adapter == null || filename == null) {
            return 0;
        }

        final String name = filename.substring(filename.lastIndexOf('/') + 1).toLowerCase(Locale.US);
        Drawable d;

        if (name.endsWith(".svg")) {
            d = vc.getGraphView().getImageCache().addSVGFile(filename, name);
        } else {
            d = vc.getGraphView().getImageCache().addBitmapFile(vc.getContext().getResources(), filename, name);
        }
        if (d != null) {
            final String destPath = adapter.getRecordPath();
            if (destPath != null) {
                ImageCache.copyFileTo(filename, destPath);
            }
            synchronized (vc.coreView()) {
                int w = ImageCache.getWidth(d);
                int h = ImageCache.getHeight(d);
                return vc.coreView().addImageShape(name, xc, yc, w, h, tag);
            }
        }
        return 0;
    }

    public static boolean getImageSize(ViewCreator vc, float[] info, int sid) {
        final Floats box = new Floats(info.length);
        boolean ret = (vc != null && info.length >= 5 &&  vc.coreView().getImageSize(box, sid));

        for (int i = 0; i < info.length; i++) {
            info[i] = box.get(i);
        }
        return ret;
    }

    public static boolean hasImageShape(ViewCreator vc) {
        int doc = acquireFrontDoc(vc);
        boolean ret = doc != 0 && vc.coreView().hasImageShape(doc);
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    public static int findShapeByImageID(ViewCreator vc, String name) {
        int doc = acquireFrontDoc(vc);
        int ret = vc.isValid() ? vc.coreView().findShapeByImageID(doc, name) : 0;
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    public static int findShapeByTag(ViewCreator vc, int tag) {
        int doc = acquireFrontDoc(vc);
        int ret = vc.isValid() ? vc.coreView().findShapeByTag(doc, tag) : 0;
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    private static class ImageFinder extends MgFindImageCallback {
        private ViewCreator vc;
        private List<Bundle> arr;

        public ImageFinder(ViewCreator vc, List<Bundle> arr) {
            this.vc = vc;
            this.arr = arr;
        }

        public void onFindImage(int sid, String name) {
            final Bundle b = new Bundle();
            b.putInt("id", sid);
            b.putString("name", name);
            b.putString("path", new File(ContextHelper.getImagePath(vc), name).getPath());
            b.putString("rect", ContextHelper.getShapeBox(vc, sid).toString());
            b.putParcelable("image", vc.getGraphView().getImageCache().getBitmap(name));
            this.arr.add(b);
        }
    }

    public static List<Bundle> getImageShapes(ViewCreator vc) {
        final List<Bundle> arr = new ArrayList<Bundle>();
        int doc = acquireFrontDoc(vc);
        vc.coreView().traverseImageShapes(doc, new ImageFinder(vc, arr));
        GiCoreView.releaseDoc(doc);
        return arr;
    }

    public static String getImagePath(ViewCreator vc) {
        return vc.isValid() ? vc.getGraphView().getImageCache().getImagePath() : null;
    }

    public static void setImagePath(ViewCreator vc, String path) {
        if (vc.isValid()) {
            vc.getGraphView().getImageCache().setImagePath(path);
        }
    }

    public static Rect getShapeBox(ViewCreator vc, int sid) {
        final Floats box = new Floats(4);
        if (vc != null && vc.coreView().getBoundingBox(box, sid)) {
            return new Rect(Math.round(box.get(0)), Math.round(box.get(1)), Math.round(box.get(2)),
                    Math.round(box.get(3)));
        }
        return new Rect();
    }

    public static RectF getModelBox(ViewCreator vc, int sid) {
        final Floats box = new Floats(4);
        if (vc != null && vc.coreView().getModelBox(box, sid)) {
            return new RectF(box.get(0), box.get(1), box.get(2), box.get(3));
        }
        return new RectF();
    }

    public static PointF getHandlePoint(ViewCreator vc, int sid, int index) {
        final Floats pt = new Floats(2);
        if (vc.isValid()) {
            vc.coreView().getHandlePoint(pt, sid, index);
        }
        return new PointF(pt.get(0), pt.get(1));
    }

    public static void onSaveInstanceState(ViewCreator vc, Bundle outState, String path) {
        final BaseViewAdapter adapter = vc.getMainAdapter();

        if (adapter == null) {
            Log.w(FileUtil.TAG, "onSaveInstanceState fail due to no view adapter");
        } else {
            final LogHelper log = new LogHelper();
            Bundle state = adapter.getSavedState();

            if (state == null) {
                state = new Bundle();
                adapter.onSaveInstanceState(state);
                onSaveNewState(vc, state, path);
                state.putBoolean(KEY_READONLY, vc.cmdView().isReadOnly());
            }
            outState.putBundle("vg", state);
            log.r();
        }
    }

    private static void onSaveNewState(ViewCreator vc, Bundle state, String path) {
        final String filename = new File(path, "resume.vg").getPath();
        final String playFile = new File(path, "playresume.vg").getPath();

        if (vc.coreView().isPlaying()) {
            if (saveToFile(vc, playFile)) {
                Log.d(FileUtil.TAG, "Auto save playing shapes to " + playFile);
                state.putString(KEY_PLAYFILE, playFile);
            }
            if (saveToFile(vc, filename, vc.coreView().backDoc())) {
                Log.d(FileUtil.TAG, "Auto save playing to " + filename);
                state.putString(KEY_BAKFILE, filename);
            }
        } else {
            if (saveToFile(vc, filename)) {
                Log.d(FileUtil.TAG, "Auto save to " + filename);
                state.putString(KEY_BAKFILE, filename);
            }
        }
    }

    public static void onRestoreInstanceState(ViewCreator vc, Bundle savedState) {
        final BaseViewAdapter adapter = vc.getMainAdapter();
        final Bundle state = savedState.getBundle("vg");

        if (adapter != null && state != null) {
            final LogHelper log = new LogHelper();
            final String filename = state.getString(KEY_BAKFILE);
            boolean readOnly = state.getBoolean(KEY_READONLY);

            if (filename != null && loadFromFile(vc, filename, readOnly)) {
                Log.d(FileUtil.TAG, "Auto load from " + filename);
            }
            adapter.onRestoreInstanceState(state);
            log.r();
        } else {
            Log.w(FileUtil.TAG, "onRestoreInstanceState fail, state:" + (state != null));
        }
    }
}
