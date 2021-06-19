#include "webserver.hpp"

#include <JsonObjectStream.h>
#include <FileSystem.h>
#include <Network/Http/HttpCommon.h>
#include <Network/Http/HttpResource.h>
#include <Network/Http/HttpResourceTree.h>

#include "led.hpp"
#include "configuration.hpp"

Webserver::Webserver(Configuration& global_config)
  : global_config(global_config)
{
}


void Webserver::start()
{
    server.listen(80);

    server.paths.set("/", HttpPathDelegate(&Webserver::on_index, this));
    server.paths.set("/led", HttpPathDelegate(&Webserver::on_set_led, this));
    server.paths.set("/settings", HttpPathDelegate(&Webserver::on_config, this));
    server.paths.setDefault(HttpPathDelegate(&Webserver::on_file, this));

    debugf("started webserver");

    debugf("--- all files in filesystem: ---");

    Directory dir;
    if (dir.open()) {
        while (dir.next()) {
            debugf("%s", dir.stat().name.c_str());
        }
    } else {
        debugf("ERROR: failed to open root directory");
    }
    

    debugf("total: %d files", dir.count());
    debugf("--------------------------------");
}

void Webserver::on_index(HttpRequest& request, HttpResponse& response)
{
    debugf("got request for index");
    response.setCache(86400, true);
    response.sendFile("index.html");
}

void Webserver::on_file(HttpRequest& request, HttpResponse& response)
{
    String file = request.uri.getRelativePath();

    debugf("got request for '%s'. Exists? %d", file.c_str(), fileExist(file));

    if(file[0] == '.')
        response.code = HTTP_STATUS_FORBIDDEN;
    else {
        response.setCache(86400, true); // It's important to use cache for better performance.
        response.sendFile(file);
    }
}

void Webserver::on_set_led(HttpRequest& request, HttpResponse& response)
{
    int R  = request.getPostParameter("R").toInt();
    int G  = request.getPostParameter("G").toInt();
    int B  = request.getPostParameter("B").toInt();
    int W1 = request.getPostParameter("W1").toInt();
    int W2 = request.getPostParameter("W2").toInt();

    debugf("got POST: %d.%d.%d.%d.%d", R, G, B, W1, W2);

    LED::set(R,G,B,W1,W2);

    response.code = HTTP_STATUS_FOUND;
    response.setHeader("Location", "/");
}

void Webserver::on_config(HttpRequest& request, HttpResponse& response)
{
    if (request.method == HTTP_GET)
    {
        on_get_config(request, response);
    }
    else if (request.method == HTTP_POST)
    {
        on_set_config(request, response);
    }
    else
    {
        response.code = HTTP_STATUS_BAD_REQUEST;
    }
}

void Webserver::on_get_config(HttpRequest& request, HttpResponse& response)
{
    JsonObjectStream* stream = new JsonObjectStream();
    JsonObject json = stream->getRoot();

    global_config.writeToJson(json);

    response.sendDataStream(stream, MIME_JSON);
}

void Webserver::on_set_config(HttpRequest& request, HttpResponse& response)
{
    global_config.setFromHttpPost(request);

    response.code = HTTP_STATUS_FOUND;
    response.setHeader("Location", "/");
}
