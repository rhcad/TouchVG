//! \file ViewHelperImpl.java
//! \brief Android绘图视图辅助类
// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view;

import java.io.File;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.IViewHelper;
import rhcad.touchvg.core.CmdObserver;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.Ints;
import rhcad.touchvg.core.MgRegenLocker;
import rhcad.touchvg.core.MgView;
import rhcad.touchvg.core.Point2d;
import rhcad.touchvg.view.impl.ContextHelper;
import rhcad.touchvg.view.impl.FileUtil;
import rhcad.touchvg.view.impl.Snapshot;
import rhcad.touchvg.view.impl.ViewCreator;
import rhcad.touchvg.view.internal.BaseViewAdapter;
import rhcad.touchvg.view.internal.ResourceUtil;
import rhcad.touchvg.view.internal.ViewUtil;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

//! Android绘图视图辅助类
public class ViewHelperImpl implements IViewHelper {
    private static final String TAG = "touchvg";
    private static final int JARVERSION = 34;
    private ViewCreator mCreator = new ViewCreator();

    static {
        System.loadLibrary(TAG);
        Log.i(TAG, "TouchVG V1.1." + JARVERSION + "." + GiCoreView.getVersion());
    }

    //! 获取当前活动视图的默认构造函数
    public ViewHelperImpl() {
        mCreator.setGraphView(ViewUtil.activeView());
    }

    //! 指定视图的构造函数
    public ViewHelperImpl(IGraphView view) {
        mCreator.setGraphView(view);
    }

    @Override
    public String getVersion() {
        return String.format(Locale.US, "1.1.%d.%d", JARVERSION, GiCoreView.getVersion());
    }

    @Override
    public IGraphView activeView() {
        return ViewUtil.activeView();
    }

    @Override
    public IGraphView getGraphView() {
        return mCreator.getGraphView();
    }

    private BaseGraphView view() {
        return mCreator.getGraphView();
    }

    @Override
    public void setGraphView(IGraphView view) {
        mCreator.setGraphView(view);
    }

    @Override
    public View getView() {
        return mCreator.getView();
    }

    @Override
    public ViewGroup getParent() {
        return mCreator.getParent();
    }

    @Override
    public Context getContext() {
        return mCreator.getContext();
    }

    @Override
    public GiCoreView coreView() {
        return mCreator.coreView();
    }

    @Override
    public int cmdViewHandle() {
        return mCreator.cmdViewHandle();
    }

    @Override
    public MgView cmdView() {
        return mCreator.cmdView();
    }

    @Override
    public ViewGroup createSurfaceView(Context context, ViewGroup layout) {
        return createSurfaceView(context, layout, null);
    }

    @Override
    public ViewGroup createSurfaceView(Context context, ViewGroup layout, Bundle savedState) {
        return mCreator.createSurfaceView(context, layout, savedState);
    }

    @Override
    public ViewGroup createSurfaceAndImageView(Context context, ViewGroup layout, Bundle savedState) {
        return mCreator.createSurfaceAndImageView(context, layout, savedState);
    }

    @Override
    public ImageView getImageViewForSurface() {
        return getImageViewForSurface();
    }

    @Override
    public ViewGroup createGraphView(Context context, ViewGroup layout) {
        return createGraphView(context, layout, null);
    }

    @Override
    public ViewGroup createGraphView(Context context, ViewGroup layout, Bundle savedState) {
        return mCreator.createGraphView(context, layout, savedState);
    }

    @Override
    public ViewGroup createMagnifierView(Context context, ViewGroup layout, IGraphView mainView) {
        return mCreator.createMagnifierView(context, layout, mainView);
    }

    @Override
    public void createDummyView(Context context, int width, int height) {
        mCreator.createDummyView(context, width, height);
    }

    @Override
    public void setExtraContextImages(Context context, int[] ids) {
        ResourceUtil.setExtraContextImages(context, ids);
    }

    @Override
    public String getCommand() {
        return ContextHelper.getCommand(mCreator);
    }

    @Override
    public boolean isCommand(String name) {
        return ContextHelper.isCommand(mCreator, name);
    }

    @Override
    public boolean setCommand(String name) {
        return ContextHelper.setCommand(mCreator, name);
    }

    @Override
    public boolean setCommand(String name, String params) {
        return ContextHelper.setCommand(mCreator, name, params);
    }

    @Override
    public boolean switchCommand() {
        return ContextHelper.switchCommand(mCreator);
    }

