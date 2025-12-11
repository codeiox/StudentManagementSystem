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
   // Route: GET /api/admin/students/{studentId}/courses → calls getStudentCourses
   ADD_METHOD_TO(StudentController::getStudentCourses, "/api/admin/students/{studentId}/courses", drogon::Get);
   // Route: POST /api/admin/students/{studentId}/enrollments → calls enrollStudentInCourse
   ADD_METHOD_TO(StudentController::enrollStudentInCourse, "/api/admin/students/{studentId}/enrollments", drogon::Post);
   // Route: GET /api/admin/students/{studentId}/grades → calls getStudentGrades
   ADD_METHOD_TO(StudentController::getStudentGrades, "/api/admin/students/{studentId}/grades", drogon::Get);
   // Route: GET /api/admin/students/{studentId}/program → calls getStudentProgram
   ADD_METHOD_TO(StudentController::getStudentProgram, "/api/admin/students/{studentId}/program", drogon::Get);
   // Route: GET /api/admin/students/{studentId}/enrollment-details → calls getEnrollmentDetails
   ADD_METHOD_TO(StudentController::getEnrollmentDetails, "/api/admin/students/{studentId}/enrollment-details", drogon::Get);
   // Route: POST /api/admin/students/{studentId}/enrollment-details → calls createEnrollmentDetails
   ADD_METHOD_TO(StudentController::createEnrollmentDetails, "/api/admin/students/{studentId}/enrollment-details", drogon::Post);
   // Route: GET /api/admin/students/{studentId}/advising-notes → calls getAdvisingNotes
   ADD_METHOD_TO(StudentController::getAdvisingNotes, "/api/admin/students/{studentId}/advising-notes", drogon::Get);
   // Route: POST /api/admin/students/{studentId}/advising-notes → calls createAdvisingNotes
   ADD_METHOD_TO(StudentController::createAdvisingNotes, "/api/admin/students/{studentId}/advising-notes", drogon::Post);
   // Route: DELETE /api/admin/students/{studentId}/advising-notes/{noteId} → calls deleteAdvisingNote
   ADD_METHOD_TO(StudentController::deleteAdvisingNote, "/api/admin/students/{studentId}/advising-notes/{noteId}", drogon::Delete);

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

   // Fetch all courses for a student (GET)
   void getStudentCourses(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                          std::string studentId);

   // Enroll a student in a course (POST)
   void enrollStudentInCourse(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              std::string studentId);

   // Fetch grades for a student (GET)
   void getStudentGrades(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                         std::string studentId);

   // Fetch program/major details for a student (GET)
   void getStudentProgram(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                          std::string studentId);

   // Fetch enrollment details including history (GET)
   void getEnrollmentDetails(const drogon::HttpRequestPtr &req,
                             std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                             std::string studentId);

   // Create or update enrollment details (POST)
   void createEnrollmentDetails(const drogon::HttpRequestPtr &req,
                                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                std::string studentId);

   // Fetch advising notes for a student (GET)
   void getAdvisingNotes(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                         std::string studentId);

   // Add an advising note for a student (POST)
   void createAdvisingNotes(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                            std::string studentId);

   // Delete an advising note by ID (DELETE)
   void deleteAdvisingNote(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                           std::string studentId, std::string noteId);
   // TODO: In the next story - We may need to add another function that checks for data duplicates before inserting data into DB.
};
