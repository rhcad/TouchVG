// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

package rhcad.touchvg.view.internal;

import rhcad.touchvg.view.CanvasAdapter;
import android.content.Context;
import android.util.Log;

public class ResourceUtil {
    private static final String TAG = "touchvg";
    
    public static int getResIDFromName(Context ctx, String type, String name) {
        int id = name != null ? ctx.getResources().getIdentifier(name,
                type, ctx.getPackageName()) : 0;
        if (id == 0 && name != null) {
            Log.i(TAG, "Need resource R." + type + "." + name);
        }
        return id;
    }
    
    public static String getResName(Context ctx, int id) {
        try {
            return ctx.getResources().getResourceEntryName(id);
        } catch (Exception e) {
            return null;
        }
    }
    
    public static int getDrawableIDFromName(Context context, String name) {
        return getResIDFromName(context, "drawable", name);
    }
    
    public static void setContextImages(Context context) {
        if (ContextAction.getButtonImages() == null) {
            final String[] imageNames = new String[] { null, "vg_selall", null, "vg_draw",
                    "vg_back", "vg_delete", "vg_clone", "vg_fixlen", "vg_freelen",
                    "vg_lock", "vg_unlock", "vg_edit", "vg_endedit", null, null,
                    "vg_addvertex", "vg_delvertex", "vg_group", "vg_ungroup", "vg_overturn" };
            final String[] handleNames = new String[] { "vgdot1", "vgdot2", "vgdot3",
                    "vg_lock", "vg_unlock", "vg_back", "vg_endedit" };
            int captionsID = getResIDFromName(context, "array", "vg_action_captions");
            int[] imageIDs = new int[(imageNames.length)];
            int[] handleImageIDs = new int[(handleNames.length)];
            
            for (int i = 0; i < imageNames.length; i++) {
                imageIDs[i] = getDrawableIDFromName(context, imageNames[i]);
            }
            for (int j = 0; j < handleNames.length; j++) {
                handleImageIDs[j] = getDrawableIDFromName(context, handleNames[j]);
            }
        
            ContextAction.setButtonImages(imageIDs);
            ContextAction.setButtonCaptionsID(captionsID);
            CanvasAdapter.setHandleImageIDs(handleImageIDs);
        }
    }
    
    //! 设置额外的上下文操作按钮的图像ID数组，其动作序号从40起
    public static void setExtraContextImages(Context context, int[] extraImageIDs) {
        ContextAction.setExtraButtonImages(extraImageIDs);
    }
}
