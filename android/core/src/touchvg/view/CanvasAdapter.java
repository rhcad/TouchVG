//! \file CanvasAdapter.java
//! \brief 实现Android画布适配器类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package touchvg.view;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DashPathEffect;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PathEffect;
import android.graphics.PorterDuff.Mode;
import android.graphics.RectF;
import android.view.View;
import touchvg.core.GiCanvas;

//! Android画布适配器类
/*! \ingroup GROUP_ANDROID
 */
public class CanvasAdapter extends GiCanvas {
    private Path mPath;
    private Paint mPen = new Paint();
    private Paint mBrush = new Paint();
    private Canvas mCanvas;
    private View mView;
    private int mBkColor = Color.TRANSPARENT;
    private PathEffect mEffects;        // solid line
    private static final float[] DASH = { 5, 5 };
    private static final float[] DOT = { 1, 2 };
    private static final float[] DASH_DOT = { 10, 2, 2, 2 };
    private static final float[] DASH_DOTDOT = { 20, 2, 2, 2, 2, 2 };
    private static int[] mHandleIDs;    // 控制手柄图像id数组
    
    static {
        System.loadLibrary("touchvg");  // 加载绘图内核动态库，以便访问JNI
    }
    
    public CanvasAdapter(View view) {
        this.mView = view;
    }
    
    @Override
    public synchronized void delete() {
        mView = null;
        mHandleIDs = null;
        super.delete();
    }
    
    public void setBackgroundColor(int color) {
        mBkColor = color;
    }
    
    public Canvas getCanvas() {
        return mCanvas;
    }
    
    public boolean isDrawing() {
        return mCanvas != null;
    }
    
    public boolean beginPaint(Canvas canvas) {
        if (this.mCanvas != null || canvas == null) {
            return false;
        }
        
        this.mCanvas = canvas;
        
        mPen.setAntiAlias(true);            // 线条反走样
        mPen.setDither(true);               // 高精度颜色采样，会略慢
        mPen.setStyle(Paint.Style.STROKE);  // 仅描边
        mPen.setPathEffect(null);           // 实线
        mPen.setStrokeCap(Paint.Cap.ROUND); // 圆端
        mPen.setStrokeJoin(Paint.Join.ROUND); // 折线转角圆弧过渡
        mBrush.setStyle(Paint.Style.FILL);  // 仅填充
        mBrush.setColor(Color.TRANSPARENT); // 默认透明，不填充
        mBrush.setAntiAlias(true);          // 文字反走样
        
        return true;
    }
    
    public void endPaint() {
        this.mCanvas = null;
    }
    
    private void makeLinePattern(float arr[], float width, float phase) {
        float f[] = new float[arr.length];
        for (int i = 0; i < arr.length; i++) {
            f[i] = arr[i] * (width < 1 ? 1 : width);
        }
        this.mEffects = new DashPathEffect(f, phase);
    }
    
    @Override
    public void setPen(int argb, float width, int style, float phase) {
        if (argb != 0) {
            mPen.setColor(argb);
        }
        if (width > 0) {
            mPen.setStrokeWidth(width);
        }
        
        if (style >= 0 && style <= 4) {
            if (style == 1) {
                this.makeLinePattern(DASH, width, phase);
            } else if (style == 2) {
                this.makeLinePattern(DOT, width, phase);
            } else if (style == 3) {
                this.makeLinePattern(DASH_DOT, width, phase);
            } else if (style == 4) {
                this.makeLinePattern(DASH_DOTDOT, width, phase);
            } else {
                this.mEffects = null;
            }
            mPen.setPathEffect(this.mEffects);
            mPen.setStrokeCap(this.mEffects != null ? Paint.Cap.BUTT
                              : Paint.Cap.ROUND);
        }
    }
    
    @Override
    public void setBrush(int argb, int style) {
        if (style == 0) {
            mBrush.setColor(argb);
        }
    }
    
    @Override
    public void saveClip() {
        mCanvas.save(Canvas.CLIP_SAVE_FLAG);
    }
    
    @Override
    public void restoreClip() {
        mCanvas.restore();
    }
    
    @Override
    public void clearRect(float x, float y, float w, float h) {
        mCanvas.save(Canvas.CLIP_SAVE_FLAG);
        mCanvas.clipRect(x, y, x + w, y + h);
        mCanvas.drawColor(mBkColor, Mode.CLEAR);    // punch a whole in the view-hierarchy below the view
        mCanvas.restore();
    }
    