    @Override
    public Map<String, String> getOptions() {
        return ContextHelper.getOptions(mCreator);
    }

    @Override
    public void setOption(String name, boolean value) {
        if (name == null) {
            coreView().setOptionBool(name, value);
        } else if (name.equals("zoomEnabled")) {
            setZoomEnabled(value);
        } else if (name.equals("contextActionEnabled")) {
            getGraphView().setContextActionEnabled(value);
        } else {
            coreView().setOptionBool(name, value);
        }
    }

    @Override
    public void setOption(String name, int value) {
        coreView().setOptionInt(name, value);
    }

    @Override
    public void setOption(String name, float value) {
        coreView().setOptionFloat(name, value);
    }

    @Override
    public void setOption(String name, String value) {
        coreView().setOptionString(name, value);
    }

    @Override
    public int getLineWidth() {
        return ContextHelper.getLineWidth(mCreator);
    }

    @Override
    public void setLineWidth(int w) {
        ContextHelper.setLineWidth(mCreator, w);
    }

    @Override
    public int getStrokeWidth() {
        return ContextHelper.getStrokeWidth(mCreator);
    }

    @Override
    public void setStrokeWidth(int w) {
        ContextHelper.setStrokeWidth(mCreator, w);
    }

    @Override
    public int getLineStyle() {
        return ContextHelper.getLineStyle(mCreator);
    }

    @Override
    public void setLineStyle(int style) {
        ContextHelper.setLineStyle(mCreator, style);
    }

    @Override
    public int getStartArrowHead() {
        return ContextHelper.getStartArrowHead(mCreator);
    }

    @Override
    public void setStartArrowHead(int style) {
        ContextHelper.setStartArrowHead(mCreator, style);
    }

    @Override
    public int getEndArrowHead() {
        return ContextHelper.getEndArrowHead(mCreator);
    }

    @Override
    public void setEndArrowHead(int style) {
        ContextHelper.setEndArrowHead(mCreator, style);
    }

    @Override
    public int getLineColor() {
        return ContextHelper.getLineColor(mCreator);
    }

    @Override
    public void setLineColor(int argb) {
        ContextHelper.setLineColor(mCreator, argb);
    }

    @Override
    public int getLineAlpha() {
        return ContextHelper.getLineAlpha(mCreator);
    }

    @Override
    public void setLineAlpha(int alpha) {
        ContextHelper.setLineAlpha(mCreator, alpha);
    }

    @Override
    public int getFillColor() {
        return ContextHelper.getFillColor(mCreator);
    }

    @Override
    public void setFillColor(int argb) {
        ContextHelper.setFillColor(mCreator, argb);
    }

    @Override
    public int getFillAlpha() {
        return ContextHelper.getFillAlpha(mCreator);
    }

    @Override
    public void setFillAlpha(int alpha) {
        ContextHelper.setFillAlpha(mCreator, alpha);
    }

    @Override
    public void setContextEditing(boolean editing) {
        ContextHelper.setContextEditing(mCreator, editing);
    }

    @Override
    public int addShapesForTest() {
        return ContextHelper.addShapesForTest(mCreator);
    }

    @Override
    public void clearCachedData() {
        ContextHelper.clearCachedData(mCreator);
    }

    @Override
    public boolean zoomToExtent() {
        return ContextHelper.zoomToExtent(mCreator);
    }

    @Override
    public boolean zoomToExtent(float margin) {
        return ContextHelper.zoomToExtent(mCreator, margin);
    }

    @Override
    public boolean zoomToModel(float x, float y, float w, float h) {
        return ContextHelper.zoomToModel(mCreator, x, y, w, h);
    }

    @Override
    public boolean zoomToModel(float x, float y, float w, float h, float margin) {
        return ContextHelper.zoomToModel(mCreator, x, y, w, h, margin);
    }

    @Override
    public boolean zoomPan(float dxPixel, float dyPixel) {
        return ContextHelper.zoomPan(mCreator, dxPixel, dyPixel);
    }

    @Override
    public PointF displayToModel(float x, float y) {
        return ContextHelper.displayToModel(mCreator, x, y);
    }

    @Override
    public RectF displayToModel(RectF rect) {
        return ContextHelper.displayToModel(mCreator, rect);
    }

    @Override
    public boolean startUndoRecord(String path) {
        final BaseViewAdapter adapter = mCreator.getMainAdapter();

        if (adapter == null || adapter.getSavedState() != null) {
            return false;
        }
        if (coreView().isUndoRecording()
                || !FileUtil.deleteDirectory(new File(path))
                || !FileUtil.createDirectory(path, true)) {
            return false;
        }
        return adapter.startUndoRecord(path);
    }

