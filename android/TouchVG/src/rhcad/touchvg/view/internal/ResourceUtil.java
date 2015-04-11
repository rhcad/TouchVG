// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.internal;

import rhcad.touchvg.view.CanvasAdapter;
import android.content.Context;
import android.util.Log;

public class ResourceUtil {
    private static final String TAG = "touchvg";
    private static final String[] HANDLE_NAMES = new String[] { "vgdot1", "vgdot2", "vgdot3",
            "vg_lock", "vg_unlock", "vg_back", "vg_endedit",
            "vgnode", "vgcen", "vgmid", "vgquad", "vgtangent", "vgcross", "vgparallel",
            "vgnear", "vgpivot", "vg_overturn" };
    private static final String[] IMAGE_NAMES = new String[] { null, "vg_selall", null, null,
            HANDLE_NAMES[5], "vg_delete", "vg_clone", "vg_fixlen", "vg_freelen", HANDLE_NAMES[3],
            HANDLE_NAMES[4], "vg_edit", HANDLE_NAMES[6], null, null, null, null, "vg_group",
            "vg_ungroup", "vg_overturn" };

    private ResourceUtil() {
    }

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
            Log.w(TAG, "getResName fail", e);
            return null;
        }
    }

    public static int getDrawableIDFromName(Context context, String name) {
        return getResIDFromName(context, "drawable", name);
    }

    // @string/name
    public static String getStringFromName(Context context, String name) {
        int id = getResIDFromName(context, "string", name);
        return id == 0 ? name : context.getResources().getString(id);
    }

    public static void setContextImages(Context context) {
        if (ContextAction.getButtonImages() == null) {
            final int captionsID = getResIDFromName(context, "array", "vg_action_captions");
            final int[] imageIDs = new int[IMAGE_NAMES.length];
            final int[] handleImageIDs = new int[HANDLE_NAMES.length];

            for (int i = 0; i < IMAGE_NAMES.length; i++) {
                imageIDs[i] = getDrawableIDFromName(context, IMAGE_NAMES[i]);
            }
            for (int j = 0; j < HANDLE_NAMES.length; j++) {
                handleImageIDs[j] = getDrawableIDFromName(context, HANDLE_NAMES[j]);
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
