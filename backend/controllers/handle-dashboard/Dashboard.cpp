//
// Created by CODEX on 10/27/25.
//

#include "Dashboard.h"

void Dashboard::getStudentAtRisk(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    // do something
    LOG_INFO << "Getting student at risk from DB.\n";
    auto client = drogon::app().getDbClient("default");

    // Execute SQL query here:
    client->execSqlAsync(
        "SELECT first_name, last_name, student_id FROM Users WHERE "
        "enrollment_status = 'probation'",
        [callback](const drogon::orm::Result& result) {
            Json::Value jsonResponse(Json::arrayValue);

            for (const auto& row : result) {
                Json::Value student;
                student["first_name"] = row["first_name"].as<std::string>();
                student["last_name"] = row["last_name"].as<std::string>();
                student["student_id"] = row["student_id"].as<std::string>();
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
