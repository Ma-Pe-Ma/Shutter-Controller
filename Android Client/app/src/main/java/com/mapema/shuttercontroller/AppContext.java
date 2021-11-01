package com.mapema.shuttercontroller;

import android.app.Application;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.util.Log;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
import androidx.work.Constraints;
import androidx.work.ExistingPeriodicWorkPolicy;
import androidx.work.NetworkType;
import androidx.work.PeriodicWorkRequest;
import androidx.work.WorkManager;

import com.mapema.shuttercontroller.activities.MainActivity;

import static androidx.core.app.NotificationCompat.PRIORITY_HIGH;
import static java.util.concurrent.TimeUnit.MINUTES;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManagerFactory;

public class AppContext extends Application {

    private static Context context;

    public void onCreate() {
        super.onCreate();
        AppContext.context = getApplicationContext();
        addCustomCertificate();
        createNotificationChannel();
        setPeriodicRequestWork();
    }

    public static Context getAppContext() {
        return AppContext.context;
    }

    private static void addCustomCertificate() {
        // Load CAs from an InputStream
        // (could be from a resource or ByteArrayInputStream or ...)
        CertificateFactory cf = null;
        //try {
            try {
                cf = CertificateFactory.getInstance("X.509");
                Log.i("DEBUG", "AFTER CF");
            } /*catch (NoSuchProviderException e) {
                e.printStackTrace();
                Log.i("DEBUG", "EXC-1");
            }*/
        //}
        catch (CertificateException e) {
            Log.i("DEBUG", "EXC0: " + e.getMessage());
            e.printStackTrace();
        }

        InputStream caInput = new BufferedInputStream(context.getResources().openRawResource(R.raw.cert));
        Log.i("DEBUG", "CAINPUT: " + caInput +", cf: " + cf);

        Certificate ca = null;
                try {
                    try {
                        ca = cf.generateCertificate(caInput);
                        //Log.i("DEBUG", "CA: "+ca);
                        //System.out.println("ca=" + ((X509Certificate) ca).getSubjectDN());
                    } catch (CertificateException e) {
                        e.printStackTrace();
                        Log.i("DEBUG", "EXC1" + e.getMessage());
                    }
                } finally {
                    try {
                        caInput.close();
                    } catch (IOException e) {
                        Log.i("DEBUG", "EXC2");
                        e.printStackTrace();
                    }
                }

        // Create a KeyStore containing our trusted CAs
        String keyStoreType = KeyStore.getDefaultType();
        KeyStore keyStore = null;
        try {
            keyStore = KeyStore.getInstance(keyStoreType);
        } catch (KeyStoreException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC3");
        }
        try {
            keyStore.load(null, null);
        } catch (CertificateException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC4");
        } catch (IOException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC5");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC6");
        }
        try {
            keyStore.setCertificateEntry("ca", ca);
        } catch (KeyStoreException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC7");
        }

        // Create a TrustManager that trusts the CAs in our KeyStore
        String tmfAlgorithm = TrustManagerFactory.getDefaultAlgorithm();
        TrustManagerFactory tmf = null;
        try {
            tmf = TrustManagerFactory.getInstance(tmfAlgorithm);
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC8");
        }

        try {
            tmf.init(keyStore);
        } catch (KeyStoreException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC9");
        }

        // Create an SSLContext that uses our TrustManager
        SSLContext context = null;
        try {
            context = SSLContext.getInstance("TLS");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC10");
        }

        try {
            context.init(null, tmf.getTrustManagers(), null);
        } catch (KeyManagementException e) {
            e.printStackTrace();
            Log.i("DEBUG", "EXC11");
        }

        // Tell the URLConnection to use a SocketFactory from our SSLContext
        HttpsURLConnection.setDefaultSSLSocketFactory(context.getSocketFactory());

        HostnameVerifier hostnameVerifier = new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                HostnameVerifier hv =
                        HttpsURLConnection.getDefaultHostnameVerifier();
                return hv.verify("example.com", session);
            }
        };

        // Tell the URLConnection to use our HostnameVerifier
        //URL url = new URL("https://example.org/");
        //HttpsURLConnection urlConnection = (HttpsURLConnection)url.openConnection();

        //HttpsURLConnection.setDefaultHostnameVerifier(hostnameVerifier);

        Log.i("DEBUG", "CUST CERT ADDED! ");
    }

    private void createNotificationChannel() {
        // Create the NotificationChannel, but only on API 26+ because
        // the NotificationChannel class is new and not in the support library
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            CharSequence name = getString(R.string.notificationChannelName);
            String description = getString(R.string.notificationChannelDescription);
            String channelID = getString(R.string.notificationChannelID);
            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel channel = new NotificationChannel(channelID, name, importance);
            channel.setDescription(description);
            // Register the channel with the system; you can't change the importance
            // or other notification behaviors after this
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
    }

    static int notificationID = 0;

    final static int requestCode = 1001;

    public static void createNotification() {
        Intent intent = new Intent(context, MainActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, requestCode, intent, 0);

        MessageContainer messageContainer = MessageContainer.getMessageContainer();
        String lastSetting = messageContainer.getMessage(0).getEvent() + " - " + messageContainer.getMessage(0).getDate();

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(AppContext.getAppContext(), AppContext.getAppContext().getString(R.string.notificationChannelID))
                .setSmallIcon(R.drawable.shutter)
                .setContentTitle(context.getString(R.string.notificationChannelName))
                .setContentText(lastSetting)
                .setPriority(PRIORITY_HIGH)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);

        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);
        notificationManager.notify(notificationID++, notificationBuilder.build());
    }

    private void setPeriodicRequestWork() {
        Constraints constraints = new Constraints.Builder()
                .setRequiredNetworkType(NetworkType.CONNECTED)
                //.setRequiresDeviceIdle(true)
                .build();
        PeriodicWorkRequest periodicWorkRequest = new PeriodicWorkRequest.Builder(PeriodicShutterWork.class, 20, MINUTES, 5, MINUTES)
        //PeriodicWorkRequest periodicWorkRequest = new PeriodicWorkRequest.Builder(PeriodicShutterWork.class, 15, MINUTES)
                .setConstraints(constraints)
                .build();

        WorkManager.getInstance().enqueueUniquePeriodicWork(getString(R.string.uniquePeriodicRequestWorkName), ExistingPeriodicWorkPolicy.KEEP, periodicWorkRequest);

        /*WorkManager.getInstance().getWorkInfosForUniqueWorkLiveData(getString(R.string.uniquePeriodicRequestWorkName))
                .observeForever(new Observer<List<WorkInfo>>() {
                    @Override
                    public void onChanged(List<WorkInfo> workInfos) {
                        for (WorkInfo workInfo : workInfos) {
                            Log.i("DEBUG", "State: " + workInfo.getState());
                        }
                    }
                });*/
    }
}
