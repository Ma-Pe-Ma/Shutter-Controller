import { RequestQueue, TestRequestQueue } from './requestqueue.js';

var testMode = document.getElementById("control").src.split("testMode=")[1] == "true";

const NR_OF_TIMINGS = 6;
const NR_OF_MESSAGES = 10;

var requestQueue;
var guiElements;

var shResponse;
var shRequest;
var shZero;
var shTiming;
var shTimingContainer;

class GuiElements {
    constructor() {
        this.dayAbbreviationMap = ["M", "Tu", "W", "Th", "F", "Sa", "Su"];
        this.timingButton = null;
        this.setButton = null;
        
        this.upButton = null;
        this.downButton = null;
        this.autoButton = null;
    
        this.seekbar = null;
        this.seekbarValueText = null;
    
        this.stateMessage = null;
        this.messages = null;
        this.messageTemplate = null;
        this.startupTimeText = null;
    
        this.currentValue = null;
        this.timings = null;
        this.timingTemplate = null;
        this.checkboxTemplate = null;
        this.timingSeekbars = [];

        this.disableables = [];
    }  

    setGuiState(guiState) {
        for (var i = 0; i < this.disableables.length; i++)
        {
            this.disableables[i].disabled = !guiState;
        }
    
        timingButton.disabled = !guiState;
        setButton.disabled = !guiState;
        upButton.disabled = !guiState;
        downButton.disabled = !guiState;
        autoButton.disabled = !guiState;
    
        seekbar.disabled = !guiState;
    }

    initMessages() {
        messages = document.getElementById("messages");
        messageTemplate = document.getElementById("messageTemplate");

        for (var i = 0; i < NR_OF_MESSAGES; i++) {
            var messageElement = messageTemplate.content.cloneNode(true);
            messageElement.getElementById("id").innerText =`${i+1}`;
            messageElement.getElementById("event").innerText = `-`;
            messageElement.getElementById("date").innerText = `-`;
            messages.appendChild(messageElement);
        }
    }
    
    initTimings() {
        var timingContainer = shTimingContainer.create({
            "timing" : []
        });

        for (var i = 0; i < NR_OF_TIMINGS; i++) {
            timingContainer["timing"].push(shTiming.create({"hour" : null, "minute" : null, "days" : [false, false, false, false, false, false, false], "active" : true}));
        }

        this.timings = document.getElementById("timings");
        this.timingTemplate = document.getElementById("timingTemplate");
        this.checkboxTemplate = document.getElementById("checkboxTemplate");

        this.parseTimings(shTimingContainer.encode(timingContainer).finish());
    }

    serializeTimings() {
        var timingRequest = shRequest.create({
            "timing" : []
        });

        for (var timing of timings.children) {
            var times = timing.querySelector("#timeSet").value.split(":");
            if (times[0] == null || times[1] == null) {
                return null;
            }

            var days = [];
            for (var day of timing.querySelectorAll("#dayCheck")) {
                days.push(day.checked);
            }

            timingRequest["timing"].push(
                shTiming.create({
                    "hour" : parseInt(times[0]),
                    "minute" : parseInt(times[1]),
                    "days" : days,
                    "active" : timing.querySelector("#activeCheck").checked,
                    "value" : parseInt(timing.querySelector("#seekbar").value)
                })
            );            
        }

        return shRequest.encode(timingRequest).finish();
    }

