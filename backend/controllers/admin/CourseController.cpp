#include "CourseController.h"
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>

using namespace drogon;

void CourseController::createCourse(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("course_name") || !json->isMember("credits"))
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields: course_name, credits");
        callback(resp);
        return;
    }

    std::string name = (*json)["course_name"].asString();
    std::string description = json->get("course_description", "").asString();
    int credits = (*json)["credits"].asInt();

    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "INSERT INTO Courses (course_name, course_description, credits) VALUES (?, ?, ?)",
        [callback](const orm::Result &r) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k200OK);
            resp->setBody("Course created successfully");
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        name, description, credits);
}

void CourseController::getAllCourses(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "SELECT course_id, course_name, course_description, credits FROM Courses",
        [callback](const orm::Result &result) {
            Json::Value jsonResponse(Json::arrayValue);
            for (const auto &row : result)
            {
                Json::Value course;
                course["course_id"] = row["course_id"].as<int>();
                course["course_name"] = row["course_name"].as<std::string>();
                course["course_description"] = row["course_description"].as<std::string>();
                course["credits"] = row["credits"].as<int>();
                jsonResponse.append(course);
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

void CourseController::getCourseById(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback,
                                     std::string courseId)
{
    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "SELECT course_id, course_name, course_description, credits FROM Courses WHERE course_id = ?",
        [callback](const orm::Result &result) {
            if (result.empty())
            {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k404NotFound);
                resp->setBody("Course not found");
                callback(resp);
                return;
            }
            const auto &row = result[0];
            Json::Value course;
            course["course_id"] = row["course_id"].as<int>();
            course["course_name"] = row["course_name"].as<std::string>();
            course["course_description"] = row["course_description"].as<std::string>();
            course["credits"] = row["credits"].as<int>();
            auto resp = HttpResponse::newHttpJsonResponse(course);
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        courseId);
}

void CourseController::updateCourse(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback,
                                    std::string courseId)
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

    std::string name = json->get("course_name", "").asString();
    std::string description = json->get("course_description", "").asString();
    int credits = json->get("credits", 0).asInt();

    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "UPDATE Courses SET course_name = ?, course_description = ?, credits = ? WHERE course_id = ?",
        [callback](const orm::Result &r) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k200OK);
            resp->setBody("Course updated successfully");
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        name, description, credits, courseId);
}

void CourseController::deleteCourse(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback,
                                    std::string courseId)
{
    auto client = app().getDbClient("default");
    client->execSqlAsync(
        "DELETE FROM Courses WHERE course_id = ?",
        [callback](const orm::Result &r) {
            auto resp = HttpResponse::newHttpResponse();
            if (r.affectedRows() == 0)
            {
                resp->setStatusCode(k404NotFound);
                resp->setBody("Course not found");
            }
            else
            {
                resp->setStatusCode(k200OK);
                resp->setBody("Course deleted successfully");
            }
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        courseId);
}
