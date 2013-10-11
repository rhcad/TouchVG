//! \file ImageCache.java
//! \brief 图像对象缓存类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package touchvg.view;

import com.larvalabs.svgandroid.SVG;
import com.larvalabs.svgandroid.SVGBuilder;
import com.larvalabs.svgandroid.SVGParseException;

import android.content.res.Resources;
import android.graphics.Picture;

//! 图像对象缓存类
/*! \ingroup GROUP_ANDROID
 */
public class ImageCache extends Object {
	Picture picture;
	
	public ImageCache() {
	}
	
	//! 插入一个图片绘制对象
    public boolean addPicture(Picture picture) {
    	this.picture = picture;
    	return true;
    }
    
    public Picture getPicture() {
    	return this.picture;
    }
    
    //! 插入一个SVG图像
    public Picture addSVGFromResource(Resources resources, int resId) {
    	try {
            SVG svg = new SVGBuilder().readFromResource(resources, resId).build();
            picture = svg.getPicture();
            return picture;
        } catch (SVGParseException e) {
            e.printStackTrace();
        }
        return null;
    }
}
