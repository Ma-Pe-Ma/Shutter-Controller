#ifndef EMSCRIPTENREQUEST_H
#define EMSCRIPTENREQUEST_H

#include <iostream>
#include <thread>

#include <emscripten/fetch.h>
#include <emscripten.h>

#include "Request.h"
#include "../EmscriptenGlue.h"

class EmscriptenRequest : public Request
{
public:
	void launchRequest() override
	{
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay * 1000));

        requestThread = std::thread([this]() -> void {

            emscripten_fetch_attr_t attr;
            emscripten_fetch_attr_init(&attr);

            std::string jsonString;

            if (postdata == nullptr || postdata->empty())
            {
                strcpy(attr.requestMethod, "GET");
            }
            else
            {
                jsonString = postdata->dump();
                strcpy(attr.requestMethod, "POST");
                attr.requestData = jsonString.c_str();
                attr.requestDataSize = jsonString.size();
            }

            //const char* headers[] = { "Content-Type", "application/json", "Access-Control-Allow-Origin", "*", 0};
            //const char* headers[] = { "Access-Control-Allow-Origin", "*", 0 };

            //attr.requestHeaders = headers;

            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
            attr.userData = this;
            attr.onsuccess = [](emscripten_fetch_t* fetch) -> void {
                EmscriptenRequest* self = static_cast<EmscriptenRequest*>(fetch->userData);
                
                //std::cout << "STATUS " << fetch->status << std::endl;
                //std::cout << "REPSONSE CONTENT: " << fetch->data << std::endl;

                //This is currently unreachable see explanation below
                if (fetch->status == 302) {

                    auto headerLength = emscripten_fetch_get_response_headers_length(fetch);
                    char* plainHeaders = new char[headerLength + 1];
                    auto hS = emscripten_fetch_get_response_headers(fetch, plainHeaders, headerLength + 1);
                    char** headers = emscripten_fetch_unpack_response_headers(plainHeaders);

                    int index = 0;
                    while (headers[2 * index] != 0)
                    {
                        char* key = headers[2 * index + 0];
                        char* value = headers[2 * index + 1];

                        if (strcmp(key, "Location") || strcmp(key, "location"))
                        {;
                            std::cout << "REDIRECTING FROM WASM MODULE to " << value << std::endl;

                            //not working, see explanation below
                            redirect_to_location(value);
                        }

                        index += 1;
                    }

                    delete[] plainHeaders;
                }                

                std::string responseString(fetch->data);
                responseString = responseString.substr(0, fetch->numBytes);

                try {
                    self->response = nlohmann::json::parse(responseString);
                    self->lastRequestSuccess = true;
                }
                catch (nlohmann::json::exception e) {
                    //std::cout << "e: " << e.what() << std::endl;
                    //self->lastRequestSuccess = false;

                    // lame solution: Emscripten fetch auto redirect so 302 status is indetectable
                    // this why the redirected page is returned in the response which cannot be parsed
                    // so this exception is caught
                    redirect_to_location("/");
                }
            };

            attr.onerror = [](emscripten_fetch_t* fetch) -> void {
                std::cout << "Error sending request: " << fetch->status << " - " << fetch->statusText << std::endl;
                EmscriptenRequest* self = static_cast<EmscriptenRequest*>(fetch->userData);
                self->lastRequestSuccess = false;
            };

            emscripten_fetch(&attr, location.c_str());

            finished.store(true);
        });
	}
};

#endif