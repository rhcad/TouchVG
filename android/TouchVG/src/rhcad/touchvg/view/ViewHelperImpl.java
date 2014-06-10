//! \file ViewHelperImpl.java
//! \brief Android绘图视图辅助类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Locale;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.IViewHelper;
import rhcad.touchvg.core.CmdObserver;
import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiContext;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.MgFindImageCallback;
import rhcad.touchvg.core.MgView;
import rhcad.touchvg.view.internal.BaseViewAdapter;
import rhcad.touchvg.view.internal.BaseViewAdapter.StringCallback;
import rhcad.touchvg.view.internal.ImageCache;
import rhcad.touchvg.view.internal.LogHelper;
import rhcad.touchvg.view.internal.ResourceUtil;
import rhcad.touchvg.view.internal.ViewUtil;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;

/**
 * \ingroup GROUP_ANDROID
 * Android绘图视图辅助类
 */
public class ViewHelperImpl implements IViewHelper{
    private static final String TAG = "touchvg";
    private static final int JARVERSION = 12;
    private BaseGraphView mView;

    static {
        System.loadLibrary("touchvg");
        Log.i(TAG, "TouchVG V1.1." + JARVERSION + "." + GiCoreView.getVersion());
    }

    @Override
    public String getVersion() {
        return String.format(Locale.US, "1.1.%d.%d", JARVERSION, GiCoreView.getVersion());
    }

    //! 指定视图的构造函数
    public ViewHelperImpl(IGraphView view) {
        mView = (BaseGraphView)view;
    }

    //! 获取当前活动视图的默认构造函数
    public ViewHelperImpl() {
        mView = ViewUtil.activeView();
    }

    @Override
    public IGraphView activeView() {
        return ViewUtil.activeView();
    }

    @Override
    public IGraphView getGraphView() {
        return mView;
    }

    @Override
    public View getView() {
        return mView != null ? mView.getView() : null;
    }

    @Override
    public Context getContext() {
        return mView != null ? mView.getView().getContext() : null;
    }

    @Override
    public GiCoreView coreView() {
        return mView != null ? mView.coreView() : null;
    }

    @Override
    public int cmdViewHandle() {
        final GiCoreView v = mView != null ? mView.coreView() : null;
        return v != null ? v.viewAdapterHandle() : 0;
    }

    @Override
    public MgView cmdView() {
        return MgView.fromHandle(cmdViewHandle());
    }

    @Override
    public ViewGroup createSurfaceView(Context context, ViewGroup layout) {
        return createSurfaceView(context, layout, null);
    }

