// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.app;

import vgtest.testview.ViewFactory;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.app.ListActivity;
import android.content.Intent;
import android.content.res.Configuration;

public class MainActivity extends ListActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setListAdapter(new ArrayAdapter<ViewFactory.DummyItem>(this,
                android.R.layout.simple_list_item_1, android.R.id.text1, ViewFactory.ITEMS));
    }

    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
        super.onListItemClick(l, v, position, id);

        Intent i = new Intent(this, DummyActivity.class);
        i.putExtra("className", ViewFactory.ITEMS.get(position).id);
        i.putExtra("title", ViewFactory.ITEMS.get(position).title);
        i.putExtra("flags", ViewFactory.ITEMS.get(position).flags);
        startActivity(i);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        int orientation = this.getResources().getConfiguration().orientation;

        if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
        }
        else if (orientation == Configuration.ORIENTATION_PORTRAIT) {
        }
    }
}
