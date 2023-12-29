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
        return `${date.getFullYear()}. ${date.getMonth() + 1}. ${date.getDate()}. ${h}:${m}`;
    }

    constructor(guiElements) {
        super(guiElements)

        for (var i = 0; i < 10; i++) {
            var message = this.dummy_messages[String(i)];
            var time  = message["D"];
            message["D"] = this.serializeDate(new Date(Date.now() - 1000 * 60 * (time[0] * 60 + time[1])));
        }
        
        this.dummy_timing_setter();
    }

    dummy_value = 45;

    dummy_timings = {
        "0" : {"H" :  6, "M" : 0, "D" : "TTTTTFF", "A" :  true, "V" : 100},
        "1" : {"H" : 18, "M" : 0, "D" : "TTTTTFF", "A" :  true, "V" :  0},
        "2" : {"H" :  8, "M" : 0, "D" : "FFFFFTT", "A" :  true, "V" : 100},
        "3" : {"H" : 22, "M" : 0, "D" : "FFFFFTT", "A" :  true, "V" :   0},
        "4" : {"H" : 15, "M" : 30, "D" : "FFFFFTT", "A" : false, "V" :  50},
        "5" : {"H" : 12, "M" : 45, "D" : "TTFFFFF", "A" : false, "V" :  50}
    }

    dummy_messages = {
        "0" : {"T" : "S", "R" : "M", "A" : "45", "D" : [0, 17]},
        "1" : {"T" : "Z", "R" : "O", "A" : "U", "D" : [3, 4]},
        "2" : {"T" : "S", "R" : "T", "A" : "", "D" : [4, 37]},
        "3" : {"T" : "T", "R" : "1", "A" : "70", "D" : [11, 11]},
        "4" : {"T" : "S", "R" : "M", "A" : "30", "D" : [23, 2]},
        "5" : {"T" : "I", "R" : "", "A" : "", "D" : [23, 5]},
        "6" : {"T" : "S", "R" : "T", "A" : "", "D" : [23, 28]},
        "7" : {"T" : "S", "R" : "M", "A" : "20", "D" : [23, 50]},
        "8" : {"T" : "S", "R" : "Z", "A" : "15", "D" : [25, 3]},        
        "9" : {"T" : "S", "R" : "M", "A" : "15", "D" : [26, 18]},
    }

    dummy_timing_setter() {
        const d = new Date();
        let today = d.getDay();
        today = today > 0 ? today - 1 : 6;

        let currentMinutes = 60 * d.getHours() + d.getMinutes();

        var that = this;

        for (var i = 0; i < 6; i++) {
            var dummy_timing = this.dummy_timings[i];

            if (dummy_timing["D"][today] == "T" && dummy_timing["A"]) {
                var timingSumMinutes = 60 * dummy_timing["H"] + dummy_timing["M"];

                if (currentMinutes < timingSumMinutes && timingSumMinutes < currentMinutes + 30) {
                    setTimeout(function(index, originalSum) {
                        var currentTiming = that.dummy_timings[index];
                        var timingSumMinutes =  60 * currentTiming["H"] + currentTiming["M"];

                        if (Math.abs(timingSumMinutes - originalSum) <= 1) {
                            that.dummy_value = currentTiming["V"];
                            that.addNewMessage("T", index + 1, currentTiming["V"]);
                        }

                    }, (timingSumMinutes - currentMinutes) * 60 * 1000, i, timingSumMinutes);
                }
            }
        }
    }    

    addNewMessage(t, r, a) {
        for (var i = 8; i >= 0; i--) {
            this.dummy_messages[String(i+1)] = this.dummy_messages[String(i)];
        }

        this.dummy_messages["0"] = {"T" : t, "R" : r, "A" : a, "D" : this.serializeDate(new Date())};
    }

    launchRequest(request) {
        var that = this;
        setTimeout(function() {that.launchTestRequest(request);}, 500);
    }

    launchTestRequest(request) {
        var delay = 0;            

        switch(request.getLocation()) {
            case "/D":
                
                break;
            case "/V":
                if (this.dummy_value != request.getPostData()["V"]) {
                    this.dummy_value = request.getPostData()["V"];
                    this.addNewMessage("S", "M", this.dummy_value);
                }
                delay = 1;
                break;
            case "/T":
                this.dummy_timings = request.getPostData();
                this.dummy_timing_setter();
                this.addNewMessage("S", "T", "");
                break;
            case "/Z":    
                var z = request.getPostData()["Z"];

                if (z == "up") {
                    delay = 2;
                    this.addNewMessage("Z", "O", "U");
                    this.dummy_value = 100;
                }
                else if (z == "down") {
                    delay = 2;
                    this.addNewMessage("Z", "O", "D");
                    this.dummy_value = 0;
                }
                else {
                    delay = 10;
                    this.addNewMessage("S", "Z", this.dummy_value);
                }                
                
                break;
        }

        var responseObject = {
            "G" : {
                "R" : 0,
                "V" : this.dummy_value,
                "M" : {
                    "M" : this.dummy_messages,
                    "S" : this.serializeDate(new Date(Date.now() - 1000 * 60 * ( 60 * 23 + 5)))
                }
            }
        }

        if (request.getLocation() == "/D") {
            responseObject["T"] = this.dummy_timings;
        }

        var that = this;
        setTimeout(function() {
            that.guiElements.parseResponse(responseObject);
            that.currentRequest = null;

            that.guiElements.setGuiState(true);
            that.guiElements.stateMessage.innerText = "The server is up and running!";
        }, delay * 1000);
    }    
}