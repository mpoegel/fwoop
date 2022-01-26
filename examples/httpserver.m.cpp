#include <iostream>

#include <fwoop_httpserver.h>

int main( int argc, char* argv[])
{
    const int port = 8080;
    fwoop::HttpServer server(port);
    if (0 != server.serve()) {
        return 1;
    }

    return 0;
}
