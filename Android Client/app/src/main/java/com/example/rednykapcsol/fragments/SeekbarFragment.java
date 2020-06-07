package com.example.rednykapcsol.fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatDialogFragment;
import androidx.fragment.app.DialogFragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.lifecycle.ViewModelProviders;

import com.example.rednykapcsol.R;
import com.example.rednykapcsol.activities.MainActivity;

import java.util.Objects;

public class SeekbarFragment extends DialogFragment {
    private TextView progressText;
    private Button okButton;
    private SeekBar seekBar;

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

        seekBar.setProgress(mainActivity.getProgressBar().getProgress());
        progressText.setText(mainActivity.getProgressBar().getProgress()+"%");

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressText.setText(progress+"%");
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

            getDialog().dismiss();
            }
        });

        return seekbarDialog;
    }

    @Override
    public void onResume() {
        super.onResume();
        getDialog().getWindow().setLayout(600, 400);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }
}
