#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main(int argc, const char *argv[]) {

    drogon::app().setLogLevel(trantor::Logger::kDebug);

    LOG_INFO << "Starting application";

    const char *envPort = std::getenv("PORT");
    uint16_t port = envPort ? std::stoi(envPort) : 8080;
    try {
        drogon::app()
                .loadConfigFile("/backend/config/mysqlconfig.json")
                .loadConfigFile("/backend/config/static.json")
                .setDocumentRoot("/docs")
                .addListener("0.0.0.0", port);
        LOG_INFO << "Configuration loaded successfully";

    } catch (const std::exception &e) {
        LOG_ERROR << "Failed to load configuration: " << e.what();
        return 1;
    }
    LOG_INFO << "http://localhost:8080/index.html" << "\n";
    drogon::app().run();
    return 0;
}
