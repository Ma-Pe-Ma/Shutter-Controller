#ifndef SIMPLEREQUEST_H
#define SIMPLEREQUEST_H

#include "Request.h"

#include <cpp-httplib/httplib.h>
#include <exception>

class SimpleRequest : public Request {

public:
	void launchRequest() override
	{
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay * 1000));

        requestThread = std::thread([this]() -> void {
            httplib::Client cli(siteAddress);

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            cli.set_ca_cert_path("./resources/cert.pem");
            cli.enable_server_certificate_verification(true);
#endif            
            cli.set_connection_timeout(timeout);
            //cli.set_read_timeout(timeout, 0);
            //cli.set_write_timeout(timeout, 0);

            std::string sub = location + parameters;

            httplib::Result res;
            try {
                if (postdata == nullptr)
                {
                    if (res = cli.Get(sub))
                    {
                        response.insert(response.begin(), res->body.begin(), res->body.end());
                        lastRequestSuccess = true;
                    }
                    else
                    {
                        lastRequestSuccess = false;
                        std::cout << "HTTP error: " << httplib::to_string(res.error()) << std::endl;
                    }
                }
                else {
                    if (res = cli.Post(sub, (const char*) postdata->data(), postdata->size(), "text/plain"))
                    {
                        response.insert(response.begin(), res->body.begin(), res->body.end());
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