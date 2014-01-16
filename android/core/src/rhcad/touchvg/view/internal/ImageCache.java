//! \file ImageCache.java
//! \brief 图像对象缓存类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package rhcad.touchvg.view.internal;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Picture;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.PictureDrawable;
import android.support.v4.util.LruCache;
import android.util.Log;
import android.view.View;

import com.larvalabs.svgandroid.SVG;
import com.larvalabs.svgandroid.SVGBuilder;
import com.larvalabs.svgandroid.SVGParseException;

//! 图像对象缓存类
public class ImageCache extends Object {
    private static final String TAG = "touchvg";
    public static final String BITMAP_PREFIX = "bmp:";
    public static final String SVG_PREFIX = "svg:";
    private static final int CACHE_SIZE = 2 * 1024 * 1024;    // 2MB
    private LruCache<String, Drawable> mCache;
    private String mPath;
    
    public ImageCache() {
    }
    
    private boolean createCache() {
        try {
            mCache = new LruCache<String, Drawable>(CACHE_SIZE) {
                @Override
                protected int sizeOf(String key, Drawable d) {
                    int size = 1; // TODO: SVG size?
                    if (d.getClass().isInstance(BitmapDrawable.class)) {
                        size = ((BitmapDrawable)d).getBitmap().getByteCount();
                    }
                    return size;
                }
            };
        }
        catch (NoClassDefFoundError e) {
            Log.e(TAG, "Need android-support-v4.jar in application");
        }
        return mCache != null;
    }
    
    private void addToCache(String name, Drawable drawable) {
        if (mCache != null || createCache()) {
            mCache.put(name, drawable);
        }
    }
    
    //! 设置图像文件的默认路径，自动加载时用
    public void setImagePath(String path) {
        this.mPath = path;
    }
    
    public static int getWidth(Drawable drawable) {
        try {
            return ((BitmapDrawable)drawable).getBitmap().getWidth();
        } catch (ClassCastException e) {}
        
        try {
            return ((PictureDrawable)drawable).getPicture().getWidth();
        } catch (ClassCastException e) {}
        
        return 0;
    }
    
    public static int getHeight(Drawable drawable) {
        try {
            return ((BitmapDrawable)drawable).getBitmap().getHeight();
        } catch (ClassCastException e) {}
        
        try {
            return ((PictureDrawable)drawable).getPicture().getHeight();
        } catch (ClassCastException e) {}
        
        return 0;
    }
    
    //! 清除所有资源
    public void clear() {
        if (mCache != null)
            mCache.evictAll();
    }
    
    //! 查找图像对象
    public Drawable getImage(View view, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;
        
        if (drawable == null && view != null) {
            if (name.indexOf(BITMAP_PREFIX) == 0) { // R.drawable.resName
                final String resName = name.substring(BITMAP_PREFIX.length());
                int id = view.getResources().getIdentifier(resName,
                        "drawable", view.getContext().getPackageName());
                drawable = this.addBitmap(view.getResources(), id, name);
            }
            else if (name.indexOf(SVG_PREFIX) == 0) { // R.raw.resName
                final String resName = name.substring(SVG_PREFIX.length());
                int id = view.getResources().getIdentifier(resName,
                        "raw", view.getContext().getPackageName());
                drawable = this.addSVG(view.getResources(), id, name);
            }
            else if (name.endsWith(".svg")) {
                if (mPath != null && !mPath.isEmpty()) {
                    drawable = this.addSVGFile(new File(mPath, name).getPath(), name);
                }
            }
            else if (mPath != null && !mPath.isEmpty()) {
                drawable = this.addBitmapFile(view.getResources(),
                        new File(mPath, name).getPath(), name);
            }
        }
        
        return drawable;
    }
    
    //! 插入一个程序资源中的位图图像
    public Drawable addBitmap(Resources res, int id, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && id != 0) {
            final Bitmap bitmap = BitmapFactory.decodeResource(res, id);
            
            if (bitmap != null && bitmap.getWidth() > 0) {
                drawable = new BitmapDrawable(res, bitmap);
                addToCache(name, drawable);
            }
        }
        
        return drawable;
    }
    
    //! 插入一个程序资源中的SVG图像
    public Drawable addSVG(Resources res, int id, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && id != 0) {
            drawable = addSVG(new SVGBuilder().readFromResource(res, id), name);
        }
        
        return drawable;
    }
    
    //! 插入一个PNG等图像文件
    public Drawable addBitmapFile(Resources res, String filename, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && new File(filename).exists()) {
            final BitmapFactory.Options opts = new BitmapFactory.Options();
            
            opts.inJustDecodeBounds = true;
            BitmapFactory.decodeFile(filename, opts);
            opts.inJustDecodeBounds = false;
            
            if (opts.outWidth > 1600 || opts.outHeight > 1600) {
                opts.inSampleSize = 4;
            } else if (opts.outWidth > 600 || opts.outHeight > 600) {
                opts.inSampleSize = 2;
            }
            
            final Bitmap bitmap = BitmapFactory.decodeFile(filename, opts);
            
            if (bitmap != null && bitmap.getWidth() > 0) {
                drawable = new BitmapDrawable(res, bitmap);
                addToCache(name, drawable);
            }
        }
        
        return drawable;
    }
    
    //! 插入一个SVG文件的图像
    public Drawable addSVGFile(String filename, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && name.endsWith(".svg")) {
            try {
                final InputStream data = new FileInputStream(new File(filename));
                drawable = addSVG(new SVGBuilder().readFromInputStream(data), name);
                data.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        
        return drawable;
    }
    
    private Drawable addSVG(SVGBuilder builder, String name) {
        Drawable drawable  = null;
        
        try {
            final SVG svg = builder.build();
            final Picture picture = svg.getPicture();
            
            if (picture != null && picture.getWidth() > 0) {
                drawable = svg.getDrawable();
                addToCache(name, drawable);
            }
        } catch (SVGParseException e) {
            e.printStackTrace();
        }
        
        return drawable;
    }
}
