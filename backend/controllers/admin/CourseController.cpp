#include "CourseController.h"
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>

using namespace drogon;

// GET /api/admin/courses → return all courses
void CourseController::getAllCourses(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto client = app().getDbClient("default"); // connect to default DB
    client->execSqlAsync(
        "SELECT course_id, course_name, course_description, credits FROM Courses",
        [callback](const orm::Result &result)
        {
            Json::Value jsonResponse(Json::arrayValue); // JSON array for all courses
            for (const auto &row : result)
            {
                Json::Value course;
                course["course_id"] = row["course_id"].as<int>(); // course ID
                course["course_name"] = row["course_name"].as<std::string>(); // course name
                course["course_description"] = row["course_description"].as<std::string>(); // description
                course["credits"] = row["credits"].as<int>(); // credit hours
                jsonResponse.append(course);
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

// GET /api/admin/courses/{courseId} → return one course by ID
void CourseController::getCourseById(const HttpRequestPtr &req,
                                     std::function<void(const HttpResponsePtr &)> &&callback,
                                     std::string courseId)
{
    auto client = app().getDbClient("default"); // connect to default DB
    client->execSqlAsync(
        "SELECT course_id, course_name, course_description, credits FROM Courses WHERE course_id = ?",
        [callback](const orm::Result &result)
        {
            if (result.empty())
            {
                // return 404 if no course found
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k404NotFound);
                resp->setBody("Course not found");
                callback(resp);
                return;
            }
            const auto &row = result[0]; // first row = course
            Json::Value course;
            course["course_id"] = row["course_id"].as<int>();
            course["course_name"] = row["course_name"].as<std::string>();
            course["course_description"] = row["course_description"].as<std::string>();
            course["credits"] = row["credits"].as<int>();
            auto resp = HttpResponse::newHttpJsonResponse(course); // send JSON response
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
        courseId); // pass courseId into query
}
