#include "MajorController.h"
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>

using namespace drogon;

void MajorController::createMajor(const HttpRequestPtr &req,
                                  std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("name"))
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required field: name");
        callback(resp);
        return;
    }

    std::string name = (*json)["name"].asString();

    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "INSERT INTO Majors (name) VALUES (?)",
        [callback](const orm::Result &r) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k200OK);
            resp->setBody("Major created successfully");
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        name);
}

void MajorController::getAllMajors(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "SELECT major_id, name FROM Majors",
        [callback](const orm::Result &result) {
            Json::Value jsonResponse(Json::arrayValue);
            for (const auto &row : result)
            {
                Json::Value major;
                major["major_id"] = row["major_id"].as<int>();
                major["name"] = row["name"].as<std::string>();
                jsonResponse.append(major);
            }
            auto resp = HttpResponse::newHttpJsonResponse(jsonResponse);
            resp->setStatusCode(k200OK);
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        });
}

void MajorController::getMajorById(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback,
                                   std::string majorId)
{
    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "SELECT major_id, name FROM Majors WHERE major_id = ?",
        [callback](const orm::Result &result) {
            if (result.empty())
            {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k404NotFound);
                resp->setBody("Major not found");
                callback(resp);
                return;
            }
            const auto &row = result[0];
            Json::Value major;
            major["major_id"] = row["major_id"].as<int>();
            major["name"] = row["name"].as<std::string>();
            auto resp = HttpResponse::newHttpJsonResponse(major);
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        majorId);
}

void MajorController::updateMajor(const HttpRequestPtr &req,
                                  std::function<void(const HttpResponsePtr &)> &&callback,
                                  std::string majorId)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("name"))
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required field: name");
        callback(resp);
        return;
    }

    std::string name = (*json)["name"].asString();

    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "UPDATE Majors SET name = ? WHERE major_id = ?",
        [callback](const orm::Result &r) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k200OK);
            resp->setBody("Major updated successfully");
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        name, majorId);
}

void MajorController::deleteMajor(const HttpRequestPtr &req,
                                  std::function<void(const HttpResponsePtr &)> &&callback,
                                  std::string majorId)
{
    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "DELETE FROM Majors WHERE major_id = ?",
        [callback](const orm::Result &r) {
            auto resp = HttpResponse::newHttpResponse();
            if (r.affectedRows() == 0)
            {
                resp->setStatusCode(k404NotFound);
                resp->setBody("Major not found");
            }
            else
            {
                resp->setStatusCode(k200OK);
                resp->setBody("Major deleted successfully");
            }
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        majorId);
}
