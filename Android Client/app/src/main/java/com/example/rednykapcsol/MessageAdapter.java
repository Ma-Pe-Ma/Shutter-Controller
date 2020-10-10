package com.example.rednykapcsol;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import org.w3c.dom.Text;

import java.util.ArrayList;
import java.util.List;

public class MessageAdapter extends RecyclerView.Adapter<MessageAdapter.MessageHolder> {

    List<String> messages;

    public MessageAdapter(List<String> messages) {
        this.messages = messages;
        Log.i("DEBUG", "Message s: "+messages.size());
    }

    @Override
    public int getItemViewType(int position) {
        return R.layout.message_row;
    }

    @NonNull
    @Override
    public MessageHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        View view = inflater.inflate(viewType, parent, false);

        Log.i("DEBUG", "inflating row");

        return new MessageHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull MessageHolder holder, int position) {
        holder.message.setText(messages.get(position));
    }

    @Override
    public int getItemCount() {
        Log.i("DEBUG", "Message s2: "+messages.size());
        return messages.size();
    }

    public static class MessageHolder extends RecyclerView.ViewHolder {

        public TextView message;

        public MessageHolder(@NonNull View itemView) {
            super(itemView);
            message = itemView.findViewById(R.id.messageRowText);
        }
    }
}
