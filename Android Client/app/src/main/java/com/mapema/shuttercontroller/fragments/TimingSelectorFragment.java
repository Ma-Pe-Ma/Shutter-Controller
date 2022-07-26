package com.mapema.shuttercontroller.fragments;

import android.content.DialogInterface;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageButton;

import android.view.LayoutInflater;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.DialogFragment;

import com.mapema.shuttercontroller.ActivityNotifier;
import com.mapema.shuttercontroller.FragmentNotifier;
import com.mapema.shuttercontroller.R;
import com.mapema.shuttercontroller.RequestDispatcher;
import com.mapema.shuttercontroller.Timing;
import com.mapema.shuttercontroller.activities.MainActivity;

import org.json.JSONObject;

public class TimingSelectorFragment extends DialogFragment implements FragmentNotifier {

    private CustomAdapter customAdapter;
    private Button closeButton;
    private ListView listView;
    //private TextView valueText;
    private ActivityNotifier activityNotifier;
    public static final String configTag = "config";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        customAdapter = new CustomAdapter();
        activityNotifier = (ActivityNotifier) getActivity();
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
                JSONObject timingsObject = Timing.generateTimingObjectDump();
                RequestDispatcher.getRequestDispatcher().postTimings(timingsObject);
            }
        });

        return timingListDialog;
    }

    @Override
    public void onDismiss(DialogInterface dialogInterface) {
        super.onDismiss(dialogInterface);
    }

    @Override
    public void onResume() {
        super.onResume();

        Timing.updateBuffer();

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getActivity().getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        int height = (int) (displayMetrics.heightPixels * 0.9f);
        int width = (int) (displayMetrics.widthPixels * 0.9f);

        ((MainActivity) getActivity()).subscribeToFragmentNotifications(this);
        getDialog().getWindow().setLayout(width, height);
    }

    @Override
    public void onPause() {
        super.onPause();
        ((MainActivity) getActivity()).unsubscribeToFragmentNotifications(this);
    }

    @Override
    public void onCancel(DialogInterface dialogInterface) {
        super.onCancel(dialogInterface);
        Timing.updateBuffer();
    }

    @Override
    public void notifySelectorAboutNewTiming(int position, Timing timing) {
        View view = listView.getChildAt(position - listView.getFirstVisiblePosition());

        boolean active = false;

        //check if modified object has active days
        for (Object value : timing.getDays().values()) {
            if ((Boolean) value) {
                active = true;
                break;
            }
        }

        if (Timing.getBufferTimings()[position].getTime() == null && active) {
            timing.setActive(true);
        }
        else {
            timing.setActive(Timing.getBufferTimings()[position].isActive());
        }

        Timing.getBufferTimings()[position] = timing;
        updateListViewElement(view, timing);
    }

    @Override
    public void notifySelectorAboutCanceling(int position) {
        cancelSetup(position);
    }

    public class CustomAdapter extends BaseAdapter {

        @Override
        public int getCount() {
            return Timing.getBufferTimings().length;
        }

        @Override
        public Object getItem(int position) {
            return Timing.getBufferTimings()[position];
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

            Timing timing = Timing.getBufferTimings()[position];

            updateListViewElement(view, timing);

            Switch timingSwitch = view.findViewById(R.id.timing_switch);
            timingSwitch.setChecked(timing.isActive());
            timingSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    Timing timing = Timing.getBufferTimings()[position];

                    boolean nonactive = true;

                    for (Object value : timing.getDays().values()) {
                        if ((Boolean) value) {
                            nonactive = false;
                            break;
                        }
                    }

                    if((timing.getTime() == null || nonactive) && isChecked) {
                        timing.setActive(true);
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

    public void cancelSetup(int position) {
        Timing timing = Timing.getBufferTimings()[position];

        if(timing.getDays() == null) {
            View view = listView.getChildAt(position - listView.getFirstVisiblePosition());
            Switch timingSwitch = (Switch) view.findViewById(R.id.timing_switch);
            timingSwitch.setChecked(false);
        }
    }

    private void updateListViewElement(View view, Timing timing) {
        TextView timeText = view.findViewById(R.id.time_text);
        TextView valueText = view.findViewById(R.id.value_text);
        if(timing.getTime() != null && timing.getTime().getMinuteOfHour() > -1 && timing.getTime().getHourOfDay() > -1){
            timeText.setText(timing.getTime().toString("HH:mm"));
            valueText.setText(getString(R.string.percentage, timing.getValue()));
        }
        else {
            timeText.setText(getString(R.string.unsetTime));
            valueText.setText(getString(R.string.hyphen));
        }

        TextView daysText = view.findViewById(R.id.days_text);
        daysText.setText(timing.generateActiveDaysString());

        Switch timingSwitch = (Switch) view.findViewById(R.id.timing_switch);
        timingSwitch.setChecked(timing.isActive());
    }
}