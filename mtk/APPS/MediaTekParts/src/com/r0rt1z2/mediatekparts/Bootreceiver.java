// Copyright (C) 2017  Daniel 'dady8889' Múčka
// Copyright (C) 2020  Roger 'R0rt1z2' Ortiz

package com.r0rt1z2.mediatekparts;

import android.content.BroadcastReceiver;
import android.content.SharedPreferences;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.preference.PreferenceManager;

import com.r0rt1z2.mediatekparts.Functions;

public class Bootreceiver extends BroadcastReceiver  {

    private static final String TAG = "MediaTekParts";
    private static final String HDMI_PROP = "sys.service.hdmi.enable";

    SharedPreferences preferenceManager;

    @Override
    public void onReceive(Context context, Intent intent) {

        Log.d(TAG, "Received on boot finish event");

        preferenceManager = PreferenceManager.getDefaultSharedPreferences(context.getApplicationContext());

        // Enable DT2W if user enabled it
        if (Functions.IsDT2WAvailable() && preferenceManager.getBoolean("pref_dt2w", false)) {
            Functions.SetDT2WValue(true);
        }

        // Enable HDMI if user enabled it
        if (preferenceManager.getBoolean("pref_hdmi", false)) {
            SystemPropertiesReflection.SetSystemString(HDMI_PROP, "1");
        }
    }
}
