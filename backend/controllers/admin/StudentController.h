#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <drogon/HttpRequest.h>

#include <json/json.h>
#include <sodium.h>
#include <cctype>
#include <drogon/drogon.h>
#include <algorithm>
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
   // Route: GET /api/admin/students/{studentId} → calls getStudentById
   ADD_METHOD_TO(StudentController::getStudentById, "/api/admin/students/{studentId}", drogon::Get);

   ADD_METHOD_TO(StudentController::deleteStudent, "/api/admin/students/{studentId}", drogon::Delete);
   // Route: PUT /api/admin/students/{studentId}/status → calls updateEnrollmentStatus
   ADD_METHOD_TO(StudentController::updateEnrollmentStatus, "/api/admin/students/{studentId}/status", drogon::Put);
ADD_METHOD_TO(StudentController::getStudentCourses, "/api/admin/students/{studentId}/courses", drogon::Get);
    ADD_METHOD_TO(StudentController::enrollStudentInCourse, "/api/admin/students/{studentId}/enrollments", drogon::Post);
    ADD_METHOD_TO(StudentController::getStudentGrades, "/api/admin/students/{studentId}/grades", drogon::Get);

  
METHOD_LIST_END

   // Handler
   void createStudent(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

   // Handles GET request to return all students as JSON
   void getAllStudents(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);
   // Get student details by ID
   void getStudentById(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       std::string studentId);
   // Update student's enrollment status
   void updateEnrollmentStatus(const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                               std::string studentId);

   // In StudentController.h
   void deleteStudent(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                   std::string studentId); // match .cpp

void getStudentCourses(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                           std::string studentId);

    void enrollStudentInCourse(const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                               std::string studentId);

    void getStudentGrades(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                          std::string studentId);

   // TODO: In the next story - We may need to add another function that checks for data duplicates before inserting data into DB.

};

   


