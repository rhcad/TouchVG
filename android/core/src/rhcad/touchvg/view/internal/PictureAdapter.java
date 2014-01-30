//! \file PictureAdapter.java
//! \brief 实现Android画布适配器类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package rhcad.touchvg.view.internal;

import rhcad.touchvg.core.GiCanvas;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DashPathEffect;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PathEffect;
import android.graphics.Picture;
import android.graphics.RectF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.PictureDrawable;
import android.view.View;

//! 输出路径对象的画布适配器类
public class PictureAdapter extends GiCanvas {
    private Path mPath;
    private Paint mPen = new Paint();
    private Paint mBrush = new Paint();
    private Picture mPicture;
    private Canvas mCanvas;
    private PictureCreated mCallback;
    private View mView;
    private ImageCache mCache;
    private PathEffect mEffects;
    private static final float[] DASH = { 4, 2 };
    private static final float[] DOT = { 1, 2 };
    private static final float[] DASH_DOT = { 10, 2, 2, 2 };
    private static final float[] DASH_DOTDOT = { 20, 2, 2, 2, 2, 2 };

    public interface PictureCreated {
        void onPictureCreated(Picture p, float x, float y, int id);
    }
    
    public PictureAdapter(View view, ImageCache cache, PictureCreated created) {
        this.mView = view;
        this.mCallback = created;
        this.mCache = cache;
        
        mPen.setAntiAlias(true);            // 线条反走样
        mPen.setDither(true);               // 高精度颜色采样，会略慢
        mPen.setStyle(Paint.Style.STROKE);  // 仅描边
        mPen.setPathEffect(null);           // 实线
        mPen.setStrokeCap(Paint.Cap.ROUND); // 圆端
        mPen.setStrokeJoin(Paint.Join.ROUND); // 折线转角圆弧过渡
        mBrush.setStyle(Paint.Style.FILL);  // 仅填充
        mBrush.setColor(Color.TRANSPARENT); // 默认透明，不填充
        mBrush.setAntiAlias(true);          // 文字反走样
    }
    
    @Override
    public synchronized void delete() {
        mView = null;
        mCallback = null;
        mCache = null;
        super.delete();
    }
    
    @Override
    public boolean beginShape(int type, int sid, int version,
            float x, float y, float w, float h) {
        if (w < 1 || h < 1)
            return false;
        mPicture = new Picture();
        mCanvas = mPicture.beginRecording(Math.round(w), Math.round(h));
        mCanvas.translate(-x, -y);
        return true;
    }
    
    @Override
    public void endShape(int type, int sid, float x, float y) {
        mPicture.endRecording();
        mCallback.onPictureCreated(mPicture, x, y, sid);
        mPicture = null;
        mCanvas = null;
    }
    
    private void makeLinePattern(float arr[], float width, float phase) {
        float f[] = new float[arr.length];
        for (int i = 0; i < arr.length; i++) {
            f[i] = arr[i] * (width < 1 ? 1 : width);
        }
        this.mEffects = new DashPathEffect(f, phase);
    }
    
    @Override
    public void setPen(int argb, float width, int style, float phase, float orgw) {
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
        }
        
        return ret;
    }
    
    @Override
    public void drawHandle(float x, float y, int type) {
    }
    
    @Override
    public void drawBitmap(String name, float xc, float yc, float w, float h,
                           float angle) {
        final Drawable drawable = mCache != null ? mCache.getImage(mView, name) : null;
        
        if (drawable != null) {
            Matrix mat = new Matrix();
            int width = ImageCache.getWidth(drawable);
            int height = ImageCache.getHeight(drawable);
            
            mat.postTranslate(-0.5f * width, -0.5f * height);
            mat.postRotate(-angle * 180.f / 3.1415926f); // degree to radian
            mat.postScale(w / width, h / height);
            mat.postTranslate(xc, yc);
            
            try {
                BitmapDrawable b = (BitmapDrawable)drawable;
                mCanvas.drawBitmap(b.getBitmap(), mat, null);
            } catch (ClassCastException e) {
                try {
                    PictureDrawable p = (PictureDrawable)drawable;
                    mCanvas.concat(mat);
                    mCanvas.drawPicture(p.getPicture());
                    mat.invert(mat);
                    mCanvas.concat(mat);
                } catch (ClassCastException e2) {}
            }
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
