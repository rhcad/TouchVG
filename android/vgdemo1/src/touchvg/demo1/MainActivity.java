// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package touchvg.demo1;

import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class MainActivity extends ListActivity {
    private static final String[] VIEWS = new String[] {
        "ExampleActivity1", "ExampleActivitySF", };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setListAdapter(new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1,
                android.R.id.text1, VIEWS));
    }

    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
        super.onListItemClick(l, v, position, id);

        try {
            String name = "touchvg.demo1." + VIEWS[position];
            Class<?> cls = Class.forName(name);
            Intent i = new Intent(this, cls);
            startActivity(i);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