    @Override
    public void drawRect(float x, float y, float w, float h,
                         boolean stroke, boolean fill) {
        if (fill) {
            mCanvas.drawRect(x, y, x + w, y + h, mBrush);
        }
        if (stroke) {
            mCanvas.drawRect(x, y, x + w, y + h, mPen);
        }
    }
    
    @Override
    public boolean clipRect(float x, float y, float w, float h) {
        return mCanvas.clipRect(x, y, x + w, y + h);
    }
    
    @Override
    public void drawLine(float x1, float y1, float x2, float y2) {
        mCanvas.drawLine(x1, y1, x2, y2, mPen);
    }
    
    @Override
    public void drawEllipse(float x, float y, float w, float h,
                            boolean stroke, boolean fill) {
        if (stroke && Math.abs(w) <= 1 && Math.abs(h) <= 1) {
            mCanvas.drawPoint(x, y, mPen);
        } else {
            if (fill) {
                mCanvas.drawOval(new RectF(x, y, x + w, y + h), mBrush);
            }
            if (stroke) {
                mCanvas.drawOval(new RectF(x, y, x + w, y + h), mPen);
            }
        }
    }
    
    @Override
    public void beginPath() {
        if (mPath == null) {
            mPath = new Path();
        } else {
            mPath.reset();
        }
    }
    
    @Override
    public void moveTo(float x, float y) {
        mPath.moveTo(x, y);
    }
    
    @Override
    public void lineTo(float x, float y) {
        mPath.lineTo(x, y);
    }
    
    @Override
    public void bezierTo(float c1x, float c1y, float c2x, float c2y,
                         float x, float y) {
        mPath.cubicTo(c1x, c1y, c2x, c2y, x, y);
    }
    
    @Override
    public void quadTo(float cpx, float cpy, float x, float y) {
        mPath.quadTo(cpx, cpy, x, y);
    }
    
    @Override
    public void closePath() {
        mPath.close();
    }
    
    @Override
    public void drawPath(boolean stroke, boolean fill) {
        if (fill) {
            mCanvas.drawPath(mPath, mBrush);
        }
        if (stroke) {
            mCanvas.drawPath(mPath, mPen);
        }
    }
    
    @Override
    public boolean clipPath() {
        boolean ret = false;
        
        try {
            ret = mCanvas.clipPath(mPath);
        } catch (UnsupportedOperationException e) { // GLES20Canvas, >=api11
            e.printStackTrace();
            if (mView != null) {                    // 改为软实现后下次绘制才生效
                mView.setLayerType(View.LAYER_TYPE_SOFTWARE, null); // need API11 or above
            }
        }
        
        return ret;
    }
    
    public static void setHandleImageIDs(int[] ids) {
        mHandleIDs = ids;
    }
    
    public Bitmap getHandleBitmap(int type) {
        return (mHandleIDs != null && type >= 0 && type < mHandleIDs.length) ?
        BitmapFactory.decodeResource(mView.getResources(), mHandleIDs[type]) : null;
    }
    
    @Override
    public void drawHandle(float x, float y, int type) {
        final Bitmap bmp = getHandleBitmap(type);
        if (bmp != null) {
            mCanvas.drawBitmap(bmp, x - bmp.getWidth() / 2,
                               y - bmp.getHeight() / 2, null);
        }
    }
    
    @Override
    public void drawBitmap(String name, float xc, float yc, float w, float h,
                           float angle) {
        final Bitmap bmp = getHandleBitmap(4);
        if (bmp != null && bmp.getWidth() > 0) {
            Matrix mat = new Matrix();
            mat.postTranslate(-0.5f * bmp.getWidth(), -0.5f * bmp.getHeight());
            mat.postRotate(-angle * 180.f / 3.1415926f); // degree to radian
            mat.postScale(w / bmp.getWidth(), h / bmp.getHeight());
            mat.postTranslate(xc, yc);
            mCanvas.drawBitmap(bmp, mat, null);
            mat = null;
        }
    }
    
    @Override
    public float drawTextAt(String text, float x, float y, float h, int align) {
        final Paint.FontMetrics fm = mBrush.getFontMetrics();
        float lineHeight = Math.abs(fm.descent) + Math.abs(fm.ascent);
        
        if (Math.abs(lineHeight - h) > 1e-2f) {
            mBrush.setTextSize(h * mBrush.getTextSize() / lineHeight);
        }
        
        float w = mBrush.measureText(text);
        x -= (align == 2) ? w : ((align == 1) ? w / 2 : 0);
        mCanvas.drawText(text, x, y - fm.top, mBrush);
        
        return w;
    }
}
