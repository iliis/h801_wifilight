#include "webserver.hpp"
#include "FileSystem.h"
#include "Network/Http/HttpResource.h"
#include "Network/Http/HttpResourceTree.h"

Webserver::Webserver()
{
}


void Webserver::start()
{
    server.listen(80);

    server.paths.set("/", HttpPathDelegate(&Webserver::on_index, this));
    server.paths.setDefault(HttpPathDelegate(&Webserver::on_file, this));

    debugf("started webserver");

    debugf("--- all files in filesystem: ---");
    for(String s: fileList()) {
        debugf("%s", s.c_str());
    }
    debugf("total: %d files", fileList().size());
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

