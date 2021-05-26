/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.mpm.lodeRunner;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.NativeActivity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.WindowManager.LayoutParams;
import android.widget.ImageButton;
import android.widget.PopupWindow;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.mpm.helper.NDKHelper;

import java.util.Locale;

import io.github.controlwear.virtual.joystick.android.JoystickView;

public class GameActivity extends NativeActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        
        if(SDK_INT >= 19)
        {
            setImmersiveSticky();
            View decorView = getWindow().getDecorView();
            decorView.setOnSystemUiVisibilityChangeListener
                    (new View.OnSystemUiVisibilityChangeListener() {
                @Override
                public void onSystemUiVisibilityChange(int visibility) {
                    setImmersiveSticky();
                }
            });
        }

        processConfig();
        ndkHelper = new NDKHelper(GameActivity.this);
    }

    @TargetApi(19)
    protected void onResume() {
        super.onResume();

        //Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        if(SDK_INT >= 11 && SDK_INT < 14)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
        }
        else if(SDK_INT >= 14 && SDK_INT < 19)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LOW_PROFILE);
        }
        else if(SDK_INT >= 19)
        {
            setImmersiveSticky();
        }
    }

    protected void onPause()
    {
        super.onPause();
    }
    // Our popup window, you will call it from your C/C++ code later

    @TargetApi(19)
    void setImmersiveSticky() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    GameActivity _activity;
    PopupWindow _popupWindow;
    TextView _label;

    static {
        System.loadLibrary("LodeRunnerNativeActivity");
    }

    @Override
    public void onBackPressed() {
        Log.d("CPP", "onBackPressed Called");
        Intent gameActivityIntent = new Intent(GameActivity.this, StartupActivity.class);
        startActivity(gameActivityIntent);
        finish();
    }

    @SuppressLint("InflateParams")
    public void showUI()
    {
        if( _popupWindow != null )
            return;

        _activity = this;

        this.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                LayoutInflater layoutInflater = (LayoutInflater)getBaseContext()
                    .getSystemService(LAYOUT_INFLATER_SERVICE);
                View popupView = layoutInflater.inflate(R.layout.widgets, null);
                _popupWindow = new PopupWindow(popupView, LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);

                ConstraintLayout mainLayout = new ConstraintLayout(_activity);
                MarginLayoutParams params = new MarginLayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
                params.setMargins(0, 0, 0, 0);
                _activity.setContentView(mainLayout, params);

                // Show our UI over NativeActivity window
                _popupWindow.showAtLocation(mainLayout, Gravity.TOP | Gravity.START, 0, 0);
                _popupWindow.update();

                _label = (TextView) popupView.findViewById(R.id.textViewFPS);
                setupButtons(popupView);
            }});
    }

    public void updateFPS(final float fFPS) {
        if( _label == null )
            return;

        _activity = this;
        this.runOnUiThread(new Runnable()  {
            @Override
            public void run()  {
                _label.setText(String.format(Locale.getDefault(), "%2.2f FPS", fFPS));
            }});
    }

    ImageButton up;
    ImageButton down;
    ImageButton right;
    ImageButton left;

    ImageButton leftDig;
    ImageButton rightDig;

    ImageButton select;
    ImageButton start;

    io.github.controlwear.virtual.joystick.android.JoystickView joystickView;

    private void setupButtons(View popupView) {
        SharedPreferences sharedPreferences = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        boolean joystick = sharedPreferences.getBoolean("joystick", true);

        up = (ImageButton) popupView.findViewById(R.id.up);
        right = (ImageButton) popupView.findViewById(R.id.right);
        down = (ImageButton) popupView.findViewById(R.id.down);
        left = (ImageButton) popupView.findViewById(R.id.left);
        joystickView = (JoystickView) popupView.findViewById(R.id.joystickView);

        if (joystick) {
            up.setClickable(false);
            up.setVisibility(View.INVISIBLE);
            up.setEnabled(false);

            right.setClickable(false);
            right.setVisibility(View.INVISIBLE);
            right.setEnabled(false);

            down.setClickable(false);
            down.setVisibility(View.INVISIBLE);
            down.setEnabled(false);

            left.setClickable(false);
            left.setVisibility(View.INVISIBLE);
            left.setEnabled(false);

            joystickView.setOnMoveListener(new JoystickView.OnMoveListener() {
                @Override
                public void onMove(int angle, int strength) {
                    processJoystick(angle, strength);
                }
            }, 17);
        }
        else {
            joystickView.setEnabled(false);
            joystickView.setVisibility(View.INVISIBLE);
            joystickView.setClickable(false);

            setupButton(up, 0);
            setupButton(right, 1);
            setupButton(down, 2);
            setupButton(left, 3);
        }

        leftDig = (ImageButton) popupView.findViewById(R.id.leftDig);
        setupButton(leftDig, 4);

        rightDig = (ImageButton) popupView.findViewById(R.id.rightDig);
        setupButton(rightDig, 5);

        select = (ImageButton) popupView.findViewById(R.id.selectButton);
        setupButton(select, 6);

        start = (ImageButton) popupView.findViewById(R.id.startButton);
        setupButton(start, 7);
    }

    @SuppressLint("ClickableViewAccessibility")
    private void setupButton(ImageButton button, final int ID) {
        //button.setClickable(true);
        SharedPreferences sharedPreferences = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);

        button.setAlpha(1.0f - sharedPreferences.getFloat("alphaValue", 0.5f));

        button.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                processInput(ID, true);
            }
            else if (event.getAction() == MotionEvent.ACTION_UP) {
                processInput(ID,false);
            }

            return false;
            }
        });
    }

    NDKHelper ndkHelper;

    private void processConfig() {
        SharedPreferences sharedPreferences = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);

        boolean championship = sharedPreferences.getBoolean("championship", false);
        boolean usCover = sharedPreferences.getBoolean("usCover", false);
        boolean joystick = sharedPreferences.getBoolean("joystick", true);

        int level;

        if (championship) {
            level = sharedPreferences.getInt("champLastLevel", 1);
        }
        else {
            level = sharedPreferences.getInt("oriLastLevel", 1);
        }

        float playerSpeed = sharedPreferences.getFloat("playerSpeed", 0.9f);
        float enemySpeed = sharedPreferences.getFloat("enemySpeed", 0.415f);

        initializeGame(championship, usCover, joystick, level, playerSpeed, enemySpeed);
    }

    public native void initializeGame(boolean champ, boolean usCover, boolean joystick, int level, float player, float enemy);
    public native void processInput(int buttonID, boolean pushed);
    public native void processJoystick(int angle, int strength);
}


