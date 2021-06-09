// Copyright (C) 2017  Daniel 'dady8889' Múčka
// Copyright (C) 2020  Roger 'R0rt1z2' Ortiz
// Copyright (C) 2020  Jose 'jmpfbmx'

package com.r0rt1z2.mediatekparts;

import android.os.Bundle;
import android.app.AlertDialog;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.Context;
import android.preference.SwitchPreference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View.OnClickListener;
import com.android.settingslib.drawer.SettingsDrawerActivity;

public class SettingsActivity extends SettingsDrawerActivity {

    private static final String TAG = "MediaTekParts";
    private static final String PROPERTY_HDMI_HANDLER = "sys.service.hdmi.enable";
    private static final int MENU_REBOOT = Menu.FIRST;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Display the fragment as the main content.
        getFragmentManager().beginTransaction().replace(R.id.content_frame, new MediaTekFragment()).commit();
        Log.d(TAG, "Activity opened!");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);

        // Add reboot button to menu
        menu.add(Menu.NONE, MENU_REBOOT, Menu.NONE, R.string.action_reboot_title).setIcon(R.drawable.ic_replay_white_24dp).setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        switch (id) {
            case MENU_REBOOT: {
                // Shows the reboot confirmation dialog
                AlertDialog.Builder alertBuilder = new AlertDialog.Builder(this);
                alertBuilder.setMessage(R.string.action_reboot_description);
                alertBuilder.setCancelable(true);
                alertBuilder.setPositiveButton(
                    R.string.alert_yes,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            try {
                                Process proc = Runtime.getRuntime().exec(new String[]{"sh", "-c", "svc power reboot"});
                                proc.waitFor();
                            } catch (Exception ex) {
                                Log.e(TAG, ex.getMessage());
                            }
                        }
                    });
                alertBuilder.setNegativeButton(
                    R.string.alert_no,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            dialog.cancel();
                        }
                    });
                AlertDialog alertReboot = alertBuilder.create();
                alertReboot.show();
                break;
            }
            default:
                return super.onOptionsItemSelected(item);
        }
        return true;
    }

    public static class MediaTekFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {

        private static SharedPreferences preferenceManager;
        private static Context globalContext;

        @Override
        public void onAttach(Activity activity) {
            super.onAttach(activity);
            globalContext = activity.getApplicationContext();
        }

        @Override
        public void onResume() {
            super.onResume();
            getPreferenceManager().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);

        }

        @Override
        public void onPause() {
            getPreferenceManager().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
            super.onPause();
        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            // Load the preferences from an XML resource
            addPreferencesFromResource(R.layout.preferences);
            preferenceManager = PreferenceManager.getDefaultSharedPreferences(globalContext);

            // Set UP HDMI
            SwitchPreference switchPref = (SwitchPreference)findPreference("pref_hdmi");
            boolean savedValue = preferenceManager.getBoolean("pref_hdmi", false);
            if (savedValue) {
                int hdmi_on = 1;
                switchPref.setChecked(savedValue);
            } else {
                int hdmi_on = 0;
            }
            SystemPropertiesReflection.SetSystemString(PROPERTY_HDMI_HANDLER, "1");
            
        }

        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key)
        {
            int resourceIndex = getResources().getIdentifier(key + "_key", "string", getContext().getPackageName());
            boolean newValue = sharedPreferences.getBoolean(key, false);
            SharedPreferences.Editor editor = preferenceManager.edit();

            switch (resourceIndex) {
                case R.string.pref_hdmi_key: {
                    SystemPropertiesReflection.SetSystemString(PROPERTY_HDMI_HANDLER, newValue ? "1" : "0");
                    editor.putBoolean("pref_hdmi", newValue);
                    break;
                }
            }
            editor.apply();
        }
    }
}
