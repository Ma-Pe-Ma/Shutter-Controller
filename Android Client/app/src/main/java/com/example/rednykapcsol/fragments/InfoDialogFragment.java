package com.example.rednykapcsol.fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.RecyclerView;

import com.example.rednykapcsol.MessageAdapter;
import com.example.rednykapcsol.MessageContainer;
import com.example.rednykapcsol.R;

public class InfoDialogFragment extends DialogFragment {

    RecyclerView recyclerView;
    MessageAdapter messageAdapter;

    public InfoDialogFragment(String[] messages) {
        if (messages == null) {
            messages = MessageContainer.getMessageContainer().getMessages();
        }

        messageAdapter = new MessageAdapter(messages);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        //super.onCreateView(inflater, container, savedInstanceState);
        View infoView = inflater.inflate(R.layout.info_dialog, container, false);

        recyclerView = infoView.findViewById(R.id.infoRecycler);

        recyclerView.setAdapter(messageAdapter);

        return  infoView;
    }
}
