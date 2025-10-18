#include "StudentController.h"
#include <drogon/orm/DbClient.h>
#include <json/json.h>

using namespace drogon;

void StudentController::createStudent(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid JSON");
        callback(resp);
        return;
    }

    // Validate required fields
    if (!json->isMember("name") || !json->isMember("email") || 
        !json->isMember("studentId") || !json->isMember("phone") ||
        !json->isMember("dateofbirth") || !json->isMember("address") ||
        !json->isMember("sex"))
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields");
        callback(resp);
        return;
    }

    auto client = app().getDbClient("default");

    // Fixed: Proper Drogon execSqlAsync syntax with parameters after callbacks
    client->execSqlAsync(
        "INSERT INTO Students (name, phone, email, dateofbirth, address, sex, studentId) VALUES (?,?,?,?,?,?,?)",
        [callback](const drogon::orm::Result &)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k200OK);
            resp->setBody("Student created successfully");
            callback(resp);
        },
        [callback](const drogon::orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        (*json)["name"].asString(),
        (*json)["phone"].asString(),
        (*json)["email"].asString(),
        (*json)["dateofbirth"].asString(),
        (*json)["address"].asString(),
        (*json)["sex"].asString(),
        (*json)["studentId"].asString()
    );
}