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
        (*json)["studentId"].asString());
}

// Handles GET request to retrieve all student records from the database
void StudentController::getAllStudents(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback)
{
    // Get the default database client instance
    auto client = app().getDbClient("default");

    // Execute SQL query asynchronously to fetch student data
    client->execSqlAsync(
        "SELECT id, name, phone, email, dateofbirth, address, sex, studentId FROM Students",

        // Success callback: transform DB result into JSON array
        [callback](const drogon::orm::Result &result)
        {
            Json::Value jsonResponse(Json::arrayValue);

            // Iterate through each row and build a JSON object for each student
            for (const auto &row : result)
            {
                Json::Value student;
                student["studentId"] = row["studentId"].as<std::string>();
                student["name"] = row["name"].as<std::string>();
                student["dateofbirth"] = row["dateofbirth"].as<std::string>();
                student["sex"] = row["sex"].as<std::string>();
                student["phone"] = row["phone"].as<std::string>();
                student["email"] = row["email"].as<std::string>();
                student["address"] = row["address"].as<std::string>();
                //student["enrollmentStatus"] = row["enrollmentStatus"].as<std::string>(); // May be null
                jsonResponse.append(student);
            }

            // Return JSON response with HTTP 200 OK
            auto resp = HttpResponse::newHttpJsonResponse(jsonResponse);
            resp->setStatusCode(k200OK);
            callback(resp);
        },

        // Error callback: return HTTP 500 with error message
        [callback](const drogon::orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        });
}
