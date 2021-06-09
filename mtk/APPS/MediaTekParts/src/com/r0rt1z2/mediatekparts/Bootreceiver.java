// Copyright (C) 2017  Daniel 'dady8889' Múčka
// Copyright (C) 2020  Roger 'R0rt1z2' Ortiz
// Copyright (C) 2020 Jose 'jmpfbmx'

package com.r0rt1z2.mediatekparts;

import android.content.BroadcastReceiver;
import android.content.SharedPreferences;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.preference.PreferenceManager;

public class Bootreceiver extends BroadcastReceiver  {

    private static final String TAG = "MediaTekParts";
    private static final String HDMI_PROP = "sys.service.hdmi.enable";

    @Override
    public void onReceive(Context context, Intent intent) {

        Log.d(TAG, "Received on boot finish event");

        SharedPreferences preferenceManager = PreferenceManager.getDefaultSharedPreferences(context.getApplicationContext());

        // Enable HDMI if user enabled it
        if (preferenceManager.getBoolean("pref_hdmi", false)) {
            SystemPropertiesReflection.SetSystemString(HDMI_PROP, "1");
        }
    }
}
