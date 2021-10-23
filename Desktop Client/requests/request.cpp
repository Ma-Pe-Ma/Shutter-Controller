#include "request.h"
#include "requestqueue.h"

RequestQueue Request::requestQueue;


Request::Request() {

}

void Request::run() {
    this->get();
}

void Request::get() {
    httplib::Client cli(SERVER_URI);
    cli.set_ca_cert_path(path.c_str());
    cli.enable_server_certificate_verification(true);
    cli.set_connection_timeout(timeout);

    std::string sub = location + arguments;
    httplib::Result res = cli.Get(sub.c_str());

    if (res == nullptr) {
        emit requestFinish("");
    }
    else {
        emit requestFinish(res->body);
    }
}

void Request::post() {
    httplib::Client cli(SERVER_URI);
    cli.set_ca_cert_path(path.c_str());
    cli.enable_server_certificate_verification(true);
    cli.set_connection_timeout(timeout);

    std::string sub = location + arguments;
    httplib::Result res = cli.Post(sub.c_str(), postString, "text/plain");

    if (res == nullptr) {
        emit requestFinish("");
    }
    else {
        emit requestFinish(res->body);
    }
}
