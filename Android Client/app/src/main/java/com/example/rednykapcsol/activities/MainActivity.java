package com.example.rednykapcsol.activities;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.example.rednykapcsol.FragmentCommunicating;
import com.example.rednykapcsol.R;
import com.example.rednykapcsol.RequestDispatcher2;
import com.example.rednykapcsol.Timing;
import com.example.rednykapcsol.ZeroState;
import com.example.rednykapcsol.fragments.SeekbarFragment;
import com.example.rednykapcsol.fragments.TimingSelectorFragment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;

public class MainActivity extends AppCompatActivity implements FragmentCommunicating {
    private Button connectButton;
    private EditText messageSendText;
    private Button sendButton;

    private ImageButton up;
    private ImageButton down;

    private ImageButton zeroButton;
    private ImageButton timingButton;

    private ImageButton upMax;
    private ImageButton downMax;

    private PrintWriter output;
    private BufferedReader input;

    private ProgressBar progressBar;
    private TextView progressText;

    private int inactiveColor;
    private int activeColor;
    private final String selectorTag = "selector";
    private final String configTag= "config";

    //TODO: automatikus nullázás reggel/este?
    //TODO: újraindítási figyelmeztetés az utolsó 10 napban
    //TODO: ütemezés
    //TODO: dátum+időbeállító
    //TODO: Óraátállítás? idő lekérése egy szerverről esp-vel?
    //TODO: Nyilak a csúszkához?
    //TODO: SESSION-ÖK!

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setupButtons();
        setupProgressBar();

        Timing.initialize(this);

        inactiveColor = ContextCompat.getColor(MainActivity.this,R.color.colorGray);
        activeColor = ContextCompat.getColor(MainActivity.this,R.color.colorGreen);
    }

    @Override
    protected void onResume() {
        super.onResume();
        RequestDispatcher2.getRequestDispatcher().setContext(this.getApplicationContext());
    }

    @Override
    protected void onPause () {
        super.onPause();
        RequestDispatcher2.getRequestDispatcher().setContext(null);
    }

    private void setupProgressBar() {

        upMax = findViewById(R.id.upMax);
        upMax.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        downMax = findViewById(R.id.downMax);
        downMax.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        progressBar = findViewById(R.id.vertical_progressbar);
        progressBar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SeekbarFragment seekbarFragment = new SeekbarFragment();
                seekbarFragment.show(getSupportFragmentManager(), seekbarFragment.getTag());
            }
        });

        progressText = findViewById(R.id.progressText);

        progressText.setText("50%");
        progressText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SeekbarFragment seekbarFragment = new SeekbarFragment();
                seekbarFragment.show(getSupportFragmentManager(), seekbarFragment.getTag());
            }
        });
    }

    private void setupButtons() {
        up = findViewById(R.id.up);
        up.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                constantUp();
            }
        });

        //up.setColorFilter(Color.argb(255, 0, 255, 255)); // White Tint

        down = findViewById(R.id.down);
        down.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                constantDown();
            }
        });

        down.setEnabled(false);

        zeroButton = findViewById(R.id.zero);
        zeroButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog alertDialog = zeroingQuestion();
                alertDialog.show();
            }
        });

        timingButton = findViewById(R.id.clock);
        timingButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TimingSelectorFragment timingSelectorFragment = new TimingSelectorFragment();
                timingSelectorFragment.show(getSupportFragmentManager(), selectorTag);
            }
        });
    }

    private void constantUp() {

    }

    private void constantDown() {

    }

    private AlertDialog zeroingQuestion() {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        //final View dialogLayout = getLayoutInflater().inflate(R.layout.alert_layout, null);
        //builder.setView(dialogLayout);
        builder.setPositiveButton("Igen", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

                AlertDialog zeroingChoose = zeroingChoose();
                zeroingChoose.show();
            }
        });

        builder.setNegativeButton("Nem", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

            }
        });

        AlertDialog zeroDialog = builder.create();
        zeroDialog.setTitle("Szeretné nullázni a redőny pozícióját?");

        return zeroDialog;
    }

    private AlertDialog zeroingChoose() {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);

        builder.setPositiveButton("Fent", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                RequestDispatcher2.getRequestDispatcher().postZeroState(ZeroState.up);
            }
        });

        builder.setNeutralButton("Nem tudom", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

                AlertDialog zeroingAutomatic = zeroingAutomatic();
                zeroingAutomatic.show();

            }
        });
        builder.setNegativeButton("Lent", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                RequestDispatcher2.getRequestDispatcher().postZeroState(ZeroState.down);
            }
        });

        builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                AlertDialog zeroingAutomatic = zeroingAutomatic();
                zeroingAutomatic.show();
            }
        });

        AlertDialog zeroChooseDialog = builder.create();
        zeroChooseDialog.setTitle("Hol található a redőny?");

        return zeroChooseDialog;
    }

    private AlertDialog zeroingAutomatic() {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        //final View dialogLayout = getLayoutInflater().inflate(R.layout.alert_layout, null);
        //builder.setView(dialogLayout);
        builder.setPositiveButton("Igen", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                RequestDispatcher2.getRequestDispatcher().postZeroState(ZeroState.find);
            }
        });

        builder.setNegativeButton("Nem", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

            }
        });

        AlertDialog zeroAutomatic = builder.create();
        zeroAutomatic.setTitle("Megkeressem automatikusan?");

        return zeroAutomatic;
    }

    public ProgressBar getProgressBar() {
        return progressBar;
    }

    @Override
    public void notifySelectorAboutNewTiming(int position, Timing timing) {
        TimingSelectorFragment timingSelectorFragment = (TimingSelectorFragment) getSupportFragmentManager().findFragmentByTag(selectorTag);
        timingSelectorFragment.saveSettings(position, timing);
    }

    @Override
    public void notifySelectorAboutCanceling(int position) {
        TimingSelectorFragment timingSelectorFragment = (TimingSelectorFragment) getSupportFragmentManager().findFragmentByTag(selectorTag);
        timingSelectorFragment.cancelSetup(position);
    }

    @Override
    public void notifyConfig() {

    }
}
