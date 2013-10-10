//! \file GraphView.java
//! \brief Android绘图视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package touchvg.view;

import java.util.ArrayList;

import touchvg.core.Floats;
import touchvg.core.GiCoreView;
import touchvg.core.GiView;
import touchvg.core.Ints;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

//! Android绘图视图类
/*! \ingroup GROUP_ANDROID
 *  建议使用FrameLayout作为容器创建绘图视图，使用LinearLayout将无法显示上下文操作按钮。
 */
public class GraphView extends View {
    private static GraphView mActiveView;   // 当前激活视图
    private CanvasAdapter mCanvasAdapter;   // onDraw用的画布适配器
    private CanvasAdapter mCanvasRegen;     // regen用的画布适配器
    private ViewAdapter mViewAdapter;       // 视图回调适配器
    private GiCoreView mCoreView;           // 内核视图分发器
    private GestureDetector mGestureDetector; // 手势识别器
    private GestureListener mGestureListener; // 手势识别实现
    private boolean mGestureEnable = true;  // 是否允许交互
    private boolean mRegenning = false;     // 是否正在regenAll
    private Bitmap mCachedBitmap;           // 缓存快照
    private Bitmap mRegenBitmap;            // regen用的缓存位图
    private int mBkColor = Color.TRANSPARENT;
    
    //! 普通绘图视图的构造函数
    public GraphView(Context context) {
        super(context);
        createAdapter(context);
        mCoreView = new GiCoreView(null);
        mCoreView.createView(mViewAdapter);
        initView(context);
        if (mActiveView == null) {
            mActiveView = this;
        }
    }
    
    //! 放大镜视图的构造函数
    public GraphView(Context context, GraphView mainView) {
        super(context);
        createAdapter(context);
        mCoreView = new GiCoreView(mainView.coreView());
        mCoreView.createMagnifierView(mViewAdapter, mainView.viewAdapter());
        initView(context);
    }
    
    private void createAdapter(Context context) {
        mCanvasAdapter = new CanvasAdapter(this);
        mCanvasRegen = new CanvasAdapter(this);
        mViewAdapter = new ViewAdapter();
    }
    
    private void initView(Context context) {
        mGestureListener = new GestureListener(mCoreView, mViewAdapter);
        mGestureDetector = new GestureDetector(context, mGestureListener);
        
        final DisplayMetrics dm = context.getApplicationContext().getResources().getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi);         // 应用API
        setLayerType(View.LAYER_TYPE_SOFTWARE, null);   // 避免路径太大不能渲染
        