    parseTimings(byteArray) {
        this.disableables = []
        timings.innerHTML = "";

        function formatTimeValue(value) {
            var valueString = String(value);
            return valueString.length == 1 ? "0" + valueString : valueString;
        }

        var timingContainer = shTimingContainer.decode(byteArray);

        for (var i = 0; i < timingContainer.timing.length; i++) {
            var timing = timingContainer.timing[i];
            var timingElement = this.timingTemplate.content.cloneNode(true);

            timingElement.getElementById("timingId").innerText = `${i + 1}.`;
            
            timingElement.getElementById("timeSet").value = formatTimeValue(timing["hour"]) + ":" + formatTimeValue(timing["minute"]);
            this.disableables.push(timingElement.getElementById("timeSet"));

            timingElement.getElementById("activeCheck").checked = timing["active"];
            this.disableables.push(timingElement.getElementById("activeCheck"));

            var inputText = timingElement.getElementById("seekbarValueText");
            inputText.innerText = `${timing["value"]}%`;

            var input = timingElement.getElementById("seekbar");
            input.textHolder = inputText;
            input.value = timing["value"];
            input.oninput = function() {
                this.textHolder.innerText = `${this.value}%`;
            }
            this.disableables.push(input);

            var daySelector = timingElement.getElementById("daySelector");            

            var j = 0;
            for (const dayID of this.dayAbbreviationMap) {
                var dayCheckBox = this.checkboxTemplate.content.cloneNode(true);
                dayCheckBox.getElementById("dayLabel").innerText = dayID;
                dayCheckBox.getElementById("dayCheck").checked = timing["days"][j++];
                this.disableables.push(dayCheckBox.getElementById("dayCheck"));
                daySelector.appendChild(dayCheckBox);
            }

            timings.appendChild(timingElement);
        }
    }

    parseGenericResponse(byteArray) {
        var genericResponse = shResponse.decode(byteArray);

        if (genericResponse["retryTime"] > 0) {
            var newRequest = new CustomRequest();
            newRequest.setLocation("/S");
            newRequest.setDelay(genericResponse["retryTime"]);
            return newRequest;
        }

        this.seekbar.value = genericResponse["value"];
        this.seekbarValueText.innerText = this.seekbar.value + "%"; 

        this.currentValue.innerText = this.seekbar.value + "%";

        var messageContainer = genericResponse["messageContainer"];
        
        this.startupTimeText.innerText = messageContainer["startTime"];

        messages.innerHTML = "";

        for (var i = 0; i < messageContainer.genericMessage.length; i++) {
            var messageElement = messageTemplate.content.cloneNode(true);
            messageElement.getElementById("id").innerText =`${i+1}`;
            messageElement.getElementById("event").innerText = this.getFormattedMessage(messageContainer.genericMessage[i]);
            messageElement.getElementById("date").innerText = messageContainer.genericMessage[i].datetime;
            messages.appendChild(messageElement);
        }

        return null;
    }

    messageMap = {
        0 : "-",
        1 : "Server start.",
        2 : "Manual set: %1$s%%",
        3 : "Timing set: %1$s%%.",
        4 : "Zeroing: %1$s%%",
        5 : "Zeroing failed.",
        6 : "Position found: %1$s%%",
        7 : "Timings updated."
    }

    getFormattedMessage(genericMessage) {
        var formattableString = this.messageMap[genericMessage["event"]];
        return sprintf(formattableString, genericMessage["value"]);
    }

    bindGUIElements()
    {
        function sendZero(state)
        {
            var request = new CustomRequest();
            
            var zeroRequest = shRequest.create({
                "zero" : state
            });

            request.setPostData(shRequest.encode(zeroRequest).finish());
            request.setLocation("/Z");
            requestQueue.addNewRequest(request);
        }

        this.currentValue = document.getElementById("curVal");
        this.startupTimeText = document.getElementById("startup");
        this.stateMessage = document.getElementById("stat");

        this.seekbarValueText = document.getElementById("seekbarValueText");

        this.seekbar = document.getElementById("seekbar");
        this.seekbar.oninput = function() {
            document.getElementById("seekbarValueText").innerText = seekbar.value + "%";
        };

        this.timingButton = document.getElementById("timingButton");
        this.timingButton.onclick = function() {
            var timingsObject = guiElements.serializeTimings();

            if (timingsObject == null) {
                alert("Invalid time input for a timing!");
                return;
            }

            var request = new CustomRequest();
            request.setLocation("/T");
            request.setPostData(timingsObject);
            requestQueue.addNewRequest(request);
        };
        this.setButton = document.getElementById("setButton");
        this.setButton.onclick = function() {
            var request = new CustomRequest();

            var valueRequest = shRequest.create({
                "value" : document.getElementById("seekbar").value
            });

            request.setPostData(shRequest.encode(valueRequest).finish());
			request.setLocation("/V");
            requestQueue.addNewRequest(request);
        };
        this.upButton = document.getElementById("upButton");
        this.upButton.onclick = function() {
            sendZero(shZero.values.up);
        };

        this.downButton = document.getElementById("downButton");
        this.downButton.onclick = function() {
            sendZero(shZero.values.down);
        };       
        this.autoButton = document.getElementById("autoButton");
        this.autoButton.onclick = function() {
            sendZero(shZero.values.current);
        };
    }
}

