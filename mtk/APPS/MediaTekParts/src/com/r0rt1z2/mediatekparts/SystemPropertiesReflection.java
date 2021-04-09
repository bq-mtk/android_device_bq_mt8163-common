// Copyright (C) 2017  Daniel 'dady8889' Múčka
// Copyright (C) 2020  Roger 'R0rt1z2' Ortiz

package com.r0rt1z2.mediatekparts;

import android.util.Log;

import java.lang.reflect.Method;

// Class that enables use of SystemProperties class, which is hidden within AOSP Sources

final class SystemPropertiesReflection {

    private static final String TAG = "MediaTekParts";

    static String GetSystemString(String name, String def)
    {
        try {
            Class<?> systemProperties = Class.forName("android.os.SystemProperties");
            Method method = systemProperties.getDeclaredMethod("get", String.class, String.class);
            String propertyValue = (String)method.invoke(systemProperties, name, def);
            Log.d(TAG, "GetSystemString: prop=" + name + " value=" + propertyValue);
            return propertyValue;
        } catch (Exception ex) {
            Log.e(TAG, "GetSystemString: " + ex.getMessage());
        }
        return def;
    }

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

    static boolean GetSystemBoolean(String name, boolean def)
    {
        try {
            Class<?> systemProperties = Class.forName("android.os.SystemProperties");
            Method method = systemProperties.getDeclaredMethod("getBoolean", String.class, boolean.class);
            boolean propertyValue = (boolean)method.invoke(systemProperties, name, def);
            Log.d(TAG, "GetSystemBoolean: prop=" + name + " value=" + propertyValue);
            return propertyValue;
        } catch (Exception ex) {
            Log.e(TAG, "GetSystemBoolean: " + ex.getMessage());
        }
        return def;
    }
}
