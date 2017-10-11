//! \file CanvasAdapter.java
//! \brief Canvas adapter class
// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view;

import rhcad.touchvg.core.GiCanvas;
import rhcad.touchvg.view.internal.ImageCache;
import rhcad.touchvg.view.internal.ResourceUtil;
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
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.PictureDrawable;
import android.util.Log;
import android.view.View;

//! Canvas adapter class
public class CanvasAdapter extends GiCanvas {
    private static final String TAG = "touchvg";
    private Path mPath;
    private Paint mPen = new Paint();
    private Paint mBrush = new Paint();
    private Canvas mCanvas;
    private View mView;
    private ImageCache mCache;
    private int mBkColor = Color.TRANSPARENT;
    private PathEffect mEffects;
    private static final float[] DASH = { 4, 2 };
    private static final float[] DOT = { 1, 2 };
    private static final float[] DASH_DOT = { 10, 2, 2, 2 };
    private static final float[] DASH_DOTDOT = { 20, 2, 2, 2, 2, 2 };
    private static int[] mHandleIDs;

    static {
        System.loadLibrary(TAG);
    }

    public CanvasAdapter(View view) {
        this.mView = view;
    }

    public CanvasAdapter(View view, ImageCache cache) {
        this.mView = view;
        this.mCache = cache;
    }

    @Override
    public synchronized void delete() {
        mView = null;
        mCache = null;
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
        return beginPaint(canvas, false);
    }

    public boolean beginPaint(Canvas canvas, boolean fast) {
        if (this.mCanvas != null || canvas == null) {
            return false;
        }

        this.mCanvas = canvas;

        mPen.setAntiAlias(!fast);
        mPen.setDither(!fast);
        mPen.setStyle(Paint.Style.STROKE);
        mPen.setPathEffect(null);
        mPen.setStrokeCap(Paint.Cap.ROUND);
        mPen.setStrokeJoin(Paint.Join.ROUND);
        mBrush.setStyle(Paint.Style.FILL);
        mBrush.setColor(Color.TRANSPARENT);
        mBrush.setAntiAlias(true);

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
    public void setPen(int argb, float width, int style, float phase, float orgw) {
        if (argb != 0) {
            mPen.setColor(argb);
        }
        if (width > 0) {
            mPen.setStrokeWidth(width);
        }
        if (style >= 0) {
            int linecap = style & kLineCapMask;

            style = style & kLineDashMask;
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
            }
            if ((linecap & kLineCapButt) != 0)
                mPen.setStrokeCap(Paint.Cap.BUTT);
            else if ((linecap & kLineCapRound) != 0)
                mPen.setStrokeCap(Paint.Cap.ROUND);
            else if ((linecap & kLineCapSquare) != 0)
                mPen.setStrokeCap(Paint.Cap.SQUARE);
            else
                mPen.setStrokeCap((style > 0 && style < 5) ? Paint.Cap.BUTT : Paint.Cap.ROUND);
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
        // punch a whole in the view-hierarchy below the view
        mCanvas.drawColor(mBkColor, Mode.CLEAR);
        mCanvas.restore();
    }

    @Override
    public void drawRect(float x, float y, float w, float h, boolean stroke, boolean fill) {
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
    public void drawEllipse(float x, float y, float w, float h, boolean stroke, boolean fill) {
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
    public void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y) {
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
        } catch (UnsupportedOperationException e) {
            // GLES20Canvas, >=api11
            Log.w(TAG, "Unsupported operation: clipPath", e);
            if (mView != null) {
                // Soft-implementation will come into effect after the next drawing (need API11+)
                mView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
            }
        }

        return ret;
    }

    public static void setHandleImageIDs(int[] ids) {
        mHandleIDs = ids;
    }

    public Bitmap getHandleBitmap(int type) {
        if (mHandleIDs != null && type >= 0 && type < mHandleIDs.length && mView != null) {
            return BitmapFactory.decodeResource(mView.getResources(), mHandleIDs[type]);
        }
        if (type > 10 && mView != null) {
            int id = ResourceUtil.getDrawableIDFromName(mView.getContext(), "vgdot" + type);
            return BitmapFactory.decodeResource(mView.getResources(), id);
        }
        return null;
    }

    @Override
    public boolean drawHandle(float x, float y, int type, float angle) {
        final Bitmap bmp = getHandleBitmap(type);
        if (bmp != null) {
            mCanvas.drawBitmap(bmp, x - bmp.getWidth() / 2, y - bmp.getHeight() / 2, null);
        } else {
            Log.w(TAG, "Fail to draw handle, type=" + type);
        }
        return bmp != null;
    }

    @Override
    public boolean drawBitmap(String name, float xc, float yc, float w, float h, float angle) {
        final Drawable drawable = mCache != null ? mCache.getImage(mView, name)
                : (name == null && mView != null) ? new BitmapDrawable(mView.getResources(),
                        getHandleBitmap(3)) : null;

        if (drawable != null) {
            Matrix mat = new Matrix();
            int width = ImageCache.getWidth(drawable);
            int height = ImageCache.getHeight(drawable);

            mat.postTranslate(-0.5f * width, -0.5f * height);
            // degree to radian
            mat.postRotate(-angle * 180.f / 3.1415926f);
            mat.postScale(w / width, h / height);
            mat.postTranslate(xc, yc);

            try {
                BitmapDrawable b = (BitmapDrawable) drawable;
                mCanvas.drawBitmap(b.getBitmap(), mat, null);
            } catch (ClassCastException e1) {
                Log.v(TAG, "Not BitmapDrawable", e1);
                try {
                    PictureDrawable p = (PictureDrawable) drawable;
                    mCanvas.concat(mat);
                    mCanvas.drawPicture(p.getPicture());
                    mat.invert(mat);
                    mCanvas.concat(mat);

                    return true;
                } catch (ClassCastException e2) {
                    Log.v(TAG, "Not PictureDrawable", e2);
                } catch (UnsupportedOperationException e3) {
                    // GLES20Canvas, >=api11
                    Log.w(TAG, "Unsupported operation", e3);
                    if (mView != null) {
                        // Soft-implementation will come into effect after the next drawing (need API11+)
                        mView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
                    }
                }
            }
        }

        return false;
    }

    @Override
    public float drawTextAt(String text, float x, float y, float h, int align, float angle) {
        final Paint.FontMetrics fm = mBrush.getFontMetrics();
        float lineHeight = Math.abs(fm.descent) + Math.abs(fm.ascent);

        if (Math.abs(lineHeight - h) > 1e-2f) {
            mBrush.setTextSize(h * mBrush.getTextSize() / lineHeight);
        }

        if (mView != null && text.startsWith("@")) {
            text = ResourceUtil.getStringFromName(mView.getContext(), text.substring(1));
        }

        Matrix mat = null;
        float w = mBrush.measureText(text);

        if (Math.abs(angle) > 1e-3f) {
            mat = new Matrix();
            mat.postRotate(-angle * 180.f / 3.1415926f);
            mat.postTranslate(x, y);
            x = y = 0;
            mCanvas.concat(mat);
        }

        y -= (align & kAlignBottom) != 0 ? h : (align & kAlignVCenter) != 0 ? h / 2 : 0.f;
        x -= (align & kAlignRight) != 0 ? w : ((align & kAlignCenter) != 0 ? w / 2 : 0.f);
        mCanvas.drawText(text, x, y - fm.top, mBrush);

        if (mat != null) {
            mat.invert(mat);
            mCanvas.concat(mat);
        }

        return w;
    }
}
