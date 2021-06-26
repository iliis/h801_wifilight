#pragma once

#include <Network/Http/HttpRequest.h>
#include <Network/Http/HttpResponse.h>
#include <SmingCore.h>

class Application; // forward def

class Webserver
{
public:
    Webserver(Application& app);

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
    Application& app;
};
