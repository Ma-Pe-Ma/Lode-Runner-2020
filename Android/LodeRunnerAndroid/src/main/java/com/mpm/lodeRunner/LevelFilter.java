package com.mpm.lodeRunner;

import android.text.InputFilter;
import android.text.Spanned;

public class LevelFilter implements InputFilter {
    private int min = 1;
    private int max = 150;

    public LevelFilter(int min, int max) {
        this.min = min;
        this.max = max;
    }

    @Override
    public CharSequence filter(CharSequence source, int start, int end, Spanned dest, int dstart, int dend) {
        try {
            int input = Integer.parseInt(dest.toString() + source.toString());
            if (isInRange(input)) {
                return null;
            }
        } catch (NumberFormatException nfe) { }
        return "";
    }

    private boolean isInRange(int input) {
        if (input >= min && input <= max) {
            return true;
        }

        return false;
    }
}