    @Override
    public void stopUndoRecord() {
        final BaseViewAdapter adapter = mCreator.getMainAdapter();
        if (adapter != null) {
            adapter.stopRecord(true);
        }
    }

    @Override
    public boolean canUndo() {
        return mCreator.isValid() && coreView().canUndo();
    }

    @Override
    public boolean canRedo() {
        return mCreator.isValid() && coreView().canRedo();
    }

    @Override
    public void undo() {
        final BaseViewAdapter adapter = mCreator.getMainAdapter();
        if (adapter != null) {
            adapter.undo();
        }
    }

    @Override
    public void redo() {
        final BaseViewAdapter adapter = mCreator.getMainAdapter();
        if (adapter != null) {
            adapter.redo();
        }
    }

    @Override
    public void combineRegen(Runnable action) {
        final MgRegenLocker locker = new MgRegenLocker(this.cmdView());
        action.run();
        locker.delete();
    }

    @Override
    public boolean isRecording() {
        return mCreator.isValid() && coreView().isRecording();
    }

    @Override
    public boolean startRecord(String path) {
        final BaseViewAdapter adapter = mCreator.getMainAdapter();

        if (adapter == null
                || adapter.getSavedState() != null) {
            return false;
        }
        if (coreView().isRecording()
                || !FileUtil.deleteDirectory(new File(path))
                || !FileUtil.createDirectory(path, true)) {
            return false;
        }
        return adapter.startRecord(path);
    }

    @Override
    public void stopRecord() {
        final BaseViewAdapter adapter = mCreator.getMainAdapter();
        if (adapter != null) {
            adapter.stopRecord(false);
        }
    }

    @Override
    public boolean isPaused() {
        return mCreator.isValid() && coreView().isPaused();
    }

    @Override
    public boolean isPlaying() {
        return mCreator.isValid() && coreView().isPlaying();
    }

    @Override
    public int getRecordTicks() {
        return mCreator.isValid() ? coreView().getRecordTick(false, BaseViewAdapter.getTick()) : 0;
    }

    @Override
    public boolean getGestureEnabled() {
        return mCreator.isValid() && mCreator.getGraphView().getGestureEnabled();
    }

    @Override
    public void setGestureEnabled(boolean enabled) {
        if (mCreator.isValid()) {
            mCreator.getGraphView().setGestureEnabled(enabled);
        }
    }

    @Override
    public void setVelocityTrackerEnabled(boolean enabled) {
        final BaseViewAdapter adapter = mCreator.getMainAdapter();
        if (adapter != null) {
            adapter.getGestureListener().setVelocityTrackerEnabled(enabled);
        }
    }

    @Override
    public boolean getZoomEnabled() {
        return mCreator.isValid()
                && coreView().isZoomEnabled(mCreator.getGraphView().viewAdapter());
    }

    @Override
    public void setZoomEnabled(boolean enabled) {
        if (mCreator.isValid()) {
            coreView().setZoomEnabled(mCreator.getGraphView().viewAdapter(), enabled);
        }
    }

    @Override
    public void setBackgroundColor(int color) {
        if (mCreator.isValid()) {
            getView().setBackgroundColor(color);
        }
    }

    @Override
    public void setBackgroundDrawable(Drawable background) {
        if (mCreator.isValid()) {
            mCreator.getGraphView().setBackgroundDrawable(background);
        }
    }

    @Override
    public Bitmap snapshot(boolean transparent) {
        return Snapshot.snapshot(view(), transparent);
    }

    @Override
    public Bitmap extentSnapshot(int spaceAround, boolean transparent) {
        return Snapshot.extentSnapshot(view(), spaceAround, transparent);
    }

    @Override
    public Bitmap snapshotWithShapes(int sid, int width, int height) {
        return Snapshot.snapshotWithShapes(this, new ViewHelperImpl(), sid, width, height);
    }

    @Override
    public Bitmap snapshotWithShapes(int width, int height) {
        return Snapshot.snapshotWithShapes(this, new ViewHelperImpl(), width, height);
    }

    @Override
    public boolean exportExtentAsPNG(String filename, int spaceAround) {
        return Snapshot.exportExtentAsPNG(view(), filename, spaceAround);
    }

    @Override
    public boolean exportPNG(String filename, boolean transparent) {
        return Snapshot.exportPNG(view(), filename, transparent);
    }

