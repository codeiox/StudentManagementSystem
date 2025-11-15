#include "MajorController.h"
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>

using namespace drogon;

// GET /api/admin/majors → return all majors/minors
void MajorController::getAllMajors(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto client = app().getDbClient("default"); // connect to default DB
    client->execSqlAsync(
        "SELECT major_id, name, type FROM Majors",
        [callback](const orm::Result &result)
        {
            Json::Value jsonResponse(Json::arrayValue); // JSON array for majors
            for (const auto &row : result)
            {
                Json::Value major;
                major["major_id"] = row["major_id"].as<int>(); // major ID
                major["name"] = row["name"].as<std::string>(); // major name
                major["type"] = row["type"].as<std::string>(); // "major" or "minor"
                jsonResponse.append(major);
            }
            auto resp = HttpResponse::newHttpJsonResponse(jsonResponse); // send JSON response
            resp->setStatusCode(k200OK);
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e)
        {
            // send 500 error if DB query fails
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        });
}

// GET /api/admin/majors/{majorId} → return one major by ID
void MajorController::getMajorById(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback,
                                   std::string majorId)
{
    auto client = app().getDbClient("default"); // connect to default DB
    client->execSqlAsync(
        "SELECT major_id, name FROM Majors WHERE major_id = ?",
        [callback](const orm::Result &result)
        {
            if (result.empty())
            {
                // return 404 if no major found
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k404NotFound);
                resp->setBody("Major not found");
                callback(resp);
                return;
            }
            const auto &row = result[0]; // first row = major
            Json::Value major;
            major["major_id"] = row["major_id"].as<int>();
            major["name"] = row["name"].as<std::string>();
            auto resp = HttpResponse::newHttpJsonResponse(major); // send JSON response
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e)
        {
            // send 500 error if DB query fails
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        majorId); // pass majorId into query
}
