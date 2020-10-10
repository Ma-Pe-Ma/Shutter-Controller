package com.example.rednykapcsol;

import android.util.Log;

import com.android.volley.AuthFailureError;
import com.android.volley.NetworkResponse;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.VolleyLog;
import com.android.volley.toolbox.HttpHeaderParser;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import android.os.Handler;

public class RequestDispatcher {

    private List<ActivityNotifier> activityNotifiers = new ArrayList<>();

    public void notifyNewValue(float newValue) {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifyNewValue(newValue);
        }
    }

    public void notifyMessage(int newMessageSize) {
        for(ActivityNotifier activityNotifier : activityNotifiers) {
            activityNotifier.notifyMessage(newMessageSize);
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

    final String URL = "http://...";

    public static RequestDispatcher requestDispatcher = null;

    private RequestQueue requestQueue = null;

    public static RequestDispatcher getRequestDispatcher() {
        if (requestDispatcher == null) {
            requestDispatcher = new RequestDispatcher();
            requestDispatcher.requestQueue = Volley.newRequestQueue(AppContext.getAppContext());
        }

        return requestDispatcher;
    }

    public void getDump() {
        notifyDisableGUI();
        String customURL = URL + "/STAT";

        StringRequest stringRequest = new StringRequest(Request.Method.GET, customURL,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        try {
                            JSONObject dumpObject = new JSONObject(response);
                            handleDumpResponse(dumpObject);
                        }
                        catch (JSONException e) {

                        }
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

    public void getState() {
        String customURL = URL + "/STAT";

        StringRequest stringRequest = new StringRequest(Request.Method.GET, customURL,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {

                        try {
                            JSONObject stateObject = new JSONObject(response);
                            handleGenericResponse(stateObject);
                        }
                        catch (JSONException e) {

                        }
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        //TODO: Handle this
                    }
        }) {

        };

        requestQueue.add(stringRequest);
    }

    public void postZeroState(ZeroState zeroState) {
        notifyDisableGUI();

        String customURL = URL + "/NUL";

        try {
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("STATE", zeroState.name());
            final String requestBody = jsonBody.toString();

            StringRequest stringRequest = new StringRequest(Request.Method.POST, customURL, new Response.Listener<String>() {
                @Override
                public void onResponse(String response) {
                    Log.i("VOLLEY", response);

                    try {
                        JSONObject zeroResponse = new JSONObject(response);
                        handleGenericResponse(zeroResponse);
                    }
                    catch (JSONException e) {

                    }
                }
            }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    Log.e("VOLLEY", error.toString());
                    notifyTimeout();
                    notifyEnableGUI();
                }
            }) {
                @Override
                public String getBodyContentType() {
                    return "application/json; charset=utf-8";
                }

                @Override
                public byte[] getBody() throws AuthFailureError {
                    try {
                        return requestBody == null ? null : requestBody.getBytes("utf-8");
                    } catch (UnsupportedEncodingException uee) {
                        VolleyLog.wtf("Unsupported Encoding while trying to get the bytes of %s using %s", requestBody, "utf-8");
                        return null;
                    }
                }

                @Override
                protected Response<String> parseNetworkResponse(NetworkResponse response) {
                    String responseString = "";
                    if (response != null) {
                        responseString = String.valueOf(response.statusCode);
                        // can get more details such as response.headers
                    }
                    return Response.success(responseString, HttpHeaderParser.parseCacheHeaders(response));
                }
            };

            requestQueue.add(stringRequest);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void postNewValue(float valueInt) {
        notifyDisableGUI();
        String customURL = URL + "/SET";
        float value = ((float) valueInt) / 100;

        try {
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("VALUE", value);
            final String requestBody = jsonBody.toString();

            StringRequest stringRequest = new StringRequest(Request.Method.POST, customURL, new Response.Listener<String>() {
                @Override
                public void onResponse(String response) {
                    Log.i("VOLLEY", response);
                    try {
                        JSONObject newValueResponse = new JSONObject(response);
                        handleGenericResponse(newValueResponse);
                    }
                    catch (JSONException e) {

                    }
                }
            }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    Log.e("VOLLEY", error.toString());
                    notifyTimeout();
                    notifyEnableGUI();
                }
            }) {
                @Override
                public String getBodyContentType() {
                    return "application/json; charset=utf-8";
                }

                @Override
                public byte[] getBody() throws AuthFailureError {
                    try {
                        return requestBody == null ? null : requestBody.getBytes("utf-8");
                    } catch (UnsupportedEncodingException uee) {
                        VolleyLog.wtf("Unsupported Encoding while trying to get the bytes of %s using %s", requestBody, "utf-8");
                        return null;
                    }
                }

                @Override
                protected Response<String> parseNetworkResponse(NetworkResponse response) {
                    String responseString = "";
                    if (response != null) {
                        responseString = String.valueOf(response.statusCode);
                        // can get more details such as response.headers
                    }
                    return Response.success(responseString, HttpHeaderParser.parseCacheHeaders(response));
                }
            };

            requestQueue.add(stringRequest);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void postTimings() {
        notifyDisableGUI();

        String customURL = URL + "/SCH";

        JSONObject jsonBody =  Timing.serializeTimingObjectDump();

        final String requestBody = jsonBody.toString();

        StringRequest stringRequest = new StringRequest(Request.Method.POST, customURL, new Response.Listener<String>() {
            @Override
            public void onResponse(String response) {
                Log.i("VOLLEY", response);

                try {
                    JSONObject timingResponse = new JSONObject(response);
                    handleGenericResponse(timingResponse);
                }
                catch (JSONException e) {
                    //TODO: Handle bad message!
                }
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.e("VOLLEY", error.toString());
                notifyTimeout();
                notifyEnableGUI();
            }
        }) {
            @Override
            public String getBodyContentType() {
                return "application/json; charset=utf-8";
            }

            @Override
            public byte[] getBody() throws AuthFailureError {
                try {
                    return requestBody == null ? null : requestBody.getBytes("utf-8");
                } catch (UnsupportedEncodingException uee) {
                    VolleyLog.wtf("Unsupported Encoding while trying to get the bytes of %s using %s", requestBody, "utf-8");
                    return null;
                }
            }

            @Override
            protected Response<String> parseNetworkResponse(NetworkResponse response) {
                String responseString = "";
                if (response != null) {
                    responseString = String.valueOf(response.statusCode);
                    // can get more details such as response.headers
                }
                return Response.success(responseString, HttpHeaderParser.parseCacheHeaders(response));
            }
        };

        requestQueue.add(stringRequest);
    }

    public void handleGenericResponse(JSONObject responseObject) {
        try {
            String status = (String) responseObject.get("TYPE");

            float time = (float) responseObject.get("TIME") + 1;

            JSONObject newMessagesObject = (JSONObject) responseObject.get("MES");
            List<String> newMessagesList = new ArrayList<>();

            for (int i = 0; i < newMessagesObject.length(); i++) {
                String ID = "" + (i + 1);

                String newMessage = (String) newMessagesObject.get(ID);
                newMessagesList.add(newMessage);
            }

            MessageContainer.getMessageContainer().addNewMessages(newMessagesList);

            switch (status) {
                case "OK":
                    notifyEnableGUI();
                    break;

                case "WAIT":
                    notifyDisableGUI();

                    //TODO: CHECK THIS
                    Runnable newRunnable = new Runnable() {
                        @Override
                        public void run() {
                            getState();
                        }
                    };

                    stateHandler.postDelayed(newRunnable, (int) (time * 1000));

                    break;
            }

           if (newMessagesList.size() > 0) {
                notifyMessage(newMessagesList.size());
           }

           float value = (float) responseObject.get("VALUE");
           notifyNewValue(value);

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void handleDumpResponse(JSONObject dumpObject) {
        try {
            boolean blackout = (Boolean) dumpObject.get("BO");
            String time = (String) dumpObject.get("START");
            MessageContainer.getMessageContainer().setStartupTime(time);

            //Getting or Setting timing objects
            if (blackout) {
                postTimings();
            }
            else {
                JSONObject timingObject = (JSONObject) dumpObject.get("TIM");
                Timing.parseTimingObject(timingObject);
            }

            //Getting messages
            JSONObject messages = (JSONObject) dumpObject.get("MES");

            List<String> newMessagesList = new ArrayList<>();

            for (int i = 0; i < messages.length(); i++) {
                String ID = "" + (i + 1);

                String newMessage = (String) messages.get(ID);
                newMessagesList.add(newMessage);
            }

            MessageContainer.getMessageContainer().addNewMessages(newMessagesList);

            int newMessageCount = (int) dumpObject.get("MC");

            if (newMessageCount > 0) {
                notifyMessage(newMessageCount);
            }

            float value = (float) dumpObject.get("VALUE");
            notifyNewValue(value);

            startCyclicUpdate();

            notifyEnableGUI();

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private Handler stateHandler = new Handler();
    private Runnable stateChecker =  new Runnable () {
        @Override
        public void run() {
            getState();
            stateHandler.postDelayed(this, 30000);
        }
    };

    private void startCyclicUpdate() {
        stateHandler.postDelayed(stateChecker, 30000);
    }

    public void stopCyclicUpdate() {
        stateHandler.removeCallbacks(stateChecker);
    }
}
