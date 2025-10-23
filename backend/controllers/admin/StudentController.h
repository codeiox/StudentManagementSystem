#pragma once
#include <drogon/HttpController.h>

class StudentController : public drogon::HttpController<StudentController> {
public:
    METHOD_LIST_BEGIN
    // Admin-only endpoint to create a new student
    ADD_METHOD_TO(StudentController::createStudent, "/api/admin/students", drogon::Post);
    ADD_METHOD_TO(StudentController::deleteStudent, "/api/admin/students/{1}", drogon::Delete);
    METHOD_LIST_END

    // Create student
    void createStudent(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    // Delete student from the db
    void deleteStudent(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       std::string studentId);
};
