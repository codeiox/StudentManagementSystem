#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <sodium.h>

#include <iostream>
#include <string>

#include "../auth/Hasher.h"
#include "../auth/StoreCredential.h"
class StudentController : public drogon::HttpController<StudentController>
{
public:
   METHOD_LIST_BEGIN
   // Admin-only endpoint to create a new student
   ADD_METHOD_TO(StudentController::createStudent, "/api/admin/students", drogon::Post);
   // Route: GET /api/admin/students → calls getAllStudents
   ADD_METHOD_TO(StudentController::getAllStudents, "/api/admin/students", drogon::Get);

   METHOD_LIST_END

   // Handler
   void createStudent(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

   // Handles GET request to return all students as JSON
   void getAllStudents(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);

   // TODO: In the next story - We may need to add another function that checks for data duplicates before inserting data into DB.

};
