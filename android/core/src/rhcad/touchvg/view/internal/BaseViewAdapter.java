package rhcad.touchvg.view.internal;

import java.util.ArrayList;

import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.core.Ints;
import rhcad.touchvg.view.GraphView;
import rhcad.touchvg.view.GraphView.CommandChangedListener;
import rhcad.touchvg.view.GraphView.ContentChangedListener;
import rhcad.touchvg.view.GraphView.DynamicChangedListener;
import rhcad.touchvg.view.GraphView.SelectionChangedListener;

//! 视图回调适配器
public abstract class BaseViewAdapter extends GiView {
    private ContextAction mAction;
    private boolean mActionEnabled = true;
    private ArrayList<CommandChangedListener> commandChangedListeners;
    private ArrayList<SelectionChangedListener> selectionChangedListeners;
    private ArrayList<ContentChangedListener> contentChangedListeners;
    private ArrayList<DynamicChangedListener> dynamicChangedListeners;
    
    protected abstract GraphView getView();
    protected abstract ContextAction createContextAction();
    
    public synchronized void delete() {
        if (mAction != null) {
            mAction.release();
            mAction = null;
        }
        super.delete();
    }
    
    public void removeContextButtons() {
        if (mAction != null) {
            mAction.removeButtonLayout();
        }
    }
    
    public void setContextActionEnabled(boolean enabled) {
        mActionEnabled = enabled; 
    }
    
    @Override
    public boolean isContextActionsVisible() {
        return mAction != null && mAction.isVisible();
    }
    
    @Override
    public boolean showContextActions(Ints actions, Floats xy,
                                      float x, float y, float w, float h) {
        if ((actions == null || actions.count() == 0) && mAction == null) {
            return true;
        }
        if (!mActionEnabled) {
            return false;
        }
        if (mAction == null) {
            mAction = createContextAction();
        }
        return mAction.showActions(getView().getView().getContext(), actions, xy);
    }
    
    @Override
    public void commandChanged() {
        if (commandChangedListeners != null) {
            for (CommandChangedListener listener : commandChangedListeners) {
                listener.onCommandChanged(getView());
            }
        }
    }
    
    @Override
    public void selectionChanged() {
        if (selectionChangedListeners != null) {
            for (SelectionChangedListener listener : selectionChangedListeners) {
                listener.onSelectionChanged(getView());
            }
        }
    }
    
    @Override
    public void contentChanged() {
        if (contentChangedListeners != null) {
            for (ContentChangedListener listener : contentChangedListeners) {
                listener.onContentChanged(getView());
            }
        }
    }
    
    @Override
    public void dynamicChanged() {
        if (dynamicChangedListeners != null) {
            for (DynamicChangedListener listener : dynamicChangedListeners) {
                listener.onDynamicChanged(getView());
            }
        }
    }
    
    public void setOnCommandChangedListener(CommandChangedListener listener) {
        if (this.commandChangedListeners == null)
            this.commandChangedListeners = new ArrayList<CommandChangedListener>();
        this.commandChangedListeners.add(listener);
    }
    
    public void setOnSelectionChangedListener(SelectionChangedListener listener) {
        if (this.selectionChangedListeners == null)
            this.selectionChangedListeners = new ArrayList<SelectionChangedListener>();
        this.selectionChangedListeners.add(listener);
    }
    
    public void setOnContentChangedListener(ContentChangedListener listener) {
        if (this.contentChangedListeners == null)
            this.contentChangedListeners = new ArrayList<ContentChangedListener>();
        this.contentChangedListeners.add(listener);
    }
    
    public void setOnDynamicChangedListener(DynamicChangedListener listener) {
        if (this.dynamicChangedListeners == null)
            this.dynamicChangedListeners = new ArrayList<DynamicChangedListener>();
        this.dynamicChangedListeners.add(listener);
    }
}
