#ifndef ADMIN_PROFILE_H
#define ADMIN_PROFILE_H

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>

#include <any>
#include <iostream>
#include <string>
#include <vector>

#include "../auth/Hasher.h"

class AdminProfile : public drogon::HttpController<AdminProfile> {
   public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AdminProfile::getProfile, "/api/admin/profile", drogon::Get);
    ADD_METHOD_TO(AdminProfile::updateProfile, "/api/admin/profile/update", drogon::Put);
    METHOD_LIST_END

   private:
    void getProfile(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void updateProfile(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
#endif  // ADMIN_PROFILE_H
