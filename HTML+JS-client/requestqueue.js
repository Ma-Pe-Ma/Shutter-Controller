export class RequestQueue {
    failureCounter = 0;

    #queue = [];
    currentRequest = null;
    guiElements = null;

    currentRequestFinished() {
        this.currentRequest = null;
    }

    addNewRequest(request) {
        this.#queue.push(request);
    }

    constructor(guiElements) {
        this.guiElements = guiElements;
    }

    dequeue() {
        if (this.#queue.length > 0 && this.currentRequest == null)
        {
            this.currentRequest = this.#queue.shift();            
            this.guiElements.setGuiState(false);
            
            this.launchRequest(this.currentRequest);
                
            this.guiElements.stateMessage.innerText = "Synchronisation is in progress...";
        }    
    }

    launchPeriodicStatusRequest() {
        if (this.#queue.length == 0 && this.currentRequest == null) {
            return true;
        }

        return false;
    }

    launchRequest() {
        var that = this;
        setTimeout(function() { that.currentRequest.launchRequest(); }, this.currentRequest.getDelay() * 1000);
    }
};

export class TestRequestQueue extends RequestQueue {
    serializeDate(date) {
        var h = date.getHours() < 10 ? '0' + date.getHours() : date.getHours();
        var m = date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes();
        var mo = (date.getMonth() + 1) < 10 ? '0' + (date.getMonth() + 1) : (date.getMonth() + 1);
        var d = date.getDate() < 10 ? '0' + date.getDate() : date.getDate();
        return `${date.getFullYear()}. ${mo}. ${d}. ${h}:${m}`;
    }

    constructor(guiElements, sh) {
        super(guiElements)
        this.sh = sh;

        var that = this;

        function createFakeDate(offsetHour, offsetMinute) {
            return that.serializeDate(new Date(Date.now() - 1000 * 60 * (offsetHour * 60 + offsetMinute)));
        }

        this.dummy_timings = sh.TimingContainer.create({
            "timing" : [
                {"hour" :  6, "minute" : 0, "days" : [true, true, true, true, true, false, false], "active" : true, "value" : 100},
                {"hour" : 18, "minute" : 0, "days" : [true, true, true, true, true, false, false], "active" : true, "value" : 0},
                {"hour" :  8, "minute" : 0, "days" : [false, false, false, false, false, true, true], "active" : true, "value" : 100},
                {"hour" : 22, "minute" : 0, "days" : [false, false, false, false, false, true, true], "active" : true, "value" : 0},
                {"hour" : 15, "minute" : 30, "days" : [false, false, false, false, false, true, true], "active" : false, "value" : 50},
                {"hour" : 12, "minute" : 45, "days" : [true, true, false, false, false, false, false], "active" : false, "value" : 50}
            ]
        })

        this.dummy_response = sh.Response.create({
            "value" : 45,
            "retryTime" : 0,
            "messageContainer" : {
                "genericMessage" : [
                    {"value" : 45, "event" : 2, "datetime" : createFakeDate(0, 17)},
                    {"value" : 100, "event" : 4, "datetime" : createFakeDate(3, 4)},
                    {"value" : 0, "event" : 7, "datetime" : createFakeDate(4, 37)},
                    {"value" : 70, "event" : 3, "datetime" : createFakeDate(11, 11)},
                    {"value" : 30, "event" : 2, "datetime" : createFakeDate(23, 2)},
                    {"value" : 0, "event" : 1, "datetime" : createFakeDate(23, 5)},
                    {"value" : 0, "event" : 7, "datetime" : createFakeDate(23, 28)},
                    {"value" : 20, "event" : 2, "datetime" : createFakeDate(23, 50)},
                    {"value" : 15, "event" : 6, "datetime" : createFakeDate(25, 3)},
                    {"value" : 15, "event" : 2, "datetime" : createFakeDate(26, 18)}
                ],
                "startTime" : createFakeDate(23, 5)
            }
        })

        this.dummy_timing_setter();
    }

    dummy_timings = {}
    dummy_response = {}

    dummy_timing_setter() {
        const d = new Date();
        let today = d.getDay();
        today = today > 0 ? today - 1 : 6;

        let currentMinutes = 60 * d.getHours() + d.getMinutes();

        var that = this;

        for(var dummy_timing of this.dummy_timings["timing"]) {
            if (dummy_timing["days"][today] && dummy_timing["active"]) {
                var timingSumMinutes = 60 * dummy_timing["hour"] + dummy_timing["minute"];

                if (currentMinutes < timingSumMinutes && timingSumMinutes < currentMinutes + 30) {
                    setTimeout(function(dummy_timing, originalSum) {
                        var timingSumMinutes =  60 * dummy_timing["hour"] + dummy_timing["minute"];

                        if (Math.abs(timingSumMinutes - originalSum) <= 1) {
                            that.dummy_response["value"] = dummy_timing["value"];
                            that.addNewMessage(3, dummy_timing["value"]);
                        }

                    }, (timingSumMinutes - currentMinutes) * 60 * 1000, dummy_timing, timingSumMinutes);
                }
            }
        }
    }    

    addNewMessage(event, value) {
        for (var i = 8; i >= 0; i--) {
            this.dummy_response["messageContainer"]["genericMessage"][i + 1] = this.dummy_response["messageContainer"]["genericMessage"][i];
        }

        this.dummy_response["messageContainer"]["genericMessage"][0] = this.sh.GenericMessage.create({
            "value" : value,
            "event" : event,
            "datetime" : this.serializeDate(new Date())
        })
    }

    launchRequest(request) {
        var that = this;
        setTimeout(function() {that.launchTestRequest(request);}, 500);
    }

    launchTestRequest(request) {
        var delay = 0;
        var requestData = request.getPostData() != null ? this.sh.Request.decode(request.getPostData()) : null;

        var location = request.getLocation();

        switch(location) {
            case "/D":
                
                break;
            case "/V":
                if (this.dummy_response["value"] != requestData["value"]) {
                    delay = Math.round(20.0 * Math.abs(this.dummy_response["value"] - requestData["value"]) / 100);
                    this.dummy_response["value"] = requestData["value"];
                    this.addNewMessage(2, requestData["value"]);
                }
                else {
                    delay = 1;
                }                
                break;
            case "/T":
                this.dummy_timings["timing"] = requestData["timing"];
                this.dummy_timing_setter();
                this.addNewMessage(7, 0);
                delay = 1;
                break;
            case "/Z":
                var z = requestData["zero"];

                if (z == this.sh.Zero.values.up) {
                    delay = 1;
                    this.dummy_response["value"] = 100;
                    this.addNewMessage(4, 100);
                }
                else if (z == this.sh.Zero.values.down) {
                    delay = 1;
                    this.dummy_response["value"] = 0;
                    this.addNewMessage(4, 0);
                }
                else {
                    delay = 10;
                    this.addNewMessage(6, requestData["value"]);
                }
                
                break;
        }

        var that = this;
        setTimeout(function() {
            if (location == "/D") {                
                var responseBuffer = that.sh.TimingContainer.encode(that.dummy_timings).finish();
                that.guiElements.parseTimings(responseBuffer)
            }
            else {
                var responseBuffer = that.sh.Response.encode(that.dummy_response).finish();;
                that.guiElements.parseGenericResponse(responseBuffer)
            }

            that.currentRequest = null;

            that.guiElements.setGuiState(true);
            that.guiElements.stateMessage.innerText = "The server is up and running!";
        }, delay * 1000);
    }    
}