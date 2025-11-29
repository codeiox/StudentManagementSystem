//
// Created by CODEX on 10/27/25.
//

#include "Dashboard.h"

#include "CalculateGPA.h"

void Dashboard::getStudentAtRisk(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    // do something
    LOG_INFO << "Getting student at risk from DB.\n";
    auto client = drogon::app().getDbClient("default");

    // Execute SQL query here:
    client->execSqlAsync(
        "SELECT first_name, last_name, student_id, overall_gpa FROM Users WHERE "
        "enrollment_status = 'probation'",
        [callback](const drogon::orm::Result& result) {
            Json::Value jsonResponse(Json::arrayValue);

            for (const auto& row : result) {
                Json::Value student;
                student["first_name"] = row["first_name"].as<std::string>();
                student["last_name"] = row["last_name"].as<std::string>();
                student["student_id"] = row["student_id"].as<std::string>();
                student["overall_gpa"] = row["overall_gpa"].as<double>();

                // TODO: Request student major from major table

                jsonResponse.append(student);
            }

            auto resp = drogon::HttpResponse::newHttpJsonResponse(jsonResponse);
            resp->setStatusCode(drogon::k200OK);
            LOG_INFO << "Fetched " << result.size() << " at-risk students from database.";
            callback(resp);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        });
}

void Dashboard::getTotalStudent(const drogon::HttpRequestPtr& req,
                                std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    // do something
    // SELECT COUNT(*) FROM table_name;

    auto client = drogon::app().getDbClient("default");
    client->execSqlAsync(
        "SELECT COUNT(*) AS total_students FROM Users WHERE role = 'student'",
        [callback](const drogon::orm::Result& result) {
            int totalStudents = 0;
            if (!result.empty()) {
                totalStudents = result[0]["total_students"].as<int>();
            }

            Json::Value jsonResponse;
            jsonResponse["total_students"] = totalStudents;

            auto resp = drogon::HttpResponse::newHttpJsonResponse(jsonResponse);
            resp->setStatusCode(drogon::k200OK);
            callback(resp);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        });
}

void Dashboard::getStudentStatus(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    // do something
    // TODO: retrive active, graduated, pending, probation, and suspended of current year

    auto client = drogon::app().getDbClient("default");
    std::string sql = R"(
        SELECT
            SUM(CASE WHEN enrollment_status = 'active'     AND EXTRACT(YEAR FROM created_at) = EXTRACT(YEAR FROM NOW()) THEN 1 ELSE 0 END) AS active,
            SUM(CASE WHEN enrollment_status = 'graduated'  AND EXTRACT(YEAR FROM created_at) = EXTRACT(YEAR FROM NOW()) THEN 1 ELSE 0 END) AS graduated,
            SUM(CASE WHEN enrollment_status = 'pending'    AND EXTRACT(YEAR FROM created_at) = EXTRACT(YEAR FROM NOW()) THEN 1 ELSE 0 END) AS pending,
            SUM(CASE WHEN enrollment_status = 'probation'  AND EXTRACT(YEAR FROM created_at) = EXTRACT(YEAR FROM NOW()) THEN 1 ELSE 0 END) AS probation,
            SUM(CASE WHEN enrollment_status = 'suspended'  AND EXTRACT(YEAR FROM created_at) = EXTRACT(YEAR FROM NOW()) THEN 1 ELSE 0 END) AS suspended
        FROM Users
        WHERE role = 'student'
    )";

    client->execSqlAsync(
        sql,
        [callback](const drogon::orm::Result& result) {
            Json::Value jsonResponse;

            // Default to 0
            jsonResponse["active"] = 0;
            jsonResponse["graduated"] = 0;
            jsonResponse["pending"] = 0;
            jsonResponse["probation"] = 0;
            jsonResponse["suspended"] = 0;

            if (!result.empty()) {
                jsonResponse["active"] = result[0]["active"].as<int>();
                jsonResponse["graduated"] = result[0]["graduated"].as<int>();
                jsonResponse["pending"] = result[0]["pending"].as<int>();
                jsonResponse["probation"] = result[0]["probation"].as<int>();
                jsonResponse["suspended"] = result[0]["suspended"].as<int>();
            }

            auto resp = drogon::HttpResponse::newHttpJsonResponse(jsonResponse);
            callback(resp);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = std::string(e.base().what());
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            callback(resp);
        });
}

