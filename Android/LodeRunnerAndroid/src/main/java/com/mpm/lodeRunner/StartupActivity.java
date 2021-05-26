package com.mpm.lodeRunner;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.appcompat.widget.Toolbar;
import androidx.constraintlayout.widget.ConstraintLayout;

import android.text.Html;
import android.text.InputFilter;
import android.text.InputType;
import android.text.SpannableString;
import android.text.method.LinkMovementMethod;
import android.text.util.Linkify;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import java.util.Locale;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static android.text.InputType.TYPE_CLASS_NUMBER;
import static androidx.appcompat.app.AppCompatDelegate.MODE_NIGHT_YES;

public class StartupActivity extends AppCompatActivity {

    private static final int rowCount = 7;
    private ListView configList;
    private ConfigAdapter configAdapter;

    private int enemySpeed;
    private int playerSpeed;
    private int alphaValue;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        AppCompatDelegate.setDefaultNightMode(MODE_NIGHT_YES);

        setContentView(R.layout.startup);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        Button infoButton = toolbar.findViewById(R.id.infoButton);
        infoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showInfo();
            }
        });

        levelSelectorView = levelSelectorView();
        handlingSelectorView = handlingSelectorView();
        coverSelectorView = coverSelectorView();
        startLevelView = startLevelView();
        playerSpeedView = playerSpeedView();
        enemySpeedView = enemySpeedView();
        alphaChannelView = alphaChannelView();

        configList = findViewById(R.id.configList);
        configAdapter = new ConfigAdapter();
        configList.setAdapter(configAdapter);
        setupFloatingButton();
    }

    public class ConfigAdapter extends BaseAdapter {
        @Override
        public int getCount() {
            return rowCount;
        }

        @Override
        public Object getItem(int position) {
            return null;
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(final int position, View convertView, ViewGroup parent) {
            switch (position) {
                case 0:
                    return levelSelectorView;
                case 1:
                    return handlingSelectorView;
                case 2:
                    return coverSelectorView;
                case 3:
                    return startLevelView;
                case 4:
                    return playerSpeedView;
                case 5:
                    return enemySpeedView;
                case 6:
                    return alphaChannelView;
                default:
                    return null;
            }
        }

        @Override
        public int getItemViewType (int position) {
            if (position == 0 || position == 1 || position == 2) {
                return R.layout.radio_layout;
            }
            else if (position == 3) {
                return R.layout.level_start_layout;
            }
            else {
                return R.layout.speed_layout;
            }
        }
    }

    private View levelSelectorView;
    private View handlingSelectorView;
    private View coverSelectorView;
    private View startLevelView;
    private View playerSpeedView;
    private View enemySpeedView;
    private View alphaChannelView;

    private View levelSelectorView() {
        View view;
        view = getLayoutInflater().inflate(R.layout.radio_layout,null);
        RadioButton radioButton1 = view.findViewById(R.id.button1);
        RadioButton radioButton2 = view.findViewById(R.id.button2);

        TextView headerText = view.findViewById(R.id.headerText);
        headerText.setText(R.string.levelText);

        radioButton1.setText(R.string.originalLevels);
        radioButton2.setText(R.string.championShipLevels);

        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        boolean championship = sharedPref.getBoolean("championship", false);

        if (championship) {
            radioButton2.setChecked(true);
        }
        else {
            radioButton1.setChecked(true);
        }

        radioButton1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    editor.putBoolean("championship", false);
                    editor.apply();
                    switchUsCover(true);

                    determineLevelText(startLevelView);
                }
            }
        });

        radioButton2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    editor.putBoolean("championship", true);
                    editor.apply();

                    switchUsCover(false);
                    determineLevelText(startLevelView);
                }
            }
        });

        return view;
    }

    private View handlingSelectorView() {
        View view;

        view = getLayoutInflater().inflate(R.layout.radio_layout,null);

        RadioButton radioButton1 = view.findViewById(R.id.button1);
        RadioButton radioButton2 = view.findViewById(R.id.button2);
        TextView textView = view.findViewById(R.id.headerText);

        textView.setText(R.string.inputType);
        radioButton1.setText(R.string.jostick);
        radioButton2.setText(R.string.dpad);

        final SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        boolean joystick = sharedPref.getBoolean("joystick", true);

        if (joystick) {
            radioButton1.setChecked(true);
        }
        else {
            radioButton2.setChecked(true);
        }

        radioButton1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    SharedPreferences.Editor editor = sharedPref.edit();
                    editor.putBoolean("joystick", true);
                    editor.apply();
                }
            }
        });

        radioButton2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    SharedPreferences.Editor editor = sharedPref.edit();
                    editor.putBoolean("joystick", false);
                    editor.apply();
                }
            }
        });

        return view;
    }

    private View coverSelectorView() {
        View view;
        view = getLayoutInflater().inflate(R.layout.radio_layout,null);

        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        boolean usCover = sharedPref.getBoolean("usCover", false);

        TextView textView = view.findViewById(R.id.headerText);
        textView.setText(R.string.cover);

        RadioButton radioButton1 = view.findViewById(R.id.button1);
        RadioButton radioButton2 = view.findViewById(R.id.button2);

        radioButton1.setText("PAL");
        radioButton2.setText("NTSC");

        if (usCover) {
            radioButton2.setChecked(true);
        }
        else {
            radioButton1.setChecked(true);
        }

        radioButton1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    editor.putBoolean("usCover", false);
                    editor.apply();
                }
            }
        });

        radioButton2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPref.edit();
                    editor.putBoolean("usCover", true);
                    editor.apply();
                }
            }
        });

        return view;
    }

    private View startLevelView() {
        View view;
        view = getLayoutInflater().inflate(R.layout.level_start_layout,null);

        final RadioButton radioButton1 = view.findViewById(R.id.lastPlayed);
        final RadioButton radioButton2 = view.findViewById(R.id.chosen);
        final EditText editText = view.findViewById(R.id.chosenInput);

        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);

        //TODO: this should be at elsewhere
        switchUsCover(!sharedPref.getBoolean("championship", false));

        radioButton1.setChecked(true);
        editText.setClickable(false);
        editText.setEnabled(false);
        editText.setInputType(InputType.TYPE_CLASS_NUMBER);

        determineLevelText(view);

        radioButton2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    if (!editText.hasFocus()) {
                        editText.setEnabled(true);
                        //editText.setClickable(true);
                        editText.requestFocus();

                        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                        imm.showSoftInput(editText, InputMethodManager.SHOW_IMPLICIT);
                        //imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_IMPLICIT_ONLY);
                    }
                }
                else {
                    editText.clearFocus();
                    editText.setClickable(false);
                    editText.setEnabled(false);

                    InputMethodManager imm = (InputMethodManager) StartupActivity.this.getSystemService(Activity.INPUT_METHOD_SERVICE);
                    //Find the currently focused view, so we can grab the correct window token from it.
                    View view = StartupActivity.this.getCurrentFocus();
                    //If no view currently has focus, create a new one, just so we can grab a window token from it
                    if (view == null) {
                        view = new View(StartupActivity.this);
                    }
                    imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
                }
            }
        });

        return view;
    }

    private View playerSpeedView() {
        View view;
        view = getLayoutInflater().inflate(R.layout.speed_layout,null);

        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        float playerSpeed = sharedPref.getFloat("playerSpeed", 0.9f);
        StartupActivity.this.playerSpeed = (int) (playerSpeed * 100);

        final TextView valueText = view.findViewById(R.id.progressValue);

        final SeekBar seekBar = view.findViewById(R.id.speedProgress);
        seekBar.setProgress((int) (playerSpeed * 100));

        TextView textView = view.findViewById(R.id.speedText);
        textView.setText(R.string.playerSpeed);
        textView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                StartupActivity.this.playerSpeed = 90;
                valueText.setText(""+StartupActivity.this.playerSpeed);
                seekBar.setProgress(StartupActivity.this.playerSpeed);
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                StartupActivity.this.playerSpeed = progress;
                valueText.setText(""+progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        //progressBar.setOn

        TextView seekbarText = view.findViewById(R.id.progressValue);
        seekbarText.setText(""+seekBar.getProgress());

        return view;
    }

    private View enemySpeedView() {
        View view;
        view = getLayoutInflater().inflate(R.layout.speed_layout,null);

        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        final float enemySpeed = sharedPref.getFloat("enemySpeed", 0.415f);
        StartupActivity.this.enemySpeed = (int) (enemySpeed * 100);

        final TextView valueText = view.findViewById(R.id.progressValue);

        final SeekBar seekBar = view.findViewById(R.id.speedProgress);
        seekBar.setProgress((int) (enemySpeed * 100));

        TextView textView = view.findViewById(R.id.speedText);
        textView.setText(R.string.enemySpeed);
        textView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                StartupActivity.this.enemySpeed = 41;
                valueText.setText(""+ StartupActivity.this.enemySpeed);
                seekBar.setProgress(StartupActivity.this.enemySpeed);
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                StartupActivity.this.enemySpeed = progress;
                valueText.setText(""+progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        TextView progressText = view.findViewById(R.id.progressValue);
        progressText.setText(""+seekBar.getProgress());

        return view;
    }

    private View alphaChannelView() {
        View view;
        view = getLayoutInflater().inflate(R.layout.speed_layout,null);

        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        final float alphaValue = sharedPref.getFloat("alphaValue", 0.5f);
        StartupActivity.this.alphaValue = (int) (alphaValue * 100);

        final TextView valueText = view.findViewById(R.id.progressValue);

        final SeekBar seekBar = view.findViewById(R.id.speedProgress);
        seekBar.setProgress((int) (alphaValue * 100));

        TextView textView = view.findViewById(R.id.speedText);
        textView.setText(R.string.alphaText);
        textView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                StartupActivity.this.alphaValue = 50;
                valueText.setText(String.format(Locale.getDefault(), "%2d", StartupActivity.this.alphaValue));
                seekBar.setProgress(StartupActivity.this.alphaValue);
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                StartupActivity.this.alphaValue = progress;
                valueText.setText(String.format(Locale.getDefault(), "%2d", progress));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        valueText.setText(""+seekBar.getProgress());

        return view;
    }

    private void determineLevelText(View view) {
        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);

        boolean championship = sharedPref.getBoolean("championship", false);
        int max;
        int lastLevel;

        if (championship) {
            max = 51;
            lastLevel = sharedPref.getInt("champLastLevel", 1);
        } else {
            max = 150;
            lastLevel = sharedPref.getInt("oriLastLevel", 1);
        }

        EditText editText = view.findViewById(R.id.chosenInput);
        editText.setFilters(new InputFilter[]{new LevelFilter(1, max)});

        if(!editText.getText().toString().equals("")) {
            int fieldValue = Integer.parseInt(editText.getText().toString());

            if (fieldValue > max) {
                editText.setText("");
            }
        }

        String levelText = getResources().getString(R.string.lastlevelText) + (" (1 - " + max + ")");
        TextView textView = view.findViewById(R.id.levelTextView);
        textView.setText(levelText);

        String lastPlayedLevel = getResources().getString(R.string.lastPlayedLevel) + " (" + lastLevel +")";
        RadioButton radioButton1 = view.findViewById(R.id.lastPlayed);
        radioButton1.setText(lastPlayedLevel);
    }

    private void switchUsCover(boolean state) {
        RadioButton radioButton1 = coverSelectorView.findViewById(R.id.button1);
        radioButton1.setEnabled(state);
        RadioButton radioButton2 = coverSelectorView.findViewById(R.id.button2);
        radioButton2.setEnabled(state);

        TextView textView = coverSelectorView.findViewById(R.id.headerText);
        textView.setEnabled(state);
    }

    private void setupFloatingButton() {
        FloatingActionButton floatingActionButton = findViewById(R.id.launchGame);
        floatingActionButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                proceedToGame();
            }
        });

        /*FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });*/
    }

    @SuppressLint("ApplySharedPref")
    private void proceedToGame() {
        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.configPreferences), Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();

        editor.putFloat("enemySpeed", ((float) enemySpeed) / 100) ;
        editor.putFloat("playerSpeed", ((float)  playerSpeed) / 100);
        editor.putFloat("alphaValue", ((float) alphaValue) / 100);

        boolean levelChooseMethod = ((RadioButton) startLevelView.findViewById(R.id.lastPlayed)).isChecked();
                //sharedPref.getBoolean("levelChooseMethod", true);

        if (!levelChooseMethod) {
            EditText editText = startLevelView.findViewById(R.id.chosenInput);
            String string = editText.getText().toString();

            if (string.length() == 0) {
                showNotificationAboutEmptyField();
            }
            else {
                boolean championship = sharedPref.getBoolean("championship", false);
                int levelNr = Integer.parseInt(string);

                if (championship) {
                    editor.putInt("champLastLevel", levelNr);
                }
                else {
                    editor.putInt("oriLastLevel", levelNr);
                }

                editor.commit();

                Intent gameActivityIntent = new Intent(StartupActivity.this, GameActivity.class);
                startActivity(gameActivityIntent);
                finish();

                InputMethodManager imm = (InputMethodManager) getSystemService(Activity.INPUT_METHOD_SERVICE);
                //Find the currently focused view, so we can grab the correct window token from it.
                View view = getCurrentFocus();
                imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
            }
        }
        else {
            editor.commit();
            Intent gameActivityIntent = new Intent(StartupActivity.this, GameActivity.class);
            startActivity(gameActivityIntent);
            finish();

            InputMethodManager imm = (InputMethodManager) getSystemService(Activity.INPUT_METHOD_SERVICE);
            //Find the currently focused view, so we can grab the correct window token from it.
            View view = getCurrentFocus();
            imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
        }
    }

    private void showNotificationAboutEmptyField() {
        AlertDialog.Builder builder = new AlertDialog.Builder(StartupActivity.this);

        builder.setTitle("Hiányzó adat!");
        builder.setMessage("Nem adott meg kezdő pályát, kérem adjon meg egyet!");
        builder.setPositiveButton("Rendben", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

            }
        });

        builder.create().show();
    }

    private void showInfo() {
        AlertDialog.Builder builder = new AlertDialog.Builder(StartupActivity.this);

        final View infoDialogLayout = getLayoutInflater().inflate(R.layout.info_dialog_layout, null);

        final TextView creatorTextView = infoDialogLayout.findViewById(R.id.creatorTextView);
        final TextView urlTextView = infoDialogLayout.findViewById(R.id.creatorURLView);

        creatorTextView.setText(getString(R.string.creator));

        urlTextView.setText(getString(R.string.websiteDesc));
        Pattern pattern = Pattern.compile(getString(R.string.websiteDesc));
        String scheme = getString(R.string.websiteURL);
        Linkify.addLinks(urlTextView, pattern, scheme, null, new Linkify.TransformFilter() {
            @Override
            public String transformUrl(Matcher match, String url) {
                return "";
            }
        });


        /*SpannableString message = new SpannableString(getString(R.string.websiteURL));
        Linkify.addLinks(message, Linkify.WEB_URLS);
        urlTextView.setText(message);
        urlTextView.setMovementMethod(LinkMovementMethod.getInstance());*/

        builder.setTitle(getString(R.string.note));
        //builder.setMessage(message);
        builder.setPositiveButton(getString(R.string.back), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

            }
        });
        builder.setView(infoDialogLayout);

        builder.create().show();
    }
}