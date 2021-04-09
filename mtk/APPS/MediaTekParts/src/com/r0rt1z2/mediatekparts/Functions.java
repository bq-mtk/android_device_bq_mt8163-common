// Copyright (C) 2017  Daniel 'dady8889' Múčka
// Copyright (C) 2020  Roger 'R0rt1z2' Ortiz

package com.r0rt1z2.mediatekparts;

import android.util.Log;

import java.io.*;

public class Functions {
    private static final String TAG = "MediaTekParts";

    private static final String FILE_EASY_WAKEUP_GESTURE = "/proc/gesture_open";

    /* Region SYS */
    public static String SysRead(String fileName) {
        String line = null;
        BufferedReader reader = null;

        try {
            reader = new BufferedReader(new FileReader(fileName), 512);
            line = reader.readLine();
        } catch (FileNotFoundException e) {
            Log.w(TAG, "File " + fileName + " not found!", e);
        } catch (IOException e) {
            Log.e(TAG, "Cannot read" + fileName, e);
        } finally {
            try {
                if (reader != null) {
                    reader.close();
                }
            } catch (IOException e) {
                // Ignored, not much we can do anyway
            }
        }

        return line;
    }

    public static boolean SysIsAvailable(String fileName) {
        final File file = new File(fileName);
        return file.exists() && file.canWrite();
    }

    public static boolean SysWrite(String fileName, String value) {
        BufferedWriter writer = null;

        try {
            writer = new BufferedWriter(new FileWriter(fileName));
            writer.write(value);
        } catch (FileNotFoundException e) {
            Log.w(TAG, "File " + fileName + " not found!", e);
            return false;
        } catch (IOException e) {
            Log.e(TAG, "Cannot write to " + fileName, e);
            return false;
        } finally {
            try {
                if (writer != null) {
                    writer.close();
                }
            } catch (IOException e) {
                // Ignored, not much we can do anyway
            }
        }

        return true;
    }

    /* Hides the given package */
    public static void HidePackage(String PackageName) {
        try {
            Log.i(TAG, "HidePackage:" + PackageName);
            Process proc = Runtime.getRuntime().exec(new String[]{"/system/bin/pm", "hide", PackageName});
            proc.waitFor();
            } catch (Exception ex) {
                Log.e(TAG, ex.getMessage());
            }
    }

    /* Unhides the given package */
    public static void UnHidePackage(String PackageName) {
        try {
            Log.i(TAG, "UnHidePackage:" + PackageName);
            Process proc = Runtime.getRuntime().exec(new String[]{"/system/bin/pm", "unhide", PackageName});
            proc.waitFor();
            } catch (Exception ex) {
                Log.e(TAG, ex.getMessage());
            }
    }

    /* Check if DT2W it's available in the device (/proc/gesture_open exists?) */
    public static boolean IsDT2WAvailable() {
        return SysIsAvailable(FILE_EASY_WAKEUP_GESTURE);
    }

    public static void SetDT2WValue(boolean on) {
        try {
            if (on) {
                Log.i(TAG, "Enabling DT2W");
                SysWrite(FILE_EASY_WAKEUP_GESTURE, "1");
            } else {
                Log.i(TAG, "Disabling DT2W");
                SysWrite(FILE_EASY_WAKEUP_GESTURE, "0");
            }
        } catch (Exception e) { e.printStackTrace(); }
    }
}
