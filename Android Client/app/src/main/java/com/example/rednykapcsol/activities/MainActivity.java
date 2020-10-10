package com.example.rednykapcsol.activities;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.example.rednykapcsol.ActivityNotifier;
import com.example.rednykapcsol.FragmentNotifier;
import com.example.rednykapcsol.MessageContainer;
import com.example.rednykapcsol.R;
import com.example.rednykapcsol.RequestDispatcher;
import com.example.rednykapcsol.Timing;
import com.example.rednykapcsol.fragments.InfoDialogFragment;
import com.example.rednykapcsol.fragments.SeekbarFragment;
import com.example.rednykapcsol.fragments.TimeoutDialogFragment;
import com.example.rednykapcsol.fragments.TimingSelectorFragment;
import com.example.rednykapcsol.fragments.ZeroQuestion;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements ActivityNotifier {
    private ImageButton zeroButton;
    private ImageButton timingButton;

    private ImageButton upMax;
    private ImageButton downMax;

    private ImageButton infoButton;

    private ProgressBar progressBar;
    private TextView progressText;

    private int inactiveColor;
    private int activeColor;
    private final String selectorTag = "selector";
    private final String configTag = "config";

    //Working mechanism
    //TODO: BLACKOUT ZEROING?
    //TODO: AUTOMATIC ZERO AT MIDNIGHT?

    //GUI
    //TODO: GUI cleanup
    //TODO: ACTIVE/NON-ACTIVE IMAGE BUTTON
    //TODO: NOTIFICATION DIALOGS
    //TODO: MESSAGE PROCESSINMG

    //SERVER
    //TODO: ESP SERVER CONFIGURATION, SSH + user credidentials, easyddns?
    //TODO: ESP JSON PARSER MEMORY MANEGEMENT?
    //TODO: újraindítási figyelmeztetés az utolsó 10 napban/ ESP 64bit timing?

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
        RequestDispatcher.getRequestDispatcher().subscribe(this);
        RequestDispatcher.getRequestDispatcher().getDump();
    }

    @Override
    protected void onPause () {
        super.onPause();
        RequestDispatcher.getRequestDispatcher().unSubscribe(this);
        RequestDispatcher.getRequestDispatcher().stopCyclicUpdate();
    }

    private void setupProgressBar() {
        upMax = findViewById(R.id.upMax);
        upMax.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                RequestDispatcher.getRequestDispatcher().postNewValue(100);
            }
        });

        downMax = findViewById(R.id.downMax);
        downMax.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                RequestDispatcher.getRequestDispatcher().postNewValue(0);
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
        infoButton = findViewById(R.id.infoButton);
        infoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showMessageWindow(0);
            }
        });

        zeroButton = findViewById(R.id.zero);
        zeroButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ZeroQuestion zeroQuestion = new ZeroQuestion();
                zeroQuestion.show(getSupportFragmentManager(), "ZEROQ");
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

    public void showMessageWindow(int size) {
        InfoDialogFragment infoDialogFragment = (InfoDialogFragment) getSupportFragmentManager().findFragmentByTag("INFO");

        if (infoDialogFragment != null) {
            infoDialogFragment.dismiss();
            getSupportFragmentManager().beginTransaction();
        }

        List<String> filteredMessages = MessageContainer.getMessageContainer().getFilteredMessages(this, size);

        infoDialogFragment = new InfoDialogFragment(filteredMessages);
        infoDialogFragment.show(getSupportFragmentManager(), "INFO");
    }

    public ProgressBar getProgressBar() {
        return progressBar;
    }

    private List<FragmentNotifier> fragmentNotifiers = new ArrayList<>();

    public void subscribeToFragmentNotifications(FragmentNotifier fragmentNotifier) {
        if (!fragmentNotifiers.contains(fragmentNotifier)) {
            fragmentNotifiers.add(fragmentNotifier);
        }
    }

    public void unsubscribeToFragmentNotifications(FragmentNotifier fragmentNotifier) {
        fragmentNotifiers.remove(fragmentNotifier);
    }

    public void notifySelectorAboutNewTiming(int position, Timing timing) {
        for(FragmentNotifier fragmentNotifier : fragmentNotifiers) {
            fragmentNotifier.notifySelectorAboutNewTiming(position, timing);
        }
    }

    public void notifySelectorAboutCanceling(int position) {
        for(FragmentNotifier fragmentNotifier : fragmentNotifiers) {
            fragmentNotifier.notifySelectorAboutCanceling(position);
        }
    }

    @Override
    public void notifyMessage(int size) {
        showMessageWindow(size);
    }

    @Override
    public void notifyAboutTimedUpdate(int secondsTillUpdate) {

    }

    @Override
    public void notifyTimeout() {
        TimeoutDialogFragment timeoutDialogFragment = new TimeoutDialogFragment();
        timeoutDialogFragment.show(getSupportFragmentManager(), "TIMEOUT");
    }

    @Override
    public void notifyNewValue(float newValue) {
        int newValueInt = (int) (newValue * 100);
        progressBar.setProgress(newValueInt);
        progressText.setText("" + newValueInt + "%");
    }

    @Override
    public void notifyEnableGUI() {
        progressBar.setEnabled(true);
        timingButton.setEnabled(true);
        upMax.setEnabled(true);
        downMax.setEnabled(true);
        zeroButton.setEnabled(true);
        progressText.setEnabled(true);
    }

    @Override
    public void notifyDisableGUI() {
        progressBar.setEnabled(false);
        timingButton.setEnabled(false);
        upMax.setEnabled(false);
        downMax.setEnabled(false);
        zeroButton.setEnabled(false);
        progressText.setEnabled(false);

        //TODO: message about disabled gui?
    }
}