    @Override
    public ViewGroup createSurfaceView(Context context, ViewGroup layout, Bundle savedState) {
        final SFGraphView view = new SFGraphView(context, savedState);
        mView = view;
        layout.addView(view, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        createDynamicShapeView(context, layout, view);
        return layout;
    }

    @Override
    public ViewGroup createGraphView(Context context, ViewGroup layout) {
        return createGraphView(context, layout, null);
    }

    @Override
    public ViewGroup createGraphView(Context context, ViewGroup layout, Bundle savedState) {
        final StdGraphView view = new StdGraphView(context, savedState);
        mView = view;
        layout.addView(view, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        return layout;
    }

    @Override
    public ViewGroup createMagnifierView(Context context, ViewGroup layout, IGraphView mainView) {
        final SFGraphView view = new SFGraphView(context,
                (BaseGraphView) (mainView != null ? mainView : mView));
        mView = view;
        layout.addView(view, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        createDynamicShapeView(context, layout, view);
        return layout;
    }

    private void createDynamicShapeView(Context context, ViewGroup layout, IGraphView view) {
        final View dynview = view.createDynamicShapeView(context);
        if (dynview != null) {
            layout.addView(dynview, new LayoutParams(LayoutParams.MATCH_PARENT,
                    LayoutParams.MATCH_PARENT));
        }
    }

    @Override
    public void setExtraContextImages(Context context, int[] ids) {
        ResourceUtil.setExtraContextImages(context, ids);
    }

    @Override
    public String getCommand() {
        final BaseViewAdapter adapter = internalAdapter();
        return adapter != null ? adapter.getCommand() : "";
    }

    @Override
    public boolean isCommand(String name) {
        return mView != null && mView.coreView().isCommand(name);
    }

    @Override
    public boolean setCommand(String name) {
        boolean ret = false;

        if (mView != null) {
            ret = mView.coreView().setCommand(name);
            Log.d(TAG, "setCommand " + name + ": " + ret);
        }
        return ret;
    }

    @Override
    public boolean setCommand(String name, String params) {
        boolean ret = false;

        if (mView != null) {
            ret = mView.coreView().setCommand(name, params);
            Log.d(TAG, "setCommand " + name + params + ": " + ret);
        }
        return ret;
    }

    @Override
    public boolean switchCommand() {
        return mView != null && mView.coreView().switchCommand();
    }

    @Override
    public int getLineWidth() {
        return Math.round(mView.coreView().getContext(false).getLineWidth());
    }

    @Override
    public void setLineWidth(int w) {
        mView.coreView().getContext(true).setLineWidth(w, true);
        mView.coreView().setContext(GiContext.kLineWidth);
    }

    @Override
    public int getStrokeWidth() {
        float w = mView.coreView().getContext(false).getLineWidth();
        if (w < 0)
            return Math.round(-w);
        return Math.round(mView.coreView().calcPenWidth(mView.viewAdapter(), w));
    }

    @Override
    public void setStrokeWidth(int w) {
        mView.coreView().getContext(true).setLineWidth(-Math.abs(w), true);
        mView.coreView().setContext(GiContext.kLineWidth);
    }

    @Override
    public int getLineStyle() {
        return mView.coreView().getContext(false).getLineStyle();
    }

    @Override
    public void setLineStyle(int style) {
        mView.coreView().getContext(true).setLineStyle(style);
        mView.coreView().setContext(GiContext.kLineStyle);
    }

    @Override
    public int getLineColor() {
        return mView.coreView().getContext(false).getLineColor().getARGB();
    }

    @Override
    public void setLineColor(int argb) {
        mView.coreView().getContext(true).setLineARGB(argb);
        mView.coreView().setContext(argb == 0 ? GiContext.kLineARGB : GiContext.kLineRGB);
    }

    @Override
    public int getLineAlpha() {
        return mView.coreView().getContext(false).getLineColor().getA();
    }

    @Override
    public void setLineAlpha(int alpha) {
        mView.coreView().getContext(true).setLineAlpha(alpha);
        mView.coreView().setContext(GiContext.kLineAlpha);
    }

    @Override
    public int getFillColor() {
        return mView.coreView().getContext(false).getFillColor().getARGB();
    }

    @Override
    public void setFillColor(int argb) {
        mView.coreView().getContext(true).setFillARGB(argb);
        mView.coreView().setContext(argb == 0 ? GiContext.kFillARGB : GiContext.kFillRGB);
    }

    @Override
    public int getFillAlpha() {
        return mView.coreView().getContext(false).getFillColor().getA();
    }

    @Override
    public void setFillAlpha(int alpha) {
        mView.coreView().getContext(true).setFillAlpha(alpha);
        mView.coreView().setContext(GiContext.kFillAlpha);
    }

    @Override
    public void setContextEditing(boolean editing) {
        mView.coreView().setContextEditing(editing);
    }

    @Override
    public int addShapesForTest() {
        final LogHelper log = new LogHelper();
        synchronized (mView.coreView()) {
            return log.r(mView.coreView().addShapesForTest());
        }
    }

    @Override
    public void clearCachedData() {
        mView.coreView().clearCachedData();
    }

    @Override
    public boolean zoomToExtent() {
        return mView.coreView().zoomToExtent();
    }

    @Override
    public boolean zoomToModel(float x, float y, float w, float h) {
        return mView.coreView().zoomToModel(x, y, w, h);
    }

    @Override
    public PointF displayToModel(float x, float y) {
        final Floats pt = new Floats(x, y);
        if (mView.coreView().displayToModel(pt))
            return new PointF(pt.get(0), pt.get(1));
        return null;
    }

    @Override
    public RectF displayToModel(RectF rect) {
        final Floats box = new Floats(rect.left, rect.top, rect.right, rect.bottom);
        if (mView.coreView().displayToModel(box)) {
            return new RectF(box.get(0), box.get(1), box.get(2), box.get(3));
        }
        return rect;
    }

    @Override
    public boolean startUndoRecord(String path) {
        final BaseViewAdapter adapter = internalAdapter();

        if (adapter == null
                || adapter.getSavedState() != null
                || mView.coreView().isUndoRecording()
                || !deleteDirectory(new File(path))
                || !createDirectory(path, true)) {
            return false;
        }
        return adapter.startUndoRecord(path);
    }

    @Override
    public void stopUndoRecord() {
        final BaseViewAdapter adapter = internalAdapter();
        if (adapter != null) {
            adapter.stopRecord(true);
        }
    }

    @Override
    public boolean canUndo() {
        return mView != null && mView.coreView().canUndo();
    }

    @Override
    public boolean canRedo() {
        return mView != null && mView.coreView().canRedo();
    }

    @Override
    public void undo() {
        final BaseViewAdapter adapter = internalAdapter();
        if (adapter != null) {
            adapter.undo();
        }
    }

    @Override
    public void redo() {
        final BaseViewAdapter adapter = internalAdapter();
        if (adapter != null) {
            adapter.redo();
        }
    }

    @Override
    public boolean isRecording() {
        return mView != null && mView.coreView().isRecording();
    }

    @Override
    public boolean startRecord(String path) {
        final BaseViewAdapter adapter = internalAdapter();

        if (adapter == null
                || adapter.getSavedState() != null
                || mView.coreView().isRecording()
                || !deleteDirectory(new File(path))
                || !createDirectory(path, true)) {
            return false;
        }
        return adapter.startRecord(path);
    }

    @Override
    public void stopRecord() {
        final BaseViewAdapter adapter = internalAdapter();
        if (adapter != null) {
            adapter.stopRecord(false);
        }
    }

    @Override
    public boolean isPaused() {
        return mView != null && mView.coreView().isPaused();
    }

    @Override
    public boolean isPlaying() {
        return mView != null && mView.coreView().isPlaying();
    }

    @Override
    public int getRecordTicks() {
        return mView.coreView().getRecordTick(false, BaseViewAdapter.getTick());
    }

    private BaseViewAdapter internalAdapter() {
        if (mView == null || mView.getMainView() == null)
            return null;
        return (BaseViewAdapter)((BaseGraphView)mView.getMainView()).viewAdapter();
    }

    @Override
    public void setGestureEnable(boolean enabled) {
        mView.setGestureEnable(enabled);
    }

    @Override
    public void setZoomEnabled(boolean enabled) {
        mView.coreView().setZoomEnabled(mView.viewAdapter(), enabled);
    }

    @Override
    public void setBackgroundColor(int color) {
        if (mView != null) {
            getView().setBackgroundColor(color);
        }
    }

    @Override
    public void setBackgroundDrawable(Drawable background) {
        if (mView != null) {
            mView.setBackgroundDrawable(background);
        }
    }

    @Override
    public Bitmap snapshot(boolean transparent) {
        final GiCoreView v = mView.coreView();
        int doc, gs;

        synchronized (v) {
            doc = v.acquireFrontDoc();
            gs = v.acquireGraphics(mView.viewAdapter());
        }
        try {
            final LogHelper log = new LogHelper();
            final Bitmap bitmap = mView.snapshot(doc, gs, transparent);
            log.r(bitmap != null ? bitmap.getByteCount() : 0);
            return bitmap;
        } finally {
            GiCoreView.releaseDoc(doc);
            v.releaseGraphics(gs);
        }
    }

    @Override
    public Bitmap extentSnapshot(int spaceAround, boolean transparent) {
        final GiCoreView v = mView.coreView();
        int doc, gs;

        synchronized (v) {
            doc = v.acquireFrontDoc();
            gs = v.acquireGraphics(mView.viewAdapter());
        }
        try {
            return extentSnapshot(doc, gs, spaceAround, transparent);
        } finally {
            GiCoreView.releaseDoc(doc);
            v.releaseGraphics(gs);
        }
    }

    private Bitmap extentSnapshot(int doc, int gs, int spaceAround, boolean transparent) {
        final LogHelper log = new LogHelper();
        final Rect extent = getDisplayExtent();

        if (!extent.isEmpty()) {
            extent.inset(-spaceAround, -spaceAround);
            extent.intersect(0, 0, mView.getView().getWidth(), mView.getView().getHeight());
        }
        if (extent.isEmpty()) {
            return null;
        }

        final Bitmap viewBitmap = mView.snapshot(doc, gs, transparent);
        if (viewBitmap == null) {
            return null;
        }

        if (extent.width() == mView.getView().getWidth()
                && extent.height() == mView.getView().getHeight()) {
            log.r(viewBitmap.getByteCount());
            return viewBitmap;
        }

        final Bitmap realBitmap = Bitmap.createBitmap(viewBitmap, extent.left, extent.top,
                extent.width(), extent.height());

        viewBitmap.recycle();
        log.r(realBitmap.getByteCount());
        return realBitmap;
    }

    @Override
    public boolean exportExtentAsPNG(String filename, int spaceAround) {
        return savePNG(extentSnapshot(spaceAround, true), filename);
    }

    @Override
    public boolean exportPNG(String filename, boolean transparent) {
        return savePNG(snapshot(transparent), filename);
    }

    @Override
    public boolean exportPNG(String filename) {
        return savePNG(snapshot(true), filename);
    }

    private boolean savePNG(Bitmap bmp, String filename) {
        boolean ret = false;
        final LogHelper log = new LogHelper();

        if (bmp != null && filename != null && createDirectory(filename, false)) {
            synchronized (bmp) {
                try {
                    filename = addExtension(filename, ".png");
                    final FileOutputStream os = new FileOutputStream(filename);
                    ret = bmp.compress(Bitmap.CompressFormat.PNG, 100, os);
                    Log.d(TAG, "savePNG: " + filename + ", " + ret + ", "
                            + bmp.getWidth() + "x" + bmp.getHeight());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

        return log.r(ret);
    }

    @Override
    public boolean exportSVG(String filename) {
        final LogHelper log = new LogHelper();
        filename = addExtension(filename, ".svg");
        return log.r(mView.coreView().exportSVG(mView.viewAdapter(), filename) > 0);
    }

    @Override
    public int getShapeCount() {
        return mView.coreView().getShapeCount();
    }

    @Override
    public int getSelectedCount() {
        return mView.coreView().getSelectedShapeCount();
    }

    @Override
    public int getSelectedType() {
        return mView.coreView().getSelectedShapeType();
    }

    @Override
    public int getSelectedShapeID() {
        return mView.coreView().getSelectedShapeID();
    }

    @Override
    public int getChangeCount() {
        return mView.coreView().getChangeCount();
    }

    @Override
    public int getDrawCount() {
        return mView.coreView().getDrawCount();
    }

    @Override
    public Rect getDisplayExtent() {
        final Floats box = new Floats(4);
        if (mView.coreView().getDisplayExtent(box)) {
            return new Rect(Math.round(box.get(0)), Math.round(box.get(1)),
                    Math.round(box.get(2)), Math.round(box.get(3)));
        }
        return new Rect();
    }

    @Override
    public Rect getDisplayExtent(int doc, int gs) {
        final Floats box = new Floats(4);
        if (mView.coreView().getDisplayExtent(doc, gs, box)) {
            return new Rect(Math.round(box.get(0)), Math.round(box.get(1)),
                    Math.round(box.get(2)), Math.round(box.get(3)));
        }
        return new Rect();
    }

    @Override
    public Rect getBoundingBox() {
        final Floats box = new Floats(4);
        if (mView.coreView().getBoundingBox(box)) {
            return new Rect(Math.round(box.get(0)), Math.round(box.get(1)),
                    Math.round(box.get(2)), Math.round(box.get(3)));
        }
        return new Rect();
    }

    @Override
    public Rect getShapeBox(int sid) {
        final Floats box = new Floats(4);
        if (mView.coreView().getBoundingBox(box, sid)) {
            return new Rect(Math.round(box.get(0)), Math.round(box.get(1)),
                    Math.round(box.get(2)), Math.round(box.get(3)));
        }
        return new Rect();
    }

    private int acquireFrontDoc() {
        synchronized (mView.coreView()) {
            return mView.coreView().acquireFrontDoc();
        }
    }

    @Override
    public String getContent() {
        final LogHelper log = new LogHelper();
        int doc = acquireFrontDoc();
        final StringCallback c = new StringCallback();

        mView.coreView().getContent(doc, c);
        GiCoreView.releaseDoc(doc);

        return log.r(c.toString());
    }

    @Override
    public boolean setContent(String content) {
        final LogHelper log = new LogHelper();
        synchronized (mView.coreView()) {
            mView.getImageCache().clear();
            return log.r(mView.coreView().setContent(content));
        }
    }

    @Override
    public boolean loadFromFile(String vgfile) {
        return loadFromFile(vgfile, false);
    }

    @Override
    public boolean loadFromFile(String vgfile, boolean readOnly) {
        if (mView == null)
            return false;
        final LogHelper log = new LogHelper();
        vgfile = addExtension(vgfile, ".vg");
        synchronized (mView.coreView()) {
            mView.getImageCache().clear();
            return log.r(mView.coreView().loadFromFile(vgfile, readOnly));
        }
    }

    @Override
    public boolean saveToFile(String vgfile) {
        if (mView == null || vgfile == null)
            return false;

        int doc = acquireFrontDoc();
        boolean ret = saveToFile(vgfile, doc);
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    private boolean saveToFile(String vgfile, int doc) {
        final LogHelper log = new LogHelper();
        vgfile = addExtension(vgfile, ".vg");
        if (mView.coreView().getShapeCount(doc) == 0) {
            final File f = new File(vgfile);
            return log.r(!f.exists() || f.delete());
        } else {
            return log.r(createDirectory(vgfile, false)
                    && mView.coreView().saveToFile(doc, vgfile));
        }
    }

    @Override
    public void clearShapes() {
        synchronized (mView.coreView()) {
            mView.getImageCache().clear();
            mView.coreView().clear();
        }
    }

    //! 返回指定后缀名的文件名
    public static String addExtension(String filename, String ext) {
        if (filename != null && !filename.endsWith(ext)) {
            filename = filename.substring(0, filename.lastIndexOf('.')) + ext;
        }
        return filename;
    }

    //! 删除一个文件夹的所有内容
    public static boolean deleteDirectory(File path) {
        if (path.exists()) {
            final File[] files = path.listFiles();
            if (files != null) {
                int n = 0;
                for (File f : files) {
                    if (f.isDirectory()) {
                        if (!deleteDirectory(f)) {
                            Log.e(TAG, "Fail to delete folder: " + f.getPath());
                            return false;
                        }
                    } else {
                        if (!f.delete()) {
                            Log.e(TAG, "Fail to delete file: " + f.getPath());
                            return false;
                        } else {
                            n++;
                        }
                    }
                }
                if (n > 0) {
                    Log.d(TAG, n + " files deleted in " + path.getPath());
                }
            }
        }
        return !path.exists() || path.delete();
    }

    //! 创建指定的文件的上一级文件夹，如果自身是文件夹则也创建
    public static boolean createDirectory(String filename, boolean isDirectory) {
        final File file = new File(filename);
        final File pf = file.getParentFile();

        if (!pf.exists() && !pf.mkdirs()) {
            Log.e(TAG, "Fail to create folder: " + pf.getPath());
            return false;
        }
        if (isDirectory && !file.exists() && !file.mkdirs()) {
            Log.e(TAG, "Fail to create folder: " + filename);
            return false;
        }
        return true;
    }

    @Override
    public int insertSVGFromResource(String name) {
        int id = ResourceUtil.getResIDFromName(getContext(), "raw", name);
        name = ImageCache.SVG_PREFIX + name;
        final Drawable d = mView.getImageCache().addSVG(getContext().getResources(), id, name);
        synchronized (mView.coreView()) {
            return d == null ? 0 : mView.coreView().addImageShape(name, ImageCache.getWidth(d),
                    ImageCache.getHeight(d));
        }
    }

    @Override
    public int insertSVGFromResource(int id) {
        return insertSVGFromResource(ResourceUtil.getResName(getContext(), id));
    }

    @Override
    public int insertSVGFromResource(String name, int xc, int yc) {
        int id = ResourceUtil.getResIDFromName(getContext(), "raw", name);
        name = ImageCache.SVG_PREFIX + name;
        final Drawable d = mView.getImageCache().addSVG(getContext().getResources(), id, name);
        synchronized (mView.coreView()) {
            return d == null ? 0 : mView.coreView().addImageShape(name, xc, yc,
                    ImageCache.getWidth(d), ImageCache.getHeight(d), 0);
        }
    }

    @Override
    public int insertSVGFromResource(int id, int xc, int yc) {
        return insertSVGFromResource(ResourceUtil.getResName(getContext(), id), xc, yc);
    }

    @Override
    public int insertBitmapFromResource(String name) {
        int id = ResourceUtil.getDrawableIDFromName(getContext(), name);
        name = ImageCache.BITMAP_PREFIX + name;
        final Drawable d = mView.getImageCache().addBitmap(getContext().getResources(), id, name);
        synchronized (mView.coreView()) {
            return d == null ? 0 : mView.coreView().addImageShape(name, ImageCache.getWidth(d),
                    ImageCache.getHeight(d));
        }
    }

    @Override
    public int insertBitmapFromResource(int id) {
        return insertBitmapFromResource(ResourceUtil.getResName(getContext(), id));
    }

    @Override
    public int insertBitmapFromResource(String name, int xc, int yc) {
        int id = ResourceUtil.getDrawableIDFromName(getContext(), name);
        name = ImageCache.BITMAP_PREFIX + name;
        final Drawable d = mView.getImageCache().addBitmap(getContext().getResources(), id, name);
        synchronized (mView.coreView()) {
            return d == null ? 0 : mView.coreView().addImageShape(name, xc, yc,
                    ImageCache.getWidth(d), ImageCache.getHeight(d), 0);
        }
    }

    @Override
    public int insertBitmapFromResource(int id, int xc, int yc) {
        return insertBitmapFromResource(ResourceUtil.getResName(getContext(), id), xc, yc);
    }

    @Override
    public int insertImageFromFile(String filename) {
        return insertImageFromFile(filename, mView.getView().getWidth() / 2,
                mView.getView().getHeight() / 2, 0);
    }

    @Override
    public int insertImageFromFile(String filename, int xc, int yc, int tag) {
        final BaseViewAdapter adapter = internalAdapter();

        if (adapter == null || filename == null)
            return 0;

        final String name = filename.substring(filename.lastIndexOf('/') + 1).toLowerCase(Locale.US);
        Drawable d;

        if (name.endsWith(".svg")) {
            d = mView.getImageCache().addSVGFile(filename, name);
        } else {
            d = mView.getImageCache().addBitmapFile(getContext().getResources(), filename, name);
        }
        if (d != null) {
            final String destPath = adapter.getRecordPath();
            if (destPath != null) {
                ImageCache.copyFileTo(filename, destPath);
            }
            synchronized (mView.coreView()) {
                int w = ImageCache.getWidth(d);
                int h = ImageCache.getHeight(d);
                return mView.coreView().addImageShape(name, xc, yc, w, h, tag);
            }
        }
        return 0;
    }

    @Override
    public boolean hasImageShape() {
        int doc = acquireFrontDoc();
        boolean ret = mView.coreView().hasImageShape(doc);
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    @Override
    public int findShapeByImageID(String name) {
        int doc = acquireFrontDoc();
        int ret = mView.coreView().findShapeByImageID(doc, name);
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    @Override
    public int findShapeByTag(int tag) {
        int doc = acquireFrontDoc();
        int ret = mView.coreView().findShapeByTag(doc, tag);
        GiCoreView.releaseDoc(doc);
        return ret;
    }

    private class ImageFinder extends MgFindImageCallback {
        private ArrayList<Bundle> arr;

        public ImageFinder(ArrayList<Bundle> arr) {
            this.arr = arr;
        }

        @Override
        public void onFindImage(int sid, String name) {
            final Bundle b = new Bundle();
            b.putInt("id", sid);
            b.putString("name", name);
            b.putString("path", new File(getImagePath(), name).getPath());
            b.putString("rect", getShapeBox(sid).toString());
            this.arr.add(b);
        }
    }

    @Override
    public ArrayList<Bundle> getImageShapes() {
        final ArrayList<Bundle> arr = new ArrayList<Bundle>();
        int doc = acquireFrontDoc();
        coreView().traverseImageShapes(doc, new ImageFinder(arr));
        GiCoreView.releaseDoc(doc);
        return arr;
    }

    @Override
    public String getImagePath() {
        return mView.getImageCache().getImagePath();
    }

    @Override
    public void setImagePath(String path) {
        mView.getImageCache().setImagePath(path);
    }

    @Override
    public void close() {
        if (mView != null) {
            mView.onPause();
            mView.stop();
            if (mView.getView() != null) {
                final ViewGroup parent = (ViewGroup) mView.getView().getParent();
                if (parent != null)
                    parent.removeAllViews();
            }
            mView = null;
        }
    }

    @Override
    public void onDestroy() {
        final Context context = getContext();
        for (BaseGraphView view : ViewUtil.views()) {
            if (view.getView().getContext() == context) {
                view.stop();
            }
        }
    }

    @Override
    public boolean onPause() {
        Log.d(TAG, "onPause");
        final Context context = getContext();
        boolean ret = false;

        for (BaseGraphView view : ViewUtil.views()) {
            if (view.getView().getContext() == context) {
                ret = view.onPause() || ret;
            }
        }
        return ret;
    }

    @Override
    public boolean onResume() {
        Log.d(TAG, "onResume");
        final Context context = getContext();
        boolean ret = false;

        for (BaseGraphView view : ViewUtil.views()) {
            if (view.getView().getContext() == context) {
                ret = view.onResume() || ret;
            }
        }
        return ret;
    }

    @Override
    public void onSaveInstanceState(Bundle outState, String path) {
        final BaseViewAdapter adapter = internalAdapter();

        if (adapter == null) {
            Log.w(TAG, "onSaveInstanceState fail due to no view adapter");
        } else {
            final LogHelper log = new LogHelper();
            Bundle state = adapter.getSavedState();

            if (state == null) {
                final String filename = new File(path, "resume.vg").getPath();
                final String playFile = new File(path, "playresume.vg").getPath();

                state = new Bundle();
                adapter.onSaveInstanceState(state);
                if (mView.coreView().isPlaying()) {
                    if (saveToFile(playFile)) {
                        Log.d(TAG, "Auto save playing shapes to " + playFile);
                        state.putString("playFile", playFile);
                    }
                    if (saveToFile(filename, mView.coreView().backDoc())) {
                        Log.d(TAG, "Auto save to " + filename);
                        state.putString("bakFile", filename);
                    }
                } else {
                    if (saveToFile(filename)) {
                        Log.d(TAG, "Auto save to " + filename);
                        state.putString("bakFile", filename);
                    }
                }
                state.putBoolean("readOnly", cmdView().isReadOnly());
            }
            outState.putBundle("vg", state);
            log.r();
        }
    }

    @Override
    public void onRestoreInstanceState(Bundle savedState) {
        final BaseViewAdapter adapter = internalAdapter();
        final Bundle state = savedState.getBundle("vg");

        if (adapter != null && state != null) {
            final LogHelper log = new LogHelper();
            final String filename = state.getString("bakFile");
            boolean readOnly = state.getBoolean("readOnly");

            if (filename != null && loadFromFile(filename, readOnly)) {
                Log.d(TAG, "Auto load from " + filename);
            }
            adapter.onRestoreInstanceState(state);
            log.r();
        } else {
            Log.w(TAG, "onRestoreInstanceState fail, state:" + (state != null));
        }
    }

    //! 注册命令观察者
    public void registerCmdObserver(CmdObserver observer) {
        this.cmdView().getCmdSubject().registerObserver(observer);
    }

    //! 注销命令观察者
    public void unregisterCmdObserver(CmdObserver observer) {
        if (this.cmdView() != null) {
            this.cmdView().getCmdSubject().unregisterObserver(observer);
        }
    }
}
