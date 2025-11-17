#pragma once
#include <drogon/HttpController.h>

class GateController : public drogon::HttpController<GateController> {
   public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(GateController::handleLogin, "/api/login", drogon::Post);  // FIXED path
    ADD_METHOD_TO(GateController::serveProtectedFile, "/(admin|student)/{path:.+}", drogon::Get);
    METHOD_LIST_END

    void handleLogin(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void serveProtectedFile(const drogon::HttpRequestPtr& req,
                            std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                            const std::string& role, const std::string& path);
};
