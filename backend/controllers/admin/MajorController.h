#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <drogon/HttpRequest.h>
#include <json/json.h>
#include <string>

class MajorController : public drogon::HttpController<MajorController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(MajorController::createMajor, "/api/admin/majors", drogon::Post);
    ADD_METHOD_TO(MajorController::getAllMajors, "/api/admin/majors", drogon::Get);
    ADD_METHOD_TO(MajorController::getMajorById, "/api/admin/majors/{majorId}", drogon::Get);
    ADD_METHOD_TO(MajorController::updateMajor, "/api/admin/majors/{majorId}", drogon::Put);
    ADD_METHOD_TO(MajorController::deleteMajor, "/api/admin/majors/{majorId}", drogon::Delete);
    METHOD_LIST_END

    void createMajor(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getAllMajors(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getMajorById(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      std::string majorId);

    void updateMajor(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     std::string majorId);

    void deleteMajor(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     std::string majorId);
};
