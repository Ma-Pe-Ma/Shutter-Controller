package com.example.rednykapcsol.fragments;

import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CompoundButton;
import android.widget.ImageButton;

import android.view.LayoutInflater;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;

import androidx.annotation.FractionRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.DialogFragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;

import com.example.rednykapcsol.FragmentCommunicating;
import com.example.rednykapcsol.R;
import com.example.rednykapcsol.Timing;
import com.example.rednykapcsol.WeekDay;

import org.joda.time.LocalTime;

import java.util.Map;
import java.util.Objects;

public class TimingSelectorFragment extends DialogFragment {

    private CustomAdapter customAdapter;
    private ImageButton closeButton;
    private ListView listView;
    //private TextView valueText;
    private FragmentCommunicating fragmentCommunicating;
    private final String configTag = "config";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        customAdapter = new CustomAdapter();
        fragmentCommunicating = (FragmentCommunicating) getActivity();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View timingListDialog = getLayoutInflater().inflate(R.layout.timing_selector, container, false);
        listView = timingListDialog.findViewById(R.id.timing_list);
        listView.setAdapter(customAdapter);

        closeButton = timingListDialog.findViewById(R.id.close_timing);
        closeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getDialog().dismiss();
            }
        });

        return timingListDialog;
    }

    @Override
    public void onDismiss(DialogInterface dialogInterface) {
        super.onDismiss(dialogInterface);

        Log.i("DEBUG","Dismissing!");

        Timing.updateTimings(getActivity());
    }

    @Override
    public void onResume() {
        super.onResume();
        getDialog().getWindow().setLayout(700, 1100);
    }

    public class CustomAdapter extends BaseAdapter {

        @Override
        public int getCount() {
            return Timing.getTimings().length;
        }

        @Override
        public Object getItem(int position) {
            return Timing.getTimings()[position];
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(final int position, View convertView, ViewGroup parent) {
            View view = getLayoutInflater().inflate(R.layout.timing_row,null);

            ConstraintLayout details = (ConstraintLayout) view.findViewById(R.id.details);
            details.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    TimingConfigFragment timingConfigFragment = new TimingConfigFragment(position);
                    timingConfigFragment.show(getActivity().getSupportFragmentManager(), configTag);
                }
            });

            Timing timing = Timing.getTimings()[position];

            Log.i("DEBUG", "Timing - "+position +": "+timing);

            updateListViewElement(view, timing);

            Switch timingSwitch = view.findViewById(R.id.timing_switch);
            timingSwitch.setChecked(timing.isActive());
            timingSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    Timing timing = Timing.getTimings()[position];

                    if(timing.getDays() == null && isChecked) {
                        Log.i("DEBUG", "time: "+timing.getTime()+", days: "+timing.getDays());

                        TimingConfigFragment timingConfigFragment = new TimingConfigFragment(position);
                        timingConfigFragment.show(getActivity().getSupportFragmentManager(), timingConfigFragment.getTag());
                    }
                    else {
                        timing.setActive(isChecked);
                    }
                }
            });
            return view;
        }
    }

    public void saveSettings(int position, Timing timing) {
        View view = listView.getChildAt(position - listView.getFirstVisiblePosition());

        if (Timing.getTimings()[position].getDays() == null) {
            timing.setActive(true);
            Log.i("DEBUG", "Prev false, now true!");
        }
        else {
            timing.setActive(Timing.getTimings()[position].isActive());
        }

        Timing.getTimings()[position] = timing;
        updateListViewElement(view, timing);
    }

    public void cancelSetup(int position) {
        Log.i("OMMIK", "Cancelling: "+position);
        Timing timing = Timing.getTimings()[position];

        if(timing.getDays() == null) {
            View view = listView.getChildAt(position - listView.getFirstVisiblePosition());
            Switch timingSwitch = (Switch) view.findViewById(R.id.timing_switch);
            timingSwitch.setChecked(false);
        }
    }

    private void updateListViewElement(View view, Timing timing) {
        TextView timeText = view.findViewById(R.id.time_text);
        TextView valueText = view.findViewById(R.id.value_text);
        if(timing.getTime() != null){
            timeText.setText(timing.getTime().toString("HH:mm"));
            valueText.setText(timing.getValue()+"%");
        }
        else {
            timeText.setText("ÓÓ:PP");
            valueText.setText("-");
        }

        TextView daysText = view.findViewById(R.id.days_text);
        daysText.setText(timing.generateActiveDaysString());

        Switch timingSwitch = (Switch) view.findViewById(R.id.timing_switch);
        timingSwitch.setChecked(timing.isActive());
    }
}

