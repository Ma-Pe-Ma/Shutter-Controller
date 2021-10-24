package com.mapema.shuttercontroller.fragments;

import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.fragment.app.DialogFragment;

import com.mapema.shuttercontroller.R;
import com.mapema.shuttercontroller.RequestDispatcher;
import com.mapema.shuttercontroller.activities.MainActivity;

public class SeekbarFragment extends DialogFragment {
    private TextView progressText;
    private Button okButton;
    private SeekBar seekBar;

    int currentProgress;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);

        MainActivity mainActivity = (MainActivity) getActivity();

        View seekbarDialog = inflater.inflate(R.layout.seekbar_layout, container, false);

        seekBar = seekbarDialog.findViewById(R.id.seekbar);
        okButton = seekbarDialog.findViewById(R.id.choose);
        progressText = seekbarDialog.findViewById(R.id.value);

        currentProgress = mainActivity.getProgressBar().getProgress();

        seekBar.setProgress(currentProgress);
        progressText.setText(mainActivity.getProgressBar().getProgress()+"%");

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressText.setText(progress+"%");
                currentProgress = progress;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        okButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                RequestDispatcher.getRequestDispatcher().postNewValue(currentProgress);
                getDialog().dismiss();
            }
        });

        return seekbarDialog;
    }

    @Override
    public void onResume() {
        super.onResume();

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getActivity().getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        int height = (int) (displayMetrics.heightPixels * 0.25f);
        int width = (int) (displayMetrics.widthPixels * 0.8f);

        getDialog().getWindow().setLayout(width, height);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }
}
