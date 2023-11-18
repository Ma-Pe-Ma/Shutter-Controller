#include "MainWindow.h"

#include <iostream>
#include <format>

#include <imgui/ImGui.h>
 
#ifdef __EMSCRIPTEN__
#include "Request/DemoRequest.h"
#include "Request/EmscriptenRequest.h"
#else
#include "Request/SimpleRequest.h"
#endif

void MainWindow::initializeRequests(std::shared_ptr<std::map<std::string, std::string>> connectionParameters)
{
#ifdef __EMSCRIPTEN__
    if (use_demo_request()) {
        DemoRequest::initialize();
    }
#endif

    std::string siteAddress = (*connectionParameters)["siteAddress"];
    std::string username = (*connectionParameters)["username"];
    std::string password = (*connectionParameters)["password"];
    std::string parameters = "?username=" + username + "&password=" + password;

    requestQueue.setSiteAddress(siteAddress, parameters);

    receiveResponse = [this](nlohmann::json responseObject) -> std::shared_ptr<Request> {
        this->guiMutex.lock();

        std::shared_ptr<Request> newRequest = nullptr;

        if (responseObject.empty())
        {
            lastRequestOk = false;
        }
        else {
            bool isDump = responseObject.contains("T");

            if (isDump) {
                nlohmann::json timingsObject = responseObject["T"].get<nlohmann::json>();

                for (int i = 0; i < numberOfTimings; i++)
                {
                    std::string key = std::to_string(i);

                    nlohmann::json timingObject = timingsObject[key];

                    timings[i].value = timingObject["V"];
                    timings[i].hour = timingObject["H"];
                    timings[i].minute = timingObject["M"];
                    timings[i].active = timingObject["A"];
                    timings[i].parseDayState(timingObject["D"]);
                }
            }

            nlohmann::json genericResponse = responseObject["G"].get<nlohmann::json>();

            int restartTime = genericResponse["R"].get<int>();

            if (restartTime > 0)
            {
                newRequest = createRequest();
                newRequest->setLocation("/S");
                newRequest->setDelay(restartTime);
                newRequest->setCallback(this->receiveResponse);
            }
            else
            {
                sliderValue = genericResponse["V"].get<int>();
                currentValue = sliderValue;
                nlohmann::json messageContainer = genericResponse["M"].get<nlohmann::json>();

                startupMessage = messageContainer["S"];

                nlohmann::json messages = messageContainer["M"];

                for (int i = 0; i < numberOfMessages; i++)
                {
                    nlohmann::json message = messages[std::to_string(i)].get<nlohmann::json>();
                    auto t = message["T"].get<std::string>();
                    auto r = message["R"].get<std::string>();
                    auto a = message["A"].get<std::string>();
                    auto d = message["D"].get<std::string>();
                    this->messages[i] = Message(t, r, a, d);
                }
            }

            lastRequestOk = true;
        }

        lastStatusCheck = std::chrono::system_clock::now();

        this->guiMutex.unlock();

        return newRequest;
    };

    //send dump at startup
    auto request = createRequest();
    request->setLocation("/D");
    request->setCallback(receiveResponse);

    this->requestQueue.pushNewRequest(request);
}

