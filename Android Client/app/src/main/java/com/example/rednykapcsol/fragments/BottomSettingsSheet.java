package com.example.rednykapcsol.fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.example.rednykapcsol.R;
import com.google.android.material.bottomsheet.BottomSheetDialogFragment;

public class BottomSettingsSheet extends BottomSheetDialogFragment {

    CheckBox zeroNight;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.bottom_settings_sheet, container, false);

        zeroNight = v.findViewById(R.id.nightZeroCheckBox);

        zeroNight.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

            }
        });


        return v;
    }

}
