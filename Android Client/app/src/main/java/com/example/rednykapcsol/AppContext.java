package com.example.rednykapcsol;

import android.app.Application;
import android.content.Context;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;

import java.security.cert.X509Certificate;

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
}
