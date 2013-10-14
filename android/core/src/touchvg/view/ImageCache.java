//! \file ImageCache.java
//! \brief 图像对象缓存类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package touchvg.view;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Picture;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.PictureDrawable;
import android.support.v4.util.LruCache;
import android.view.View;

import com.larvalabs.svgandroid.SVG;
import com.larvalabs.svgandroid.SVGBuilder;
import com.larvalabs.svgandroid.SVGParseException;

//! 图像对象缓存类
/*! \ingroup GROUP_ANDROID
 */
public class ImageCache extends Object {
    public static final String BITMAP_PREFIX = "bmp:";
    public static final String SVG_PREFIX = "svg:";
    private static final int CACHE_SIZE = 2 * 1024 * 1024;    // 2MB
    private LruCache<String, Drawable> mCache;
    
    public ImageCache() {
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
        mCache.evictAll();
    }
    
    //! 查找图像对象
    public Drawable getImage(View view, String name) {
        Drawable drawable = mCache.get(name);
        
        if (drawable == null && view != null) {
            if (name.indexOf(BITMAP_PREFIX) == 0) { // R.drawable.resName
                String resName = name.substring(BITMAP_PREFIX.length());
                int id = view.getResources().getIdentifier(resName,
                        "drawable", view.getContext().getPackageName());
                drawable = this.addBitmap(view.getResources(), id, name);
            }
            else if (name.indexOf(SVG_PREFIX) == 0) { // R.raw.resName
                String resName = name.substring(SVG_PREFIX.length());
                int id = view.getResources().getIdentifier(resName,
                        "raw", view.getContext().getPackageName());
                drawable = this.addSVG(view.getResources(), id, name);
            }
        }
        
        return drawable;
    }
    
    //! 插入一个程序资源中的位图图像
    public Drawable addBitmap(Resources res, int id, String name) {
        Drawable drawable = mCache.get(name);

        if (drawable == null && id != 0) {
            try {
                Bitmap bitmap = BitmapFactory.decodeResource(res, id);
                
                if (bitmap != null && bitmap.getWidth() > 0) {
                    drawable = new BitmapDrawable(res, bitmap);
                    mCache.put(name, drawable);
                }
            } catch (SVGParseException e) {
                e.printStackTrace();
            }
        }
        
        return drawable;
    }
    
    //! 插入一个程序资源中的SVG图像
    public Drawable addSVG(Resources res, int id, String name) {
        Drawable drawable = mCache.get(name);

        if (drawable == null && id != 0) {
            try {
                SVG svg = new SVGBuilder().readFromResource(res, id).build();
                Picture picture = svg.getPicture();
                
                if (picture != null && picture.getWidth() > 0) {
                    drawable = svg.getDrawable();
                    mCache.put(name, drawable);
                }
            } catch (SVGParseException e) {
                e.printStackTrace();
            }
        }
        
        return drawable;
    }
}
