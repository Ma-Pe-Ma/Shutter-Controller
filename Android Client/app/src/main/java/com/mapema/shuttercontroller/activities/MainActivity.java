package com.mapema.shuttercontroller.activities;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mapema.shuttercontroller.ActivityNotifier;
import com.mapema.shuttercontroller.FragmentNotifier;
import com.mapema.shuttercontroller.MessageAdapter;
import com.mapema.shuttercontroller.MessageContainer;
import com.mapema.shuttercontroller.R;
import com.mapema.shuttercontroller.RequestDispatcher;
import com.mapema.shuttercontroller.Timing;
import com.mapema.shuttercontroller.fragments.SeekbarFragment;
import com.mapema.shuttercontroller.fragments.TimingSelectorFragment;
import com.mapema.shuttercontroller.fragments.ZeroChoose;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements ActivityNotifier {
    private ImageButton zeroButton;
    private ImageButton timingButton;

    private ImageButton upMax;
    private ImageButton downMax;

    private TextView startDateText;
    private TextView stateText;

    private ProgressBar progressBar;
    private TextView progressText;

    private int inactiveColor;
    private int activeColor;
    private final String selectorTag = "selector";
    private final String seekBarTag = "seekbar";
    private final String zeroTag = "zero";

    private List<String> dialogTags;

    RecyclerView recyclerView;
    MessageAdapter messageAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setupButtons();
        setupProgressBar();

        Timing.initialize(this);

        inactiveColor = ContextCompat.getColor(MainActivity.this, R.color.colorGray);
        activeColor = ContextCompat.getColor(MainActivity.this, R.color.colorGreen);

        recyclerView = findViewById(R.id.messageRecyclerView);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        messageAdapter = new MessageAdapter();
        recyclerView.setAdapter(messageAdapter);

        startDateText = findViewById(R.id.startDateText);
        startDateText.setText(getString(R.string.startDate, getString(R.string.hyphen)));

        stateText = findViewById(R.id.stateText);

        dialogTags = new ArrayList<>();
        dialogTags.add(selectorTag);
        dialogTags.add(TimingSelectorFragment.configTag);
        dialogTags.add(seekBarTag);

    }

    @Override
    protected void onResume() {
        super.onResume();
        RequestDispatcher.getRequestDispatcher().createRequestQueue(this);
        RequestDispatcher.getRequestDispatcher().subscribe(this);
        RequestDispatcher.getRequestDispatcher().getDump();

        for (String tag : dialogTags) {
            Fragment prev = getSupportFragmentManager().findFragmentByTag(tag);
            if (prev != null) {
                DialogFragment df = (DialogFragment) prev;
                df.dismiss();
            }
        }
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
                seekbarFragment.show(getSupportFragmentManager(), seekBarTag);
            }
        });

        progressText = findViewById(R.id.progressText);

        progressText.setText(getString(R.string.percentage, 50));
        progressText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SeekbarFragment seekbarFragment = new SeekbarFragment();
                seekbarFragment.show(getSupportFragmentManager(), seekBarTag);
            }
        });
    }

    private void setupButtons() {
        zeroButton = findViewById(R.id.zero);
        zeroButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ZeroChoose zeroChoose = new ZeroChoose();
                zeroChoose.show(getSupportFragmentManager(), zeroTag);
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
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                messageAdapter.notifyDataSetChanged();
                String startString = getString(R.string.startDate, MessageContainer.getMessageContainer().getStartupTime());
                startDateText.setText(startString);
            }
        });
    }

    @Override
    public void notifyAboutTimedUpdate(int secondsTillUpdate) {

    }

    @Override
    public void notifySynchronisation() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                stateText.setText(getString(R.string.serverState, getString(R.string.syncInProgress)));
            }
        });
    }

    @Override
    public void notifyTimeout() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyDisableGUI();
                stateText.setText(getString(R.string.serverState, getString(R.string.serverUnavailable)));
            }
        });
    }

    @Override
    public void notifyNewValue(int newValue) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                progressBar.setProgress(newValue);
                progressText.setText(getString(R.string.percentage, newValue));
            }
        });
    }

    @Override
    public void notifyEnableGUI() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                progressBar.setEnabled(true);
                timingButton.setEnabled(true);
                upMax.setEnabled(true);
                downMax.setEnabled(true);
                zeroButton.setEnabled(true);
                progressText.setEnabled(true);
                stateText.setText(getString(R.string.serverState, getString(R.string.serverAvailable)));
            }
        });
    }

    @Override
    public void notifyDisableGUI() {
        progressBar.setEnabled(false);
        timingButton.setEnabled(false);
        upMax.setEnabled(false);
        downMax.setEnabled(false);
        zeroButton.setEnabled(false);
        progressText.setEnabled(false);
    }
}
