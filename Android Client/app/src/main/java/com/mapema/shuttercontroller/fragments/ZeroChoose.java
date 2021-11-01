package com.mapema.shuttercontroller.fragments;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;

import androidx.fragment.app.DialogFragment;

import com.mapema.shuttercontroller.R;
import com.mapema.shuttercontroller.RequestDispatcher;
import com.mapema.shuttercontroller.ZeroState;

public class ZeroChoose extends DialogFragment {
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {

        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        builder.setTitle(getString(R.string.nullQuestion))
                .setPositiveButton(getString(R.string.up), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        RequestDispatcher.getRequestDispatcher().postZeroState(ZeroState.up);
                        dismiss();
                    }
                })
                .setNegativeButton(getString(R.string.down), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        RequestDispatcher.getRequestDispatcher().postZeroState(ZeroState.down);
                        dismiss();
                    }
                })
                .setNeutralButton(getString(R.string.current), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        RequestDispatcher.getRequestDispatcher().postZeroState(ZeroState.find);
                        dismiss();
                    }
                });

        return builder.create();
    }

    @Override
    public void onCancel(DialogInterface dialogInterface) {
        super.onCancel(dialogInterface);
    }
}
