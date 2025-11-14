#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <drogon/HttpRequest.h>
#include <json/json.h>
#include <string>

class CourseController : public drogon::HttpController<CourseController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CourseController::createCourse, "/api/admin/courses", drogon::Post);
    ADD_METHOD_TO(CourseController::getAllCourses, "/api/admin/courses", drogon::Get);
    ADD_METHOD_TO(CourseController::getCourseById, "/api/admin/courses/{courseId}", drogon::Get);
    ADD_METHOD_TO(CourseController::updateCourse, "/api/admin/courses/{courseId}", drogon::Put);
    ADD_METHOD_TO(CourseController::deleteCourse, "/api/admin/courses/{courseId}", drogon::Delete);
    METHOD_LIST_END

    void createCourse(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getAllCourses(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getCourseById(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       std::string courseId);

    void updateCourse(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      std::string courseId);

    void deleteCourse(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      std::string courseId);
};
