package com.example.rednykapcsol;

import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;

public class MessageAdapter extends RecyclerView.Adapter<MessageAdapter.MessageHolder> {

    List<String> messages = new ArrayList<>();

    public MessageAdapter(String[] messages) {
        for (String message : messages) {
            if (message != null && message != "") {
                this.messages.add(message);
            }
        }
    }

    @NonNull
    @Override
    public MessageHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return null;
    }

    @Override
    public void onBindViewHolder(@NonNull MessageHolder holder, int position) {

    }

    @Override
    public int getItemCount() {
        return messages.size();
    }

    public static class MessageHolder extends RecyclerView.ViewHolder {

        public MessageHolder(@NonNull View itemView) {
            super(itemView);
        }
    }
}
