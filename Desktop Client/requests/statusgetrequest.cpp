#include "statusgetrequest.h"
#include <QDebug>
#include <QCoreApplication>

StatusGetRequest::StatusGetRequest() {
    location = "/S";
    timeout = 10000;
}
