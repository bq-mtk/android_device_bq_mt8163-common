// Copyright (C) 2017  Daniel 'dady8889' Múčka
// Copyright (C) 2020  Roger 'R0rt1z2' Ortiz
// Copyright (C) 2020  Jose 'jmpfbmx'

package com.r0rt1z2.mediatekparts;

import android.util.Log;

import java.lang.reflect.Method;

// Class that enables use of SystemProperties class, which is hidden within AOSP Sources

final class SystemPropertiesReflection {

    private static final String TAG = "MediaTekParts";

    static void SetSystemString(String name, String val)
    {
        try {
            Class<?> systemProperties = Class.forName("android.os.SystemProperties");
            Method method = systemProperties.getDeclaredMethod("set", String.class, String.class);
            method.invoke(systemProperties, name, val);
            Log.d(TAG, "SetSystemString: prop=" + name + " value=" + val);
        } catch (Exception ex) {
            Log.e(TAG, "SetSystemString: " + ex.getMessage());
        }
    }
}
