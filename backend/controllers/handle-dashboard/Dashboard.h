//
// Created by CODEX on 10/27/25.
//

#ifndef STUDENTMANAGEMENTSYSTEM_DASHBOARD_H
#define STUDENTMANAGEMENTSYSTEM_DASHBOARD_H
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>
#include <string>

class Dashboard : public drogon::HttpController<Dashboard> {
   public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Dashboard::getStudentAtRisk, "/api/students/probation", drogon::Get);
    ADD_METHOD_TO(Dashboard::getTotalStudent, "/api/students/total", drogon::Get);
    ADD_METHOD_TO(Dashboard::getStudentStatus, "/api/students/status", drogon::Get);
    METHOD_LIST_END

   private:
    // TODO: Load all data from the database into vector and display on dashboard based on criteria
    void getStudentAtRisk(const drogon::HttpRequestPtr& req,
                          std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void getTotalStudent(const drogon::HttpRequestPtr& req,
                         std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getStudentStatus(const drogon::HttpRequestPtr& req,
                          std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

#endif  // STUDENTMANAGEMENTSYSTEM_DASHBOARD_H