        this.setOnTouchListener(new OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
                    activateView();
                }
                return mGestureEnable && (mGestureListener.onTouch(v, event)
                                          || mGestureDetector.onTouchEvent(event));
            }
        });
    }
    
    private void activateView() {
        mViewAdapter.removeContextButtons();
        if (mActiveView != this) {
            mActiveView = this;
        }
    }
    
    //! 返回当前激活视图
    public static GraphView activeView() {
        return mActiveView;
    }
    
    //! 返回内核视图分发器对象
    public GiCoreView coreView() {
        return mCoreView;
    }
    
    //! 返回视图回调适配器对象
    public GiView viewAdapter() {
        return mViewAdapter;
    }
    
    //! 释放临时缓存
    public void clearCachedData() {
        mCoreView.clearCachedData();
        if (mCachedBitmap != null) {
            mCachedBitmap.recycle();
            mCachedBitmap = null;
        }
    }
    
    //! 设置背景色
    public void setBackgroundColor(int color) {
        mBkColor = color;
        mCoreView.setBkColor(mViewAdapter, color);
        regen(false);
    }
    
    //! 设置上下文按钮的图像ID数组
    public static void setContextButtonImages(int[] imageIDs, int captionsID, 
            int[] extraImageIDs, int[] handleImageIDs) {
        ContextAction.setButtonImages(imageIDs, extraImageIDs);
        ContextAction.setButtonCaptionsID(captionsID);
        CanvasAdapter.setHandleImageIDs(handleImageIDs);
    }
    
    //! 设置是否允许触摸交互
    public void setGestureEnable(boolean enabled) {
        mGestureEnable = enabled;
        mGestureListener.setGestureEnable(enabled);
    }
    
    //! 得到静态图形的快照
    public Bitmap snapshot() {
        if (mCachedBitmap == null) {
            mCachedBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
            synchronized(mCachedBitmap) {
                mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
                mCachedBitmap.eraseColor(mBkColor);
                drawShapes(new Canvas(mCachedBitmap), mCanvasAdapter, false);
            }
        }
        return mCachedBitmap;
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
        mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
        if (mCachedBitmap != null) {
            drawShapes(canvas, mCanvasAdapter, true);
        }
        else if (!regen(false)) {       // 首次onDraw，但视图太大无法创建缓存位图
            canvas.drawColor(mBkColor);
            drawShapes(canvas, mCanvasAdapter, true);
        }
    }
    
    private int drawShapes(Canvas canvas, CanvasAdapter adapter, boolean dyndraw) {
        int n = 0;
        
        if (adapter.beginPaint(canvas)) {
            if (mCachedBitmap == null || !dyndraw) {
                if (this.getBackground() != null) {
                    this.getBackground().draw(canvas);
                }
                n = mCoreView.drawAll(mViewAdapter, adapter);
            }
            else if (mCachedBitmap != null) {
                synchronized(mCachedBitmap) {
                    canvas.drawBitmap(mCachedBitmap, 0, 0, null);
                    n++;
                }
            }
            
            if (dyndraw) {
                mCoreView.dynDraw(mViewAdapter, adapter);
            }
            adapter.endPaint();
        }
        
        return n;
    }
    
    private boolean regen(boolean fromRegenAll) {
        if (getWidth() < 2 || getHeight() < 2 || mRegenning) {
            return true;
        }
        try {
            if (mCachedBitmap == null) {
                mCachedBitmap = Bitmap.createBitmap(getWidth(), getHeight(),
                                                    Bitmap.Config.ARGB_8888);
            }
            else if (mRegenBitmap == null) {
                mRegenBitmap = Bitmap.createBitmap(getWidth(), getHeight(),
                                                   Bitmap.Config.ARGB_8888);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        if (mCachedBitmap != null) {
            mRegenning = true;
            new Thread(new Runnable() {
                public void run() {
                    Bitmap bmp = mRegenBitmap != null ? mRegenBitmap : mCachedBitmap;
                    synchronized(bmp) {
                        bmp.eraseColor(mBkColor);
                        drawShapes(new Canvas(bmp), mCanvasRegen, false);
                        
                        if (bmp == mRegenBitmap) {
                            if (mCachedBitmap != null) {
                                mCachedBitmap.recycle();
                            }
                            mCachedBitmap = mRegenBitmap;
                        }
                    }
                    mRegenBitmap = null;
                    mRegenning = false;
                    postInvalidate();
                }
            }).start();
        }
        else if (fromRegenAll) {    // 视图太大，无法后台绘制，将直接在onDraw中显示
            invalidate();
        }
        
        return mCachedBitmap != null;
    }
    
    @Override
    protected void onDetachedFromWindow() {
        if (mActiveView == this) {
            mActiveView = null;
        }
        if (mViewAdapter != null) {
            mCoreView.destoryView(mViewAdapter);
            mViewAdapter.delete();
            mViewAdapter = null;
        }
        if (mCoreView != null) {
            mCoreView.delete();
            mCoreView = null;
        }
        if (mCanvasAdapter != null) {
            mCanvasAdapter.delete();
            mCanvasAdapter = null;
        }
        if (mCanvasRegen != null) {
            mCanvasRegen.delete();
            mCanvasRegen = null;
        }
        if (mGestureListener != null) {
            mGestureListener.release();
            mGestureListener = null;
        }
        if (mCachedBitmap != null) {
            mCachedBitmap.recycle();
            mCachedBitmap = null;
        }
        mGestureDetector = null;
        
        super.onDetachedFromWindow();
    }
    
    //! 当前命令改变的通知
    public interface CommandChangedListener {
        void onCommandChanged(GraphView view);
    }
    
    //! 图形选择集改变的通知
    public interface SelectionChangedListener {
        void onSelectionChanged(GraphView view);
    }
    
    //! 图形数据改变的通知
    public interface ContentChangedListener {
        void onContentChanged(GraphView view);
    }
    
    private ArrayList<CommandChangedListener> commandChangedListeners;
    private ArrayList<SelectionChangedListener> selectionChangedListeners;
    private ArrayList<ContentChangedListener> contentChangedListeners;
    
    //! 添加当前命令改变的观察者
    public void setOnCommandChangedListener(CommandChangedListener listener) {
        if (this.commandChangedListeners == null)
            this.commandChangedListeners = new ArrayList<CommandChangedListener>();
        this.commandChangedListeners.add(listener);
    }
    
    //! 添加图形选择集改变的观察者
    public void setOnSelectionChangedListener(SelectionChangedListener listener) {
        if (this.selectionChangedListeners == null)
            this.selectionChangedListeners = new ArrayList<SelectionChangedListener>();
        this.selectionChangedListeners.add(listener);
    }
    
    //! 添加图形数据改变的观察者
    public void setOnContentChangedListener(ContentChangedListener listener) {
        if (this.contentChangedListeners == null)
            this.contentChangedListeners = new ArrayList<ContentChangedListener>();
        this.contentChangedListeners.add(listener);
    }
    
    //! 视图回调适配器
    private class ViewAdapter extends GiView {
        private ContextAction mContextAction;
        
        public synchronized void delete() {
            if (mContextAction != null) {
                mContextAction.release();
                mContextAction = null;
            }
            super.delete();
        }
        
        public void removeContextButtons() {
            if (mContextAction != null) {
                mContextAction.removeButtonLayout();
            }
        }
        
        @Override
        public void regenAll() {
            if (mCachedBitmap != null && !mRegenning &&
                (mCachedBitmap.getWidth() != getWidth()
                 || mCachedBitmap.getHeight() != getHeight())) {
                    mCachedBitmap.recycle();
                    mCachedBitmap = null;
                }
            regen(true);
        }
        
        @Override
        public void regenAppend() {
            if (mCachedBitmap != null && !mRegenning) {
                synchronized(mCachedBitmap) {
                    if (mCanvasAdapter.beginPaint(new Canvas(mCachedBitmap))) {
                        mCoreView.drawAppend(mViewAdapter, mCanvasAdapter);
                        mCanvasAdapter.endPaint();
                    }
                }
            }
            invalidate();
        }
        
        @Override
        public void redraw() {
            invalidate();
        }
        
        @Override
        public boolean isContextActionsVisible() {
            return mContextAction != null && mContextAction.isVisible();
        }
        
        @Override
        public boolean showContextActions(Ints actions, Floats buttonXY,
                                          float x, float y, float w, float h) {
            if (actions.count() == 0 && mContextAction == null) {
                return true;
            }
            if (mContextAction == null) {
                mContextAction = new ContextAction(getContext(), mCoreView, GraphView.this);
            }
            return mContextAction.showActions(actions, buttonXY);
        }
        
        @Override
        public void commandChanged() {
            if (commandChangedListeners != null) {
                for (CommandChangedListener listener : commandChangedListeners) {
                    listener.onCommandChanged(GraphView.this);
                }
            }
        }
        
        @Override
        public void selectionChanged() {
            if (selectionChangedListeners != null) {
                for (SelectionChangedListener listener : selectionChangedListeners) {
                    listener.onSelectionChanged(GraphView.this);
                }
            }
        }
        
        @Override
        public void contentChanged() {
            if (contentChangedListeners != null) {
                for (ContentChangedListener listener : contentChangedListeners) {
                    listener.onContentChanged(GraphView.this);
                }
            }
        }
    }
}
