package com.example.rednykapcsol.fragments;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.rednykapcsol.MessageAdapter;
import com.example.rednykapcsol.MessageContainer;
import com.example.rednykapcsol.R;

import java.util.List;

public class InfoDialogFragment extends DialogFragment {

    RecyclerView recyclerView;
    MessageAdapter messageAdapter;

    public InfoDialogFragment(List<String> filteredMessages) {
        messageAdapter = new MessageAdapter(filteredMessages);
    }
    @Override
    public void onResume() {
        super.onResume();
        getDialog().getWindow().setLayout(700, 1100);
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View infoView = inflater.inflate(R.layout.info_dialog, container, false);

        recyclerView = infoView.findViewById(R.id.infoRecycler);

        Log.i("DEBUG", "addr: "+recyclerView);

        recyclerView.setLayoutManager(new LinearLayoutManager(getActivity()));
        recyclerView.setAdapter(messageAdapter);

        messageAdapter.notifyDataSetChanged();

        return infoView;
    }
}
