package com.example.rednykapcsol.fragments;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.fragment.app.Fragment;

import com.example.rednykapcsol.FragmentCommunicating;
import com.example.rednykapcsol.R;
import com.example.rednykapcsol.Timing;
import com.example.rednykapcsol.WeekDay;

import org.joda.time.LocalTime;

import java.util.HashMap;
import java.util.Map;

public class  TimingConfigFragment extends DialogFragment {
    private int position;
    private int value;
    private Timing timing;

    private Map<WeekDay, CheckBox> checkMap;
    private Map<WeekDay, Boolean> days;

    private TextView valueText;
    private TimePicker picker;

    private FragmentCommunicating fragmentCommunicating;

    public TimingConfigFragment(int position) {
        this.position = position;
        this.timing = Timing.getTimings()[position];
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        fragmentCommunicating = (FragmentCommunicating) getActivity();
    }

    @Override
    public void onDismiss(DialogInterface dialogInterface) {
        super.onDismiss(dialogInterface);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onCancel(DialogInterface dialogInterface) {
        super.onCancel(dialogInterface);
        fragmentCommunicating.notifySelectorAboutCanceling(position);
        Log.i("DEBUG", "CANCELLED");
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {

        View timingConfig = getLayoutInflater().inflate(R.layout.timing_config, container, false);

        TextView title = timingConfig.findViewById(R.id.title);
        title.setText(position + 1 +". ütemezés");

        setupPicker(timingConfig);
        setupSeekbar(timingConfig);
        setupDayChecker(timingConfig);
        setupButtons(timingConfig);

        return timingConfig;
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    private void setupPicker(View timingConfig) {
        picker = (TimePicker) timingConfig.findViewById(R.id.time_picker);
        picker.setIs24HourView(true);
        LocalTime time = timing.getTime();
        if (time != null) {
            picker.setHour(time.getHourOfDay());
            picker.setMinute(time.getMinuteOfHour());
        }
        else {
            picker.setHour(12);
            picker.setMinute(0);
        }
    }

    private void setupSeekbar(View timingConfig) {

        valueText = (TextView) timingConfig.findViewById(R.id.value_text);
        valueText.setText(timing.getValue()+"%");

        SeekBar seekBar = (SeekBar) timingConfig.findViewById(R.id.seekbar);
        value = timing.getValue();
        seekBar.setProgress(value);

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

                value = progress;
                valueText.setText(progress+"%");

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    private void setupDayChecker(View timingConfig) {
        checkMap = new HashMap<>();

        checkMap.put(WeekDay.MONDAY, (CheckBox) timingConfig.findViewById(R.id.check_Mo));
        checkMap.put(WeekDay.TUESDAY, (CheckBox) timingConfig.findViewById(R.id.check_Tu));
        checkMap.put(WeekDay.WEDNESDAY, (CheckBox) timingConfig.findViewById(R.id.check_We));
        checkMap.put(WeekDay.THURSDAY, (CheckBox) timingConfig.findViewById(R.id.check_Th));
        checkMap.put(WeekDay.FRIDAY, (CheckBox) timingConfig.findViewById(R.id.check_Fr));
        checkMap.put(WeekDay.SATURDAY, (CheckBox) timingConfig.findViewById(R.id.check_Sa));
        checkMap.put(WeekDay.SUNDAY, (CheckBox) timingConfig.findViewById(R.id.check_Su));

        if (timing.getDays() != null) {
            for (WeekDay weekDay : WeekDay.values()) {
                checkMap.get(weekDay).setChecked(timing.getDays().get(weekDay));
            }
        }
        else {
            for (WeekDay weekDay : WeekDay.values()) {
                checkMap.get(weekDay).setChecked(false);
            }
        }
    }

    private void setupButtons(View timingConfig) {
        Button okButton = timingConfig.findViewById(R.id.okButton);
        okButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                LocalTime time = new LocalTime(picker.getHour(), picker.getMinute());
                boolean inactive = getStatesFromCheckBoxes();

                if (inactive) {
                    days = null;
                    Context context = getActivity().getApplicationContext();
                    CharSequence text = "Kérem adjon meg legalább 1 napot!";
                    int duration = Toast.LENGTH_SHORT;

                    Toast toast = Toast.makeText(context, text, duration);
                    toast.show();

                } else {
                    fragmentCommunicating.notifySelectorAboutNewTiming(position, new Timing(value, days, time));
                    dismiss();
                }
            }
        });

        ImageButton closeButton = timingConfig.findViewById(R.id.close);
        closeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getDialog().cancel();
            }
        });
    }

    private boolean getStatesFromCheckBoxes() {
        days = new HashMap<>();

        boolean inactive = true;

        for (WeekDay weekDay : WeekDay.values()) {
            boolean state = checkMap.get(weekDay).isChecked();
            if (state) {
                inactive = false;
            }
            days.put(weekDay, state);
        }

        return inactive;
    }
}
