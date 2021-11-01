package com.mapema.shuttercontroller;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

public class MessageAdapter extends RecyclerView.Adapter<MessageAdapter.MessageHolder> {
    public MessageAdapter() {

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
        return new MessageHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull MessageHolder holder, int position) {
        Message message = MessageContainer.getMessageContainer().getMessage(position);

        holder.message.setText(message.getEvent());
        holder.dateText.setText(message.getDate());
        holder.idText.setText(String.valueOf(message.getID()));
    }

    @Override
    public int getItemCount() {
        return MessageContainer.getMessageContainer().numberOfMessages;
    }

    public static class MessageHolder extends RecyclerView.ViewHolder {
        public TextView message;
        public TextView idText;
        public TextView dateText;

        public MessageHolder(@NonNull View itemView) {
            super(itemView);
            message = itemView.findViewById(R.id.messageRowText);
            idText = itemView.findViewById(R.id.messageRowID);
            dateText = itemView.findViewById(R.id.messageRowDate);
        }
    }
}
