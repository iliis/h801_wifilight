#pragma once

#include <Network/Http/HttpRequest.h>
#include <Network/Http/HttpResponse.h>
#include <SmingCore.h>

#include "configuration.hpp"

class Webserver
{
public:
    Webserver(Configuration& global_config);

    void start();

private:

    void on_index(HttpRequest& request, HttpResponse& response);
    void on_file(HttpRequest& request, HttpResponse& response);
    void on_set_led(HttpRequest& request, HttpResponse& response);

    void on_config(HttpRequest& request, HttpResponse& response);
    void on_get_config(HttpRequest& request, HttpResponse& response);
    void on_set_config(HttpRequest& request, HttpResponse& response);

private:
    HttpServer server;
    Configuration& global_config;
};