    @Override
    public boolean exportPNG(String filename) {
        return Snapshot.exportPNG(view(), filename);
    }

    @Override
    public boolean savePNG(Bitmap bmp, String filename) {
        return Snapshot.savePNG(bmp, filename);
    }

    @Override
    public boolean exportSVG(String filename) {
        return Snapshot.exportSVG(view(), filename);
    }

    @Override
    public int importSVGPath(int sid, String d) {
        return Snapshot.importSVGPath(view(), sid, d);
    }

    @Override
    public String exportSVGPath(int sid) {
        return Snapshot.exportSVGPath(view(), sid);
    }

    @Override
    public int getShapeCount() {
        return mCreator.isValid() ? coreView().getShapeCount() : 0;
    }

    @Override
    public int getUnlockedShapeCount() {
        return mCreator.isValid() ? coreView().getUnlockedShapeCount() : 0;
    }

    @Override
    public int getVisibleShapeCount() {
        return mCreator.isValid() ? coreView().getVisibleShapeCount() : 0;
    }

    @Override
    public int getSelectedCount() {
        return mCreator.isValid() ? coreView().getSelectedShapeCount() : 0;
    }

    @Override
    public int getSelectedType() {
        return mCreator.isValid() ? coreView().getSelectedShapeType() : 0;
    }

    @Override
    public int getSelectedShapeID() {
        return mCreator.isValid() ? coreView().getSelectedShapeID() : 0;
    }

    @Override
    public void setSelectedShapeID(int sid) {
        setCommand(String.format("select{'id':%d}", sid));
    }

    @Override
    public int[] getSelectedIds() {
        final Ints ids = new Ints();
        coreView().getSelectedShapeIDs(ids);
        final int[] arr = new int[ids.count()];
        for (int i = 0; i < arr.length; i++)
            arr[i] = ids.get(i);
        return arr;
    }

    @Override
    public void setSelectedIds(int[] ids) {
        int n = ids != null ? ids.length : 0;
        final Ints arr = new Ints(n);
        for (int i = 0; i < n; i++)
            arr.set(i, ids[i]);
        coreView().setSelectedShapeIDs(arr);
    }

    @Override
    public int getSelectedHandle() {
        return mCreator.isValid() ? coreView().getSelectedHandle() : 0;
    }

    @Override
    public int getChangeCount() {
        final GiCoreView v = coreView();
        if (v != null) {
            return v.getChangeCount();
        }
        return 0;
    }

    @Override
    public int getDrawCount() {
        return mCreator.isValid() ? coreView().getDrawCount() : 0;
    }

    @Override
    public Rect getViewBox() {
        return Snapshot.getViewBox(view());
    }

    @Override
    public float getViewScale() {
        return cmdView().xform().getViewScale();
    }

    @Override
    public boolean setViewScale(float scale) {
        return ContextHelper.setViewScale(mCreator, scale);
    }

    @Override
    public Rect getModelBox() {
        return Snapshot.getModelBox(view());
    }

    @Override
    public Rect getDisplayExtent() {
        return Snapshot.getDisplayExtent(view());
    }

    @Override
    public Rect getDisplayExtent(int doc, int gs) {
        return Snapshot.getDisplayExtent(view(), doc, gs);
    }

    @Override
    public Rect getBoundingBox() {
        return Snapshot.getBoundingBox(view());
    }

    @Override
    public Rect getShapeBox(int sid) {
        return ContextHelper.getShapeBox(mCreator, sid);
    }

    @Override
    public RectF getModelBox(int sid) {
        return ContextHelper.getModelBox(mCreator, sid);
    }

    @Override
    public Point getCurrentPoint() {
        Point2d pt = this.cmdView().motion().getPoint();
        return new Point(Math.round(pt.getX()), Math.round(pt.getY()));
    }

    @Override
    public PointF getCurrentModelPoint() {
        Point2d pt = this.cmdView().motion().getPointM();
        return new PointF(pt.getX(), pt.getY());
    }

    @Override
    public PointF getHandlePoint(int sid, int index) {
        return ContextHelper.getHandlePoint(mCreator, sid, index);
    }

    @Override
    public String getContent() {
        return ContextHelper.getContent(mCreator);
    }

    @Override
    public boolean setContent(String content) {
        return ContextHelper.setContent(mCreator, content);
    }

    @Override
    public boolean loadFromFile(String vgfile) {
        return ContextHelper.loadFromFile(mCreator, vgfile, false);
    }

