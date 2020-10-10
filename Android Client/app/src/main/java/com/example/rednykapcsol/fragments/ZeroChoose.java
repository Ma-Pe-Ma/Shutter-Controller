package com.example.rednykapcsol.fragments;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;

import androidx.fragment.app.DialogFragment;

import com.example.rednykapcsol.RequestDispatcher;
import com.example.rednykapcsol.ZeroState;

public class ZeroChoose extends DialogFragment {
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {

        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        builder.setTitle("Hol található a redőny?")
                .setPositiveButton("Fent", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        RequestDispatcher.getRequestDispatcher().postZeroState(ZeroState.up);
                        dismiss();
                    }
                })
                .setNegativeButton("Lent", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        RequestDispatcher.getRequestDispatcher().postZeroState(ZeroState.down);
                        dismiss();
                    }
                })
                .setNeutralButton("Nem tudom", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        ZeroAutomatic zeroAutomatic = new ZeroAutomatic();
                        zeroAutomatic.show(getParentFragmentManager(), "ZEROAU");
                    }
                });

        return builder.create();
    }

    @Override
    public void onCancel(DialogInterface dialogInterface) {
        super.onCancel(dialogInterface);
        ZeroAutomatic zeroAutomatic = new ZeroAutomatic();
        zeroAutomatic.show(getParentFragmentManager(), "ZEROAU");
    }
}
