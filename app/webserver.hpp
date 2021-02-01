#pragma once

#include "Network/Http/HttpRequest.h"
#include "Network/Http/HttpResponse.h"
#include <SmingCore.h>

class Webserver
{
public:
    Webserver();

    void start();

private:

    void on_index(HttpRequest& request, HttpResponse& response);
    void on_file(HttpRequest& request, HttpResponse& response);
    void on_set(HttpRequest& request, HttpResponse& response);

private:
    HttpServer server;
};