    @Override
    public boolean loadFromFile(String vgfile, boolean readOnly) {
        return ContextHelper.loadFromFile(mCreator, vgfile, readOnly);
    }

    @Override
    public boolean saveToFile(String vgfile) {
        return ContextHelper.saveToFile(mCreator, vgfile);
    }

    @Override
    public void clearShapes() {
        ContextHelper.clearShapes(mCreator, true);
    }

    @Override
    public void clearShapes(boolean showMessage) {
        ContextHelper.clearShapes(mCreator, showMessage);
    }

    @Override
    public void eraseView() {
        ContextHelper.eraseView(mCreator);
    }

    @Override
    public int insertSVGFromResource(String name) {
        return ContextHelper.insertSVGFromResource(mCreator, name);
    }

    @Override
    public int insertSVGFromResource(int id) {
        return ContextHelper.insertSVGFromResource(mCreator, id);
    }

    @Override
    public int insertSVGFromResource(String name, int xc, int yc) {
        return ContextHelper.insertSVGFromResource(mCreator, name, xc, yc);
    }

    @Override
    public int insertSVGFromResource(int id, int xc, int yc) {
        return ContextHelper.insertSVGFromResource(mCreator, id, xc, yc);
    }

    @Override
    public int insertBitmapFromResource(String name) {
        return ContextHelper.insertBitmapFromResource(mCreator, name);
    }

    @Override
    public int insertBitmapFromResource(int id) {
        return ContextHelper.insertBitmapFromResource(mCreator, id);
    }

    @Override
    public int insertBitmapFromResource(String name, int xc, int yc) {
        return ContextHelper.insertBitmapFromResource(mCreator, name, xc, yc);
    }

    @Override
    public int insertBitmapFromResource(int id, int xc, int yc) {
        return ContextHelper.insertBitmapFromResource(mCreator, id, xc, yc);
    }

    @Override
    public int insertImageFromFile(String filename) {
        return ContextHelper.insertImageFromFile(mCreator, filename);
    }

    @Override
    public int insertImageFromFile(String filename, int xc, int yc, int tag) {
        return ContextHelper.insertImageFromFile(mCreator, filename, xc, yc, tag);
    }

    @Override
    public boolean getImageSize(float[] info, int sid) {
        return ContextHelper.getImageSize(mCreator, info, sid);
    }

    @Override
    public boolean hasImageShape() {
        return ContextHelper.hasImageShape(mCreator);
    }

    @Override
    public int findShapeByImageID(String name) {
        return ContextHelper.findShapeByImageID(mCreator, name);
    }

    @Override
    public int findShapeByTag(int tag) {
        return ContextHelper.findShapeByTag(mCreator, tag);
    }

    @Override
    public List<Bundle> getImageShapes() {
        return ContextHelper.getImageShapes(mCreator);
    }

    @Override
    public String getImagePath() {
        return ContextHelper.getImagePath(mCreator);
    }

    @Override
    public void setImagePath(String path) {
        ContextHelper.setImagePath(mCreator, path);
    }

    @Override
    public void close() {
        close(null);
    }

    @Override
    public void close(final IGraphView.OnViewDetachedListener listener) {
        mCreator.close(Snapshot.snapshot(mCreator.getGraphView(), false), listener);
    }

    @Override
    public void onDestroy() {
        mCreator.onDestroy();
    }

    @Override
    public boolean onPause() {
        return mCreator.onPause();
    }

    @Override
    public boolean onResume() {
        return mCreator.onResume();
    }

    @Override
    public void onSaveInstanceState(Bundle outState, String path) {
        ContextHelper.onSaveInstanceState(mCreator, outState, path);
    }

    @Override
    public void onRestoreInstanceState(Bundle savedState) {
        ContextHelper.onRestoreInstanceState(mCreator, savedState);
    }

    @Override
    public void showMessage(String text) {
        mCreator.getMainAdapter().showMessage(text);
    }

    @Override
    public String getLocalizedString(String name) {
        return ResourceUtil.getStringFromName(getContext(), name);
    }

    //! 注册命令观察者
    public void registerCmdObserver(CmdObserver observer) {
        if (this.cmdView() != null) {
            this.cmdView().getCmdSubject().registerObserver(observer);
        }
    }

    //! 注销命令观察者
    public void unregisterCmdObserver(CmdObserver observer) {
        if (this.cmdView() != null) {
            this.cmdView().getCmdSubject().unregisterObserver(observer);
        }
    }
}
