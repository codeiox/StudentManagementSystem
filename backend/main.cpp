#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main(int argc, const char* argv[]) {
    const char* envPort = std::getenv("PORT");
    uint16_t port = envPort ? std::stoi(envPort) : 8080;

    drogon::app()
        .loadConfigFile("/backend/config/mysqlconfig.json")
        .loadConfigFile("/backend/config/static.json")
        .setDocumentRoot("/docs")
        .addListener("0.0.0.0", port);


    std::cout << "http://localhost:8080/index.html" << std::endl;

    drogon::app().run();
    return 0;
}
