package com.example.rednykapcsol.fragments;

import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.DialogFragment;

import com.example.rednykapcsol.RequestDispatcher;
import com.example.rednykapcsol.ZeroState;
import com.example.rednykapcsol.activities.MainActivity;

public class ZeroAutomatic extends DialogFragment {
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {

        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Megkeressem automatikusan?");

        builder.setPositiveButton("Igen", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                //RequestDispatcher.getRequestDispatcher().postZeroState(ZeroState.find);
                dismiss();
            }
        });

        builder.setNegativeButton("Nem", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dismiss();
            }
        });

        return builder.create();
    }
}
