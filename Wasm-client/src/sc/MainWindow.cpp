﻿#include "MainWindow.h"

#include <iostream>
#include <format>

#include <imgui/imgui.h>
#ifdef __EMSCRIPTEN__
#include "request/DemoRequest.h"
#include "request/EmscriptenRequest.h"
#else
#include "request/SimpleRequest.h"
#endif

#include "Shutter.pb.h"

void MainWindow::initializeOfflineRequests(std::shared_ptr<std::map<std::string, std::string>> connectionParameters) {
    std::string siteAddress = (*connectionParameters)["siteAddress"];
    std::string username = (*connectionParameters)["username"];
    std::string password = (*connectionParameters)["password"];
    std::string parameters = "?username=" + username + "&password=" + password;

    requestQueue.setSiteAddress(siteAddress, parameters);
}

void MainWindow::initializeRequests()
{
#ifdef __EMSCRIPTEN__
    if (use_demo_request()) {
        DemoRequest::dummyTimingSetter();
    }    
#endif

    receiveResponse = [this](std::vector<unsigned char> byteArray) -> std::shared_ptr<Request> {
        std::lock_guard<std::mutex> guard(this->guiMutex);

        lastRequestOk = true;

        Shutter::Response response;
        if (!response.ParseFromArray((const void*)&byteArray[0], byteArray.size())) {
            lastRequestOk = false;
            return nullptr;
        }

        if (response.retrytime() > 0) {
            std::shared_ptr<Request> newRequest = createRequest();
            newRequest->setLocation("/S");
            newRequest->setDelay(response.retrytime());
            newRequest->setCallback(this->receiveResponse);
            
            return newRequest;
        }

        sliderValue = response.value();
        currentValue = sliderValue;

        startupMessage = response.messagecontainer().starttime();

        for (int i = 0; i < response.messagecontainer().genericmessage_size(); i++) {
            const Shutter::GenericMessage& generic = response.messagecontainer().genericmessage(i);
            this->messages[i] = Message(generic.event(), generic.value(), generic.datetime());
        }

        lastStatusCheck = std::chrono::system_clock::now();

        return nullptr;
    };

    std::function<std::shared_ptr<Request>(std::vector<unsigned char>&)> receiveResponse;

    //send dump at startup
    auto timingRequest = createRequest();
    timingRequest->setLocation("/D");
    timingRequest->setCallback([this](std::vector<unsigned char>& byteArray) -> std::shared_ptr<Request> {
        std::lock_guard<std::mutex> guard(this->guiMutex);
        
        lastRequestOk = true;

        Shutter::TimingContainer incomingTimingContainer;
        if (!incomingTimingContainer.ParseFromArray((const void*) &byteArray[0], byteArray.size())) {
            lastRequestOk = false;
            return nullptr;
        }

        for (int i = 0; i < incomingTimingContainer.timing_size(); i++) {
            this->timings[i].receiveProtoObject(incomingTimingContainer.timing(i));
        }

        return nullptr;
    });

    auto statusRequest = createRequest();
    statusRequest->setLocation("/S");
    statusRequest->setCallback(this->receiveResponse);

    this->requestQueue.pushNewRequest(timingRequest);
    this->requestQueue.pushNewRequest(statusRequest);
}