class CustomRequest {
    #postData = null;
    #location = "";
    #delay = 0;
    parseResponse = (response) => {return guiElements.parseGenericResponse(response)};

    setPostData(postData) {
        this.#postData = postData;
    }

    getPostData() {
        return this.#postData;
    }

    setLocation(location) {
        this.#location = location;
    }

    getLocation() {
        return this.#location;
    }

    setDelay(delay) {
        this.#delay = delay;
    }

    getDelay() {
        return this.#delay;
    }

    launchRequest() {
        var request = new XMLHttpRequest();
        request.container = this;
        request.responseType = "arraybuffer";
        request.onload = function() {
            if (this.response == null || this.response == "") {
                guiElements.stateMessage.innerText = "Error: bad response!";
            }

            try {
                const uint8Array = new Uint8Array(this.response);

                var newRequest = this.container.parseResponse(uint8Array);

                if (newRequest != null) {
                    requestQueue.addNewRequest(newRequest);
                    guiElements.setGuiState(false);
                }
                else {
                    guiElements.setGuiState(true);
                    guiElements.stateMessage.innerText = "The server is up and running!";
                }
            }
            catch(e) {
                guiElements.stateMessage.innerText = "Error decoding message...";
                window.location.href = "/";
            }

            requestQueue.currentRequestFinished();
        }        

        request.ontimeout = function() {
            guiElements.stateMessage.innerText = "The server is not available.";
            guiElements.setGuiState(true);
            requestQueue.currentRequestFinished();
        }

        request.onerror = function() {
            guiElements.stateMessage.innerText = "Error while communicating with the server: " + request.status + ", " + request.statusText;
            guiElements.setGuiState(true);
            requestQueue.currentRequestFinished();
        }

        request.onreadystatechange = function() {
            if (this.readyState !== 4) {
                return;
            }

            if (this.status == "302") {
                window.location.href = this.getResponseHeader("Location");
            }
        }

        if (this.#postData == null) {
            request.open("GET", this.#location);
            request.send();
        }
        else {
            request.open("POST", this.#location);
            request.send(this.#postData);
        }  
    }
}

function tryDequeue() {
    requestQueue.dequeue()
    window.requestAnimationFrame(tryDequeue);
}

window.onload = function() { 
    protobuf.load("/Shutter.proto", function(err, root) {        
        shResponse = root.lookupType("Shutter.Response");
        //shGenericMessage = root.lookupType("Shutter.GenericMessage");
        shRequest = root.lookupType("Shutter.Request");
        //shEvent = root.lookup("Shutter.Event");
        shZero = root.lookup("Shutter.Zero");    
        shTiming = root.lookupType("Shutter.Timing");
        //shMessageContainer = root.lookupType("Shutter.MessageContainer");
        shTimingContainer = root.lookupType("Shutter.TimingContainer");

        guiElements = new GuiElements();
        requestQueue = testMode ? new TestRequestQueue(guiElements) : new RequestQueue(guiElements);

        guiElements.bindGUIElements();
        guiElements.initMessages();
        guiElements.initTimings();
        guiElements.setGuiState(false);

        // launch dump request
        var dumpRequest = new CustomRequest();
        dumpRequest.setLocation("/D");
        dumpRequest.parseResponse = (response) => {guiElements.parseTimings(response)};
        requestQueue.addNewRequest(dumpRequest);

        var statusRequest = new CustomRequest();
        statusRequest.setLocation("/S");
        requestQueue.addNewRequest(statusRequest);

        //check and dequeue new request if it is available at everyFrame
        window.requestAnimationFrame(() => {tryDequeue();});

        //Set periodic status request
        var i = setInterval(function() {
            if (requestQueue.launchPeriodicStatusRequest()) {
                var statusRequest = new CustomRequest();
                statusRequest.setLocation("/S");
                requestQueue.addNewRequest(statusRequest);
            }
        }, 30 * 1000);
    });
};