#include <drogon/drogon.h>

#include <iostream>

using namespace drogon;

int main(int argc, const char* argv[]) {
    drogon::app().loadConfigFile("/config/mysqlconfig.json").run();

    const char* envPort = std::getenv("PORT");
    uint16_t port = envPort ? std::stoi(envPort) : 8080;
    // tell drogon where your static files live INSIDE the container
    // (Dockerfile copies ./docs -> /docs)
    app().setDocumentRoot("/docs");
    // bind to all interfaces so Docker can forward traffic into the container
    app().addListener("0.0.0.0", port);
    std::cout << "http://localhost:8080/index.html" << std::endl;
    // start the server (blocks here)
    app().run();
    return 0;
}