void MainWindow::update()
{
    requestQueue.tryDequeue();
    guiMutex.lock();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(windowSize[0],windowSize[1]));

    ImGui::Begin((*translation)["appname"].c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::BeginDisabled(requestQueue.isActive());

    if (ImGui::BeginTabBar("FunctionTabBar", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem((*translation)["set"].c_str())) {
            handleSetting();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem((*translation)["timings"].c_str())) {
            handleTimings();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem((*translation)["zeroing"].c_str())) {
            handleZeroing();

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::EndDisabled();

    handleMessages();

    ImGui::End();

    auto now = std::chrono::system_clock::now();
    
    std::chrono::duration<double, std::milli> work_time = now - lastStatusCheck;
    if (work_time.count() > refreshPeriod * 1000.0) {
        lastStatusCheck = now;

        auto request = createRequest();
        request->setLocation("/S");
        request->setCallback(this->receiveResponse);

        this->requestQueue.pushNewRequestIfQueueIsEmpty(request);
    }

    guiMutex.unlock();
}

void MainWindow::handleSetting()
{
    ImGui::NewLine();
    ImGui::Text((*translation)["msetting"].c_str());
    ImGui::SameLine();

    ImGui::PushItemWidth(150);
    if (ImGui::SliderInt((*translation)["svalue"].c_str(), &sliderValue, 0, 100))
    {
        int digit = sliderValue % 5;
        sliderValue = digit < 3 ? sliderValue - digit : sliderValue - digit + 5;
    }
    ImGui::PopItemWidth();
    if (ImGui::Button("Set"))
    {
        std::shared_ptr<nlohmann::json> postdata = std::make_shared<nlohmann::json>();
        (*postdata)["V"] = this->sliderValue;

        auto request = createRequest();
        request->setLocation("/V");
        request->setPostData(postdata);
        request->setCallback(this->receiveResponse);

        this->requestQueue.pushNewRequest(request);
    }
}

void MainWindow::handleTimings()
{
    ImGui::NewLine();

    for (int i = 0; i < numberOfTimings; i++) {
        ImGui::PushItemWidth(24);

        std::string id = std::vformat((*translation)["timing"], std::make_format_args(i + 1));
        ImGui::PushID(id.c_str());

        ImGui::Text(id.c_str());
        ImGui::SameLine();

        int& hour = timings[i].hour;
        int& minute = timings[i].minute;

        if (ImGui::InputInt(":", &hour, 0, 23))
        {
            hour = hour < 0 ? 0 : hour;
            hour = hour > 23 ? 23 : hour;
        }

        ImGui::SameLine();
        if (ImGui::InputInt((*translation)["time"].c_str(), &minute, 0, 59))
        {
            minute = minute < 0 ? 0 : minute;
            minute = minute > 59 ? 59 : minute;
        }

        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::PushItemWidth(150);
        if (ImGui::SliderInt((*translation)["value"].c_str(), &timings[i].value, 0, 100))
        {
            int digit = timings[i].value % 5;
            timings[i].value = digit < 3 ? timings[i].value - digit : timings[i].value - digit + 5;
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox((*translation)["active"].c_str(), &timings[i].active);

        ImGui::NewLine();       
        ImGui::Text("\t\t\t\t");

        for (int j = 0; j < 7; j++)
        {
            ImGui::SameLine();
            ImGui::Checkbox(Timing::nameMap[j].c_str(), &(timings[i].days[j]));
        }
        
        ImGui::PopID();
        ImGui::NewLine();
    }

    if (ImGui::Button((*translation)["update"].c_str()))
    {
        std::shared_ptr<nlohmann::json> postdata = std::make_shared<nlohmann::json>();
        
        for (int i = 0; i < numberOfTimings; i++)
        {
            nlohmann::json timingObject;

            timingObject["H"] = timings[i].hour;
            timingObject["M"] = timings[i].minute;
            timingObject["A"] = timings[i].active;
            timingObject["V"] = timings[i].value;
            timingObject["D"] = timings[i].serializeDayState();

            (*postdata)[std::to_string(i)] = timingObject;
        }

        auto request = createRequest();
        request->setLocation("/T");
        request->setPostData(postdata);
        request->setCallback(this->receiveResponse);

        this->requestQueue.pushNewRequest(request);
    }
}

void MainWindow::handleZeroing()
{
    const std::function<void(std::string)> postZero = [this](std::string zeroState)
    {
        std::shared_ptr<nlohmann::json> postdata = std::make_shared<nlohmann::json>();
        (*postdata)["Z"] = zeroState;

        auto request = createRequest();
        request->setLocation("/Z");
        request->setPostData(postdata);
        request->setCallback(this->receiveResponse);

        this->requestQueue.pushNewRequest(request);
    };

    ImGui::NewLine();

    ImGui::Text(((*translation)["toz"]).c_str());
    ImGui::SameLine();

    if (ImGui::Button((*translation)["up"].c_str()))
    {
        postZero("up");
    }

    ImGui::SameLine();

    if (ImGui::Button((*translation)["down"].c_str()))
    {
        postZero("down");
    }

    ImGui::SameLine();

    if (ImGui::Button((*translation)["automatic"].c_str()))
    {
        postZero("find");
    }
}

void MainWindow::handleMessages() {
    ImGui::NewLine();

    ImGui::Text(std::vformat((*translation)["curPos"], std::make_format_args(currentValue)).c_str());
   
    std::string stateMessage = (*translation)["avail"];
    auto messageColor = IM_COL32(255, 0, 0, 255);

    if (requestQueue.isActive())
    {
        stateMessage = (*translation)["sync"];
        messageColor = IM_COL32(0, 0, 255, 255);

    }
    else if (lastRequestOk && requestQueue.getLastRequestSuccess())
    {
        stateMessage = (*translation)["running"];
        messageColor = IM_COL32(0, 255, 0, 255);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, messageColor);
    ImGui::Text(std::vformat((*translation)["sstate"], std::make_format_args(stateMessage)).c_str());
    ImGui::PopStyleColor();
    
    ImGui::Text(std::vformat((*translation)["stime"], std::make_format_args(startupMessage)).c_str());

    ImGui::NewLine();
    ImGui::Text(((*translation)["messages"]).c_str());
    ImGui::NewLine();

    ImGui::Columns(3);
    ImGui::SetColumnWidth(0, 30);
    ImGui::SetColumnWidth(1, 185);
    ImGui::SetColumnWidth(2, 140);

    ImGui::Text(((*translation)["mid"]).c_str());
    ImGui::NextColumn();

    ImGui::Text(((*translation)["mevent"]).c_str());
    ImGui::NextColumn();

    ImGui::Text(((*translation)["mdate"]).c_str());
    ImGui::NextColumn();
    ImGui::Separator();

    for (int i = 0; i < numberOfMessages; i++)
    {
        std::string& processedmessage = messages[i].processedMessage;

        if (processedmessage != "-")
        {
            std::string textID = std::to_string(i + 1);
            ImGui::Text(textID.c_str());
            ImGui::NextColumn();

            std::string message = processedmessage;

            ImGui::Text(message.c_str());
            ImGui::NextColumn();

            ImGui::Text(messages[i].date.c_str());
            ImGui::NextColumn();
            ImGui::Separator();
        }
    }
    
    ImGui::Columns(1);   
}

std::shared_ptr<Request> MainWindow::createRequest()
{
#ifdef __EMSCRIPTEN__
    if (use_demo_request()) {
        return std::make_shared<DemoRequest>();
    }
    
    return std::make_shared<EmscriptenRequest>();    
#else
    return std::make_shared<SimpleRequest>();
#endif
}

void MainWindow::terminate()
{
    requestQueue.terminate();
}