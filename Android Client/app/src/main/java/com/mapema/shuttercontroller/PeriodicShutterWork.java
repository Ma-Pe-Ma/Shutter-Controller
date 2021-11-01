package com.mapema.shuttercontroller;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.work.Worker;
import androidx.work.WorkerParameters;

public class PeriodicShutterWork extends Worker {

    public PeriodicShutterWork(Context context, WorkerParameters workerParameters) {
        super(context, workerParameters);

        Log.i("DEBUG", "Created PerShWor");
    }

    @NonNull
    @Override
    public Result doWork() {
        RequestDispatcher.getRequestDispatcher().getState(true);
        //AppContext.createNotification();
        Log.i("DEBUG", "DO WORK");
        return Result.success();
    }

    @Override
    public void onStopped() {
        super.onStopped();


    }

}
