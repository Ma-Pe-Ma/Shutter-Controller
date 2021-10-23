package com.example.rednykapcsol;

import android.content.Context;
import android.util.Log;

import com.android.volley.DefaultRetryPolicy;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import android.os.Handler;

import androidx.core.app.NotificationCompat;

import static androidx.core.app.NotificationCompat.PRIORITY_DEFAULT;
import static androidx.core.app.NotificationCompat.PRIORITY_HIGH;

public class RequestDispatcher {

    final String URL = BuildConfig.SERVER_URL;

    final private List<ActivityNotifier> activityNotifiers = new ArrayList<>();

    public void notifyNewValue(int newValue) {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifyNewValue(newValue);
        }
    }

    public void notifyMessage(int newMessageSize) {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifyMessage(newMessageSize);
        }
    }

    public void notifySynchronisation() {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifySynchronisation();
        }
    }

    public void notifyTimeout() {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifyTimeout();
        }
    }

    public void notifyEnableGUI() {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifyEnableGUI();
        }
    }

    public void notifyDisableGUI() {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifyDisableGUI();
        }
    }

    public void subscribe(ActivityNotifier activityNotifier) {
        if (!activityNotifiers.contains(activityNotifier)){
            activityNotifiers.add(activityNotifier);
        }
    }

    public void unSubscribe(ActivityNotifier activityNotifier) {
        activityNotifiers.remove(activityNotifier);
    }

    public static RequestDispatcher requestDispatcher = null;

    private RequestQueue requestQueue;

    public void createRequestQueue(Context context) {
        requestQueue = Volley.newRequestQueue(context);
    }

    public static RequestDispatcher getRequestDispatcher() {
        if (requestDispatcher == null) {
            requestDispatcher = new RequestDispatcher();
            requestDispatcher.requestQueue = Volley.newRequestQueue(AppContext.getAppContext());
        }

        return requestDispatcher;
    }

    public void getRoot() {
        String customURL = URL + "/";

        StringRequest stringRequest = new StringRequest(Request.Method.GET, customURL,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {

                    }
                }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                notifyTimeout();
            }
        }) {
        };

        requestQueue.add(stringRequest);
    }

    public void getDump() {
        notifyDisableGUI();
        notifySynchronisation();
        String customURL = URL + "/D";
        Log.i("DEBUG", "GET DUMP!");

        JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET, customURL, null,
                new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        handleDumpResponse(response);
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        notifyTimeout();
                    }
                }) {
        };

        jsonObjectRequest.setRetryPolicy(new DefaultRetryPolicy(
                DefaultRetryPolicy.DEFAULT_TIMEOUT_MS * 2,
                DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));

        requestQueue.add(jsonObjectRequest);
    }

    public void getState(boolean backgroundRequest) {
        String customURL = URL + "/S";

        notifySynchronisation();

        JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET, customURL, null,
                new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        if (backgroundRequest) {
                            handleBackgroundRequestResponse(response);
                        }
                        else {
                            handleGenericResponse(response);
                        }
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        //TODO: Handle this
                    }
        });

        jsonObjectRequest.setRetryPolicy(new DefaultRetryPolicy(
                DefaultRetryPolicy.DEFAULT_TIMEOUT_MS * 2,
                DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));

        requestQueue.add(jsonObjectRequest);
    }

    public void postZeroState(ZeroState zeroState) {
        notifyDisableGUI();
        String customURL = URL + "/Z";
        notifySynchronisation();

        try {
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("Z", zeroState.name());

            JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.POST, customURL, jsonBody, new Response.Listener<JSONObject>() {
                @Override
                public void onResponse(JSONObject response) {
                    handleGenericResponse(response);
                }
            }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    notifyTimeout();
                }
            });

            jsonObjectRequest.setRetryPolicy(new DefaultRetryPolicy(
                    DefaultRetryPolicy.DEFAULT_TIMEOUT_MS * 2,
                    DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                    DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));

            requestQueue.add(jsonObjectRequest);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void postNewValue(int valueInt) {
        notifyDisableGUI();
        String customURL = URL + "/V";
        notifySynchronisation();

        try {
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("V", valueInt);

            JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.POST, customURL, jsonBody, new Response.Listener<JSONObject>() {

                @Override
                public void onResponse(JSONObject response) {
                    handleGenericResponse(response);
                }
            }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    notifyTimeout();
                }
            });

            jsonObjectRequest.setRetryPolicy(new DefaultRetryPolicy(
                    DefaultRetryPolicy.DEFAULT_TIMEOUT_MS * 2,
                    DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                    DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));

            requestQueue.add(jsonObjectRequest);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void postTimings(JSONObject timingObject) {
        notifySynchronisation();
        notifyDisableGUI();
        String customURL = URL + "/T";

        JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.POST, customURL, timingObject, new Response.Listener<JSONObject>() {
            @Override
            public void onResponse(JSONObject response) {
                Timing.updateTimings();
                handleGenericResponse(response);
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                notifyTimeout();
            }
        });

        jsonObjectRequest.setRetryPolicy(new DefaultRetryPolicy(
                DefaultRetryPolicy.DEFAULT_TIMEOUT_MS * 2,
                DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));

        requestQueue.add(jsonObjectRequest);
    }

    private void handleBackgroundRequestResponse(JSONObject responseObject) {
        try {
            //time
            int time = (int) responseObject.get("T");

            if (time > 0) {
                stateHandler.postDelayed(notificationDelayer, (int) (time * 1000));
                return;
            }

            JSONObject newMessagesObject = (JSONObject) responseObject.get("M");

            //Messages
            JSONObject genericMessages = (JSONObject) newMessagesObject.get("M");
            MessageContainer.getMessageContainer().updateMessages(genericMessages);

            String startDate = (String) newMessagesObject.get("S");
            MessageContainer.getMessageContainer().setStartupTime(startDate);

            int count = (int) newMessagesObject.get("C");

            if (count > 0) {
                AppContext.createNotification();
            }
        }
        catch (JSONException e) {

        }
    }

    public void handleGenericResponse(JSONObject responseObject) {
        try {
            //time
            int time = (int) responseObject.get("T");

            if (time > 0) {
                notifyDisableGUI();
                stateHandler.postDelayed(shortStateChecker, (int) (time * 1000));
                return;
            }

            notifyEnableGUI();

            JSONObject newMessagesObject = (JSONObject) responseObject.get("M");

            //Messages
            JSONObject genericMessages = (JSONObject) newMessagesObject.get("M");
            MessageContainer.getMessageContainer().updateMessages(genericMessages);

            int count = (int) newMessagesObject.get("C");
            String startDate = (String) newMessagesObject.get("S");
            MessageContainer.getMessageContainer().setStartupTime(startDate);

            notifyMessage(MessageContainer.getMessageContainer().numberOfMessages);

            //value
            int value = responseObject.getInt("V");
            notifyNewValue(value);

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void handleDumpResponse(JSONObject dumpObject) {
        try {
            //Getting timings
            JSONObject timingObject = (JSONObject) dumpObject.get("T");
            Timing.parseTimingObjectDump(timingObject);

            //Getting generic response
            JSONObject messageObject = (JSONObject) dumpObject.get("G");
            handleGenericResponse(messageObject);

            startCyclicUpdate();

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private Handler stateHandler = new Handler();
    private Runnable longStateChecker =  new Runnable () {
        @Override
        public void run() {
            getState(false);
            stateHandler.postDelayed(this, 30000);
        }
    };

    private Runnable shortStateChecker =  new Runnable () {
        @Override
        public void run() {
            getState(false);
        }
    };

    private Runnable notificationDelayer =  new Runnable () {
        @Override
        public void run() {
            getState(true);
        }
    };

    private void startCyclicUpdate() {
        stateHandler.postDelayed(longStateChecker, 30000);
    }

    public void stopCyclicUpdate() {
        stateHandler.removeCallbacks(longStateChecker);
    }
}
