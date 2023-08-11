#ifndef SIMPLEREQUEST_H
#define SIMPLEREQUEST_H

#include "Request.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <cpp-httplib/httplib.h>
#include <json.hpp>
#include <exception>

class SimpleRequest : public Request {

public:
	void launchRequest() override
	{
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay * 1000));

        requestThread = std::thread([this]() -> void {
            httplib::Client cli(siteAddress);

            cli.set_ca_cert_path("./resources/cert.pem");
            cli.enable_server_certificate_verification(true);
            cli.set_connection_timeout(timeout);
            //cli.set_read_timeout(timeout, 0);
            //cli.set_write_timeout(timeout, 0);

            std::string sub = location + parameters;

            httplib::Result res;
            try {
                if (postdata == nullptr || postdata->empty())
                {
                    if (res = cli.Get(sub))
                    {
                        response = nlohmann::json::parse(res->body);
                        lastRequestSuccess = true;
                    }
                    else
                    {
                        lastRequestSuccess = false;
                        std::cout << "HTTP error: " << httplib::to_string(res.error()) << std::endl;
                    }
                }
                else {
                    if (res = cli.Post(sub, postdata->dump(), "text/plain"))
                    {
                        response = nlohmann::json::parse(res->body);
                        lastRequestSuccess = true;
                    }
                    else
                    {
                        lastRequestSuccess = false;
                        std::cout << "HTTP error: " << httplib::to_string(res.error()) << std::endl;
                    }
                }
            }
            catch (std::exception e)
            {
                //TODO:
            }

            finished.store(true);
        });       
    }	
};

#endif