void MainWindow::update()
{
    requestQueue.tryDequeue();
    std::lock_guard<std::mutex> guard(this->guiMutex);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(windowSize[0], windowSize[1]));

    ImGui::Begin((*translation)["appname"].c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::BeginDisabled(requestQueue.isActive());

    if (ImGui::BeginTabBar("FunctionTabBar", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem((*translation)["setText"].c_str())) {
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
}

void MainWindow::handleSetting()
{
    ImGui::NewLine();
    ImGui::Text("%s", (*translation)["msetting"].c_str());
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
        Shutter::Request requestContent;
        requestContent.set_value(this->sliderValue);

        std::shared_ptr<std::vector<unsigned char>> serialized = std::make_shared<std::vector<unsigned char>>(requestContent.ByteSizeLong());
        requestContent.SerializeToArray(serialized->data(), requestContent.ByteSizeLong());

        auto request = createRequest();
        request->setLocation("/V");
        request->setPostData(serialized);
        request->setCallback(this->receiveResponse);

        this->requestQueue.pushNewRequest(request);
    }
}

void MainWindow::handleTimings()
{
    ImGui::NewLine();

    Shutter::TimingContainer newTimingContainer;

    for (int i = 0; i < numberOfTimings; i++) {
        ImGui::PushItemWidth(24);

        int idNum = i + 1;
        std::string id = std::vformat((*translation)["timingLabel"], std::make_format_args(idNum));
        ImGui::PushID(id.c_str());

        ImGui::Text("%s", id.c_str());
        ImGui::SameLine();

        ImGui::Text("\t");
        ImGui::SameLine();

        Timing& timing = timings[i];
        int& hour = timing.hour;
        int& minute = timing.minute;

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
        ImGui::Text("\t");
        ImGui::SameLine();
        ImGui::Checkbox((*translation)["active"].c_str(), &timing.active);

        ImGui::NewLine();
        ImGui::Text("\t\t");

        for (int j = 0; j < 7; j++) {
            if (j == 5) {
                ImGui::NewLine();
                ImGui::Text("\t\t");
            }
            
            ImGui::SameLine();            
            ImGui::Checkbox(this->dayNameMap[j].c_str(), &(timing.days[j]));
        }

        ImGui::NewLine();
        ImGui::Text("\t\t");
        ImGui::SameLine();

        ImGui::PushItemWidth(150);
        if (ImGui::SliderInt((*translation)["value"].c_str(), &timing.value, 0, 100))
        {
            timing.value -= timing.value % 5 - (timing.value % 5 < 3 ? 0 : 5);
        }

        ImGui::PopItemWidth();
        
        ImGui::PopID();
        ImGui::NewLine();
        ImGui::Separator();
    }

    if (ImGui::Button((*translation)["update"].c_str()))
    {
        Shutter::Request requestContent;
        
        for (int i = 0; i < 6; i++) {
            this->timings[i].sendProtoObject(requestContent.add_timing());
        }

        std::shared_ptr<std::vector<unsigned char>> serialized = std::make_shared<std::vector<unsigned char>>(requestContent.ByteSizeLong());
        requestContent.SerializeToArray(serialized->data(), requestContent.ByteSizeLong());       

        auto request = createRequest();
        request->setLocation("/T");
        request->setPostData(serialized);
        request->setCallback(this->receiveResponse);

        this->requestQueue.pushNewRequest(request);
    }
}

void MainWindow::handleZeroing()
{
    const std::function<void(Shutter::Zero)> postZero = [this](Shutter::Zero zeroState)
    {
        Shutter::Request requestContent;
        requestContent.set_zero(zeroState);

        std::shared_ptr<std::vector<unsigned char>> serialized = std::make_shared<std::vector<unsigned char>>(requestContent.ByteSizeLong());
        requestContent.SerializeToArray(serialized->data(), requestContent.ByteSizeLong());

        auto request = createRequest();
        request->setLocation("/Z");
        request->setPostData(serialized);
        request->setCallback(this->receiveResponse);

        this->requestQueue.pushNewRequest(request);
    };

    ImGui::NewLine();

    ImGui::Text("%s", ((*translation)["toz"]).c_str());
    ImGui::SameLine();

    if (ImGui::Button((*translation)["up"].c_str()))
    {
        postZero(Shutter::Zero::up);
    }

    ImGui::SameLine();

    if (ImGui::Button((*translation)["down"].c_str()))
    {
        postZero(Shutter::Zero::down);
    }

    ImGui::SameLine();

    if (ImGui::Button((*translation)["automatic"].c_str()))
    {
        postZero(Shutter::Zero::current);
    }
}

void MainWindow::handleMessages() {
    ImGui::NewLine();

    ImGui::Text("%s", std::vformat((*translation)["curPos"], std::make_format_args(currentValue)).c_str());
   
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
    ImGui::Text("%s", std::vformat((*translation)["sstate"], std::make_format_args(stateMessage)).c_str());
    ImGui::PopStyleColor();
    
    ImGui::Text("%s", std::vformat((*translation)["stime"], std::make_format_args(startupMessage)).c_str());

    ImGui::NewLine();
    ImGui::Text("%s", ((*translation)["messages"]).c_str());
    ImGui::NewLine();

    ImGui::Columns(3);
    ImGui::SetColumnWidth(0, 30);
    ImGui::SetColumnWidth(1, 150);
    ImGui::SetColumnWidth(2, 140);

    ImGui::Text("%s", ((*translation)["mid"]).c_str());
    ImGui::NextColumn();

    ImGui::Text("%s", ((*translation)["mevent"]).c_str());
    ImGui::NextColumn();

    ImGui::Text("%s", ((*translation)["mdate"]).c_str());
    ImGui::NextColumn();
    ImGui::Separator();

    for (int i = 0; i < numberOfMessages; i++)
    {
        std::string& processedmessage = messages[i].processedMessage;

        if (processedmessage != "-")
        {
            std::string textID = std::to_string(i + 1);
            ImGui::Text("%s", textID.c_str());
            ImGui::NextColumn();

            std::string message = processedmessage;

            ImGui::Text("%s", message.c_str());
            ImGui::NextColumn();

            ImGui::Text("%s", messages[i].date.c_str());
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