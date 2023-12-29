import { RequestQueue, TestRequestQueue } from './requestqueue.js';

var testMode = document.getElementById("control").src.split("testMode=")[1] == "true";

const NR_OF_TIMINGS = 6;
const NR_OF_MESSAGES = 10;

var requestQueue;
var guiElements;

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
        var defualtTimings = {
            "0" : {"H" : null, "M" : null, "D" : "FFFFFFF", "A" : false, "V" : 0},
            "1" : {"H" : null, "M" : null, "D" : "FFFFFFF", "A" : false, "V" : 0},
            "2" : {"H" : null, "M" : null, "D" : "FFFFFFF", "A" : false, "V" : 0},
            "3" : {"H" : null, "M" : null, "D" : "FFFFFFF", "A" : false, "V" : 0},
            "4" : {"H" : null, "M" : null, "D" : "FFFFFFF", "A" : false, "V" : 0},
            "5" : {"H" : null, "M" : null, "D" : "FFFFFFF", "A" : false, "V" : 0}
        };

        this.timings = document.getElementById("timings");
        this.timingTemplate = document.getElementById("timingTemplate");
        this.checkboxTemplate = document.getElementById("checkboxTemplate");

        this.parseTimings(defualtTimings);
    }

    serializeTimings() {
        var timingsObject = {};

        var index = 0;
        for (var timing of timings.children) {
            var times = timing.querySelector("#timeSet").value.split(":");
            if (times[0] == null || times[1] == null) {
                return null;
            }

            var days = "";
            for (var day of timing.querySelectorAll("#dayCheck")) {
                days += day.checked ? "T" : "F";
            }

            timingsObject[String(index++)] = {
                "H" : parseInt(times[0]),
                "M" : parseInt(times[1]),
                "D" : days,
                "A" : timing.querySelector("#activeCheck").checked,
                "V" : parseInt(timing.querySelector("#seekbar").value)
            };
        }
    
        return timingsObject;
    }

    parseResponse(responseObject) {
        var newRequest = null;
        
        if (Object.keys(responseObject).length == 0) {
            this.stateMessage.innerText = "Invalid response from the server!";
        }
        else {
            var isDump = responseObject.hasOwnProperty("T");

            if (isDump)
            {
                var timingsObject = responseObject["T"];
                this.parseTimings(timingsObject)
            }            

            var genericResponse = responseObject["G"];
            var retrytime = genericResponse["R"];
            retrytime = parseInt(retrytime);

            if (retrytime > 0)
            {
                newRequest = new CustomRequest();
                newRequest.setLocation("/S");
                newRequest.setDelay(retrytime);   
            }
            else
            {
               this.parseGenericResponse(genericResponse);
            }
        } 
        return newRequest;      
    };

    parseTimings(timingsObject) {
        this.disableables = []
        timings.innerHTML = "";

        function formatTimeValue(value) {
            var valueString = String(value);
            return valueString.length == 1 ? "0" + valueString : valueString;
        }

        var j = 0;
        for (var timing of Object.values(timingsObject)) {
            var timingElement = this.timingTemplate.content.cloneNode(true);

            timingElement.getElementById("timingId").innerText = `${j++ + 1}.`;
            
            timingElement.getElementById("timeSet").value = formatTimeValue(timing["H"]) + ":" + formatTimeValue(timing["M"]);
            this.disableables.push(timingElement.getElementById("timeSet"));

            timingElement.getElementById("activeCheck").checked = timing["A"];
            this.disableables.push(timingElement.getElementById("activeCheck"));

            var inputText = timingElement.getElementById("seekbarValueText");
            inputText.innerText = `${timing["V"]}%`;

            var input = timingElement.getElementById("seekbar");
            input.textHolder = inputText;
            input.value = timing["V"];
            input.oninput = function() {
                this.textHolder.innerText = `${this.value}%`;
            }
            this.disableables.push(input);

            var daySelector = timingElement.getElementById("daySelector");            

            var i = 0;
            for (const dayID of this.dayAbbreviationMap) {
                var dayCheckBox = this.checkboxTemplate.content.cloneNode(true);
                dayCheckBox.getElementById("dayLabel").innerText = dayID;
                dayCheckBox.getElementById("dayCheck").checked = timing["D"][i++] == "T";
                this.disableables.push(dayCheckBox.getElementById("dayCheck"));
                daySelector.appendChild(dayCheckBox);
            }

            timings.appendChild(timingElement);
        }
    }

    parseGenericResponse(genericResponse) {
        this.seekbar.value = parseInt(genericResponse["V"]);
        this.seekbarValueText.innerText = this.seekbar.value + "%"; 

        this.currentValue.innerText = this.seekbar.value + "%";

        var messagesObject = genericResponse["M"];
        
        this.startupTimeText.innerText = messagesObject["S"];
    
        messages.innerHTML = "";
        for (const message of Object.entries(messagesObject["M"])) {
            let[key, messageObject] = message;
            var keyInt = parseInt(key);

            var messageElement = messageTemplate.content.cloneNode(true);
            messageElement.getElementById("id").innerText =`${keyInt+1}`;
            messageElement.getElementById("event").innerText = this.getFormattedMessage(messageObject["T"], messageObject["R"], messageObject["A"]);
            messageElement.getElementById("date").innerText = messageObject["D"];
            messages.appendChild(messageElement);
        }
    }

    getFormattedMessage(T, R, A) {
        if (T == "E") {
            return "-";
        }
    
        if (T == "I") {
            return "Server start.";
        }
    
        if (T == "J") {
            return "JSON Error: " + R + ", " + A;
        }
    
        if (T == "Z") {
            if (R == "F") {
                return "Zeroing failed.";
            }
    
            if (R == "O") {
                if (A == "U") {
                    return "Zeroing: up.";
                }
                else if (A == "D") {
                    return "Zeroing: down.";
                }
            }
        }
    
        if (T == "T") {
            return "" + R + ". timing set: " + A + "%.";
        }
    
        if (T == "S") {
            if (R == "M") {
                return "Manual set: " + A + "%";
            }
    
            if (R == "Z") {
                return "Position found: " + A + "%";
            }
    
            if (R == "T") {
                return "Timings updated.";
            }
        }
    
        return "Unknown event: " + T + ", " + R + ", " + A;
    }

    bindGUIElements()
    {
        function sendZero(state)
        {
            var request = new CustomRequest();
            request.setPostData({"Z" : state});
            request.setLocation("/Z");
            requestQueue.addNewRequest(request)
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
            request.setPostData({"V" : document.getElementById("seekbar").value});
			request.setLocation("/V");
            requestQueue.addNewRequest(request);
        };
        this.upButton = document.getElementById("upButton");
        this.upButton.onclick = function() {
            sendZero("up");
        };

        this.downButton = document.getElementById("downButton");
        this.downButton.onclick = function() {
            sendZero("down");
        };       
        this.autoButton = document.getElementById("autoButton");
        this.autoButton.onclick = function() {
            sendZero("find");
        };
    }
}

class CustomRequest {
    #postData = null
    #location = ""
    #delay = 0

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
        request.onload = function() {
            if (this.response == null || this.response == "") {
                guiElements.stateMessage.innerText = "Error: bad response!";
            }

            try {
                var newRequest = guiElements.parseResponse(JSON.parse(this.response));
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
                guiElements.stateMessage.innerText = "Error while parsing json...";
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
            request.body = JSON.stringify(this.#postData);
            request.open("POST", this.#location);
            request.send(request.body);
        }  
    }
}

function tryDequeue() {
    requestQueue.dequeue()
    window.requestAnimationFrame(tryDequeue);
}

window.onload = function() {
    guiElements = new GuiElements();
    requestQueue = testMode ? new TestRequestQueue(guiElements) : new RequestQueue(guiElements);

    guiElements.bindGUIElements();
    guiElements.initMessages();
    guiElements.initTimings();
    guiElements.setGuiState(false);
    
    // launch dump request
    var dumpRequest = new CustomRequest();
    dumpRequest.setLocation("/D");
    requestQueue.addNewRequest(dumpRequest);

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
};