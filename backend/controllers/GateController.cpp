#include "GateController.h"
#include <drogon/Cookie.h>
#include <drogon/drogon.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpRequest.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <json/json.h>
#include <regex>
#include <filesystem>

using namespace drogon;
using namespace drogon::orm;

void GateController::handleLogin(const HttpRequestPtr &req,
                                 std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("password"))
    {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Missing credentials"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    // ✅ Basic validation
    if (username.empty() || password.empty())
    {
        Json::Value result;
        result["message"] = "Username and password required";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    try
    {
        auto clientPtr = drogon::app().getDbClient();
        if (!clientPtr)
        {
            throw std::runtime_error("Database client not available");
        }

        // ✅ Use the correct execSqlAsync signature
        std::string sql = "SELECT username, role FROM Users WHERE username = ? AND password = ?";

        clientPtr->execSqlAsync(
            sql,
            [callback, req](const Result &r)
            {
                if (r.empty())
                {
                    Json::Value result;
                    result["message"] = "Incorrect username or password";
                    auto resp = HttpResponse::newHttpJsonResponse(result);
                    resp->setStatusCode(k401Unauthorized);
                    callback(resp);
                }
                else
                {
                    std::string username = r[0]["username"].as<std::string>();
                    std::string role = r[0]["role"].as<std::string>();
                    Json::Value result;
                    result["redirect"] = "/" + role + "/dashboard.html";
                    auto resp = HttpResponse::newHttpJsonResponse(result);

                    // ✅ Correct session handling for newer Drogon versions
                    auto session = req->session();
                    if (session)
                    {
                        session->insert("role", role);
                        session->insert("username", username);
                    }

                    // ✅ Set cookie for role-based access (simpler approach)
                    drogon::Cookie roleCookie("user_role", role);
                    roleCookie.setPath("/");
                    roleCookie.setMaxAge(3600);
                    resp->addCookie(std::move(roleCookie));

                    drogon::Cookie userCookie("username", username);
                    userCookie.setPath("/");
                    userCookie.setMaxAge(3600);
                    resp->addCookie(std::move(userCookie));

                    callback(resp);
                }
            },
            [callback](const DrogonDbException &e)
            {
                LOG_ERROR << "Database error: " << e.base().what();
                Json::Value result;
                result["message"] = "Database error";
                auto resp = HttpResponse::newHttpJsonResponse(result);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            username,
            password);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Exception: " << e.what();
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Server error"));
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void GateController::serveProtectedFile(const HttpRequestPtr &req,
                                        std::function<void(const HttpResponsePtr &)> &&callback,
                                        const std::string &role,
                                        const std::string &path)
{
    // ✅ Check cookie instead of session for simplicity
    auto cookies = req->getCookies();
    auto roleCookie = req->getCookie("user_role");

    if (roleCookie.empty())
    {
        // No role cookie, redirect to login
        auto resp = HttpResponse::newRedirectionResponse("/index.html");
        callback(resp);
        return;
    }

    if (roleCookie != role)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k403Forbidden);
        resp->setBody("Access forbidden");
        callback(resp);
        return;
    }

    // ✅ Path sanitization
    std::string cleanPath = path.empty() ? "index.html" : path;
    if (cleanPath.find("..") != std::string::npos || cleanPath.find("/") == 0)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid path");
        callback(resp);
        return;
    }

    std::string fullPath = drogon::app().getDocumentRoot() + "/" + role + "/" + cleanPath;

    // Check if file exists
    if (!std::filesystem::exists(fullPath))
    {
        // Try with index.html if it's a directory
        if (std::filesystem::is_directory(fullPath))
        {
            fullPath += "/index.html";
        }
        if (!std::filesystem::exists(fullPath))
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            callback(resp);
            return;
        }
    }

    auto resp = HttpResponse::newFileResponse(fullPath);
    callback(resp);
}