void Dashboard::getStudentCurrentGrades(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    LOG_INFO << "Requesting DB for a student grades\n";
    auto client = drogon::app().getDbClient("default");
    std::string studentId = req->getParameter("id");

    LOG_INFO << "Student ID from request: " << studentId;

    client->execSqlAsync(
        "SELECT e.grade, e.course_credits "
        "FROM Enrollments e "
        "JOIN Users u ON e.user_id = u.id "
        "WHERE e.status = 'completed' AND u.student_id = ?",
        [callback, studentId](const drogon::orm::Result& result) {
            LOG_INFO << "SELECT query returned " << result.size() << " rows";
            // insert each grade and course_credits into vector as pair string and int for overall
            // GPA calculation
            std::vector<std::pair<std::string, int>> letter_grades;

            // loop over the row
            for (const auto& row : result) {
                // convert into string
                letter_grades.emplace_back(row["grade"].as<std::string>(),
                                           row["course_credits"].as<int>());
            }

            // Avoid crashing if student has zero completed courses
            if (letter_grades.empty()) {
                LOG_INFO << "No completed courses found for student";
                Json::Value resp;
                resp["status"] = "no_completed_courses";
                resp["gpa"] = 0.0;
                resp["earned_credits"] = 0;
                callback(drogon::HttpResponse::newHttpJsonResponse(resp));
                return;
            }

            const double GPA_THRESHOLD = 1.7;
            const int CREDIT_THRESHOLD = 12;

            // call GPA calculation function with vector pair<std::string, int>
            double overallGPA = calculateOverAllGPA(letter_grades);
            // call Total credit function
            int earnedCredits = calculateTotalCredits(letter_grades);

            LOG_INFO << "Calculated GPA: " << overallGPA << ", Earned Credits: " << earnedCredits;
            LOG_INFO << "GPA Threshold: " << GPA_THRESHOLD
                     << ", Credit Threshold: " << CREDIT_THRESHOLD;

            if (overallGPA < GPA_THRESHOLD && earnedCredits > CREDIT_THRESHOLD) {
                LOG_INFO << "Student meets probation criteria - updating to probation";
                // change the enrollment_status from Users table to "probation"
                auto client2 = drogon::app().getDbClient("default");
                client2->execSqlAsync(
                    "UPDATE Users SET enrollment_status = 'probation', overall_gpa = ? WHERE "
                    "student_id = ?",

                    // SUCCESS UPDATE
                    [callback, overallGPA, earnedCredits](const drogon::orm::Result& result) {
                        LOG_INFO << "UPDATE successful. Rows affected: " << result.affectedRows();

                        Json::Value resp;
                        resp["status"] = "updated_to_probation";
                        resp["gpa"] = overallGPA;
                        resp["earned_credits"] = earnedCredits;
                        callback(drogon::HttpResponse::newHttpJsonResponse(resp));
                    },
                    // FAILURE UPDATE
                    [callback](const drogon::orm::DrogonDbException& e) {
                        Json::Value err;
                        err["error"] = e.base().what();
                        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                        resp->setStatusCode(drogon::k500InternalServerError);
                        callback(resp);
                    },
                    overallGPA,  // ?
                    studentId    // ?
                );
            } else {
                LOG_INFO << "Update Only overall GPA\n";
                auto client3 = drogon::app().getDbClient("default");

                client3->execSqlAsync(
                    "UPDATE Users SET enrollment_status = 'pending', overall_gpa = ? WHERE "
                    "student_id = ?",

                    // SUCCESS UPDATE
                    [callback, overallGPA](const drogon::orm::Result& result) {
                        LOG_INFO << "UPDATE successful. Rows affected: " << result.affectedRows();

                        Json::Value resp;
                        resp["status"] = "Overall GPA updated";
                        resp["gpa"] = overallGPA;
                        callback(drogon::HttpResponse::newHttpJsonResponse(resp));
                    },
                    // FAILURE UPDATE
                    [callback](const drogon::orm::DrogonDbException& e) {
                        Json::Value err;
                        err["error"] = e.base().what();
                        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
                        resp->setStatusCode(drogon::k500InternalServerError);
                        callback(resp);
                    },
                    overallGPA,  // ?
                    studentId    // ?
                );

                Json::Value resp;
                resp["status"] = "ok";
                resp["gpa"] = overallGPA;
                resp["earned_credits"] = earnedCredits;
                callback(drogon::HttpResponse::newHttpJsonResponse(resp));
            }
        },
        // ERROR LAMBDA FOR SELECT
        [callback](const drogon::orm::DrogonDbException& e) {
            Json::Value err;
            err["error"] = std::string(e.base().what());
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k500InternalServerError);
            callback(resp);
        },
        studentId  // ? for SELECT query
    );
}
