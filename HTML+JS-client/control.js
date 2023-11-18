import { RequestQueue, TestRequestQueue } from './requestqueue.js';

var testMode = document.getElementById("control").src.split("testMode=")[1] == "true";

const NR_OF_TIMINGS = 6;
const NR_OF_MESSAGES = 10;

var requestQueue;
var guiElements;

class GuiElements {
    timingButton = null;
    setButton = null;
    
    upButton = null;
    downButton = null;
    autoButton = null;

    seekbar = null;
    seekbarValueText = null;

    stateMessage = null;
    messages = null;
    startupTimeText = null;

    currentValue = null;
    timings = null;    
    timingSeekbars = [];

    disableables = [];

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

    initMessages() 
    {
        messages = document.getElementById("messages");
        for (var i = 0; i < NR_OF_MESSAGES; i++) {
            var tr = document.createElement('tr');
    
            const content = [`${i + 1}`, "-", "-"];
            const classNames = ['message-index-width', 'message-content-width', 'message-date-width'];

            for (var j = 0; j < 3; j++) {
                var div = document.createElement("td");
                //div.className = classNames[j]
                div.innerText = content[j];
                tr.appendChild(div);
            }
    
            messages.appendChild(tr);
        }
    }
    
    initTimings() {
        timings = document.getElementById("timings");
        
        const dayAbbreviationMap = ["M", "Tu", "W", "Th", "F", "Sa", "Su"]

        for (var i = 0; i < NR_OF_TIMINGS; i++) {
            var timingDiv = document.createElement('div');
            timingDiv.className = "timing";

            var p = document.createElement('p');
            p.innerText = `${i+1}.`;
            timingDiv.appendChild(p);

            var t = document.createElement('input');
            t.id = `timeSet${i}`;
            t.type = "time";
            timingDiv.appendChild(t);

            this.disableables.push(t);

            for (var j = 0; j < 7; j++) {
                var label = document.createElement('label');
                var checkBox = document.createElement('input');
                checkBox.id = `day${i}_${j}`;;
                checkBox.type = "checkbox";
                this.disableables.push(checkBox);

                label.appendChild(checkBox);
                label.appendChild(document.createTextNode(" " + dayAbbreviationMap[j]));

                timingDiv.appendChild(label);
            }

            var rangeContainer = document.createElement('div');
            rangeContainer.className = "range-container";

            var rangeLabel = document.createElement('label');
            rangeLabel.for = "range-input";
            rangeLabel.innerText = "Position:";
            rangeContainer.appendChild(rangeLabel);

            var rangeInput = document.createElement('input');
            rangeInput.id = `tseekbar${i}`;
            rangeInput.type = "range";
            rangeInput.step = 5;
            this.disableables.push(rangeInput);
            rangeContainer.appendChild(rangeInput);

            var span = document.createElement('span');
            span.className = "range-value";
            span.innerText = "50";
            span.id = `seekbarValue${i}`;
            rangeContainer.appendChild(span)
            
            rangeInput.customLabel = span;
            rangeInput.oninput = function() {
                this.customLabel.innerText = this.value;
            };

            timingDiv.append(rangeContainer);

            var activeLabel = document.createElement('label');
            var checkBox = document.createElement('input');
            checkBox.id = `toggle${i}`;
            checkBox.type = "checkbox";
            this.disableables.push(checkBox);

            activeLabel.appendChild(checkBox);
            activeLabel.appendChild(document.createTextNode(` Enabled`));

            timingDiv.appendChild(activeLabel);

            timings.appendChild(timingDiv);

            var breakline = document.createElement('br');
            timings.appendChild(breakline);
        }
    }

    serializeTimings() {
        var timingsObject = {};
    
        for(var i = 0; i < NR_OF_TIMINGS; i++) {
            var timingObject = {};
    
            var timeInput = document.getElementById(`timeSet${i}`).value;
            var times = timeInput.split(":");
    
            var days = "";
    
            for (var j = 0; j < 7; j++) {
                var dayBox = document.getElementById(`day${i}_${j}`);
    
                if (dayBox.checked) {
                    days += "T";
                }
                else {
                    days += "F";
                }
            }
    
            var activeBox = document.getElementById(`toggle${i}`);
            var seekbar = document.getElementById(`tseekbar${i}`);
    
            if (times[0] == null || times[1] == null) {
                return null;
            }
    
            timingObject["H"] = parseInt(times[0]);
            timingObject["M"] = parseInt(times[1]);
            timingObject["D"] = days;
            timingObject["A"] = activeBox.checked;
            timingObject["V"] = parseInt(seekbar.value);
    
            timingsObject[String(i)] =  timingObject;
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
        for (var key in timingsObject) {
            var timingObject = timingsObject[key];
            var keyInt = parseInt(key);
    
            var activeBox = document.getElementById(`toggle${keyInt}`);
            activeBox.checked = timingObject["A"];
    
            var timeSet = document.getElementById(`timeSet${keyInt}`);
    
            var hourString = String(timingObject["H"]);
            hourString = hourString.length == 1 ? "0" + hourString : hourString;
    
            var minuteString = String(timingObject["M"]);
            minuteString = minuteString.length == 1 ? "0" + minuteString : minuteString;
    
            timeSet.value = hourString + ":" + minuteString;
    
            var seekbar = document.getElementById(`tseekbar${keyInt}`);
            seekbar.value = timingObject["V"];
    
            var seekbarText = document.getElementById(`seekbarValue${keyInt}`);
            seekbarText.innerText = timingObject["V"];
    
            for(var i = 0; i < 7; i++) {
                var dayBox = document.getElementById(`day${keyInt}_${i}`);
    
                if (timingObject["D"][i] == "T") {
                    dayBox.checked = true;    
                }   
                else{
                    dayBox.checked = false;
                }
            }
        }
    }

    parseGenericResponse(genericResponse) {
        this.seekbar.value = parseInt(genericResponse["V"]);
        this.seekbarValueText.innerText = this.seekbar.value + "%"; 

        this.currentValue.innerText = this.seekbar.value + "%";

        var messagesObject = genericResponse["M"];
        
        this.startupTimeText.innerText = messagesObject["S"];
    
        for (var key in messagesObject["M"]) {
            var keyInt = parseInt(key);
            var messageObject = messagesObject["M"][key];
            var formattedMessage = this.getFormattedMessage(messageObject["T"], messageObject["R"], messageObject["A"]);
            var rootMessage = messages.childNodes[keyInt + 2];

            const content = [`${keyInt + 1}.`, `${formattedMessage}`, `${messageObject["D"]}`]

            for (var j = 0; j < rootMessage.children.length; j++) {
                rootMessage.children[j].innerText = content[j];
            }
        }
    }

    getFormattedMessage(T, R, A) {
        if (T == "E") {
            return "-";
        }
    
        if (T == "I") {
            return "Server startup.";
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
                return "Manual set: " + A + "%.";
            }
    
            if (R == "Z") {
                return "Position found: " + A + "%.";
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