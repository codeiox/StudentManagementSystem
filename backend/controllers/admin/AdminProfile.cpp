#include "AdminProfile.h"

using namespace drogon;
using namespace drogon::orm;

void AdminProfile::getProfile(const HttpRequestPtr& req,
                              std::function<void(const HttpResponsePtr&)>&& callback) {
    // Get role from cookie (same as serveProtectedFile)
    std::string role = req->getCookie("user_role");
    if (role.empty()) {
        LOG_ERROR << "role is empty";
        Json::Value ret;
        ret["error"] = "Not logged in";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k401Unauthorized);
        callback(resp);
        return;
    }

    if (role != "admin") {
        LOG_ERROR << "Access denied for role: " << role;
        Json::Value ret;
        ret["error"] = "Admin access required";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k403Forbidden);
        callback(resp);
        return;
    }

    // Get username from cookie or session
    std::string username = req->getCookie("username");
    if (username.empty()) {
        // Fallback to session
        auto session = req->session();
        if (session) {
            username = session->get<std::string>("username");
        }
    }

    // Query DB for admin profile
    auto client = app().getDbClient();
    std::string sql =
        "SELECT first_name, last_name, dob, email, phone, address, username FROM Users WHERE "
        "username = ? AND "
        "role = "
        "'admin'";

    client->execSqlAsync(
        sql,
        [callback](const Result& r) {
            if (r.empty()) {
                LOG_ERROR << "Admin not found in database";
                Json::Value ret;
                ret["error"] = "Admin not found";
                auto resp = HttpResponse::newHttpJsonResponse(ret);
                resp->setStatusCode(k404NotFound);
                callback(resp);
                return;
            }

            // Build JSON object
            Json::Value profile;
            profile["full_name"] =
                r[0]["first_name"].as<std::string>() + " " + r[0]["last_name"].as<std::string>();
            profile["dob"] = r[0]["dob"].as<std::string>();
            profile["email"] = r[0]["email"].as<std::string>();
            profile["phone"] = r[0]["phone"].as<std::string>();
            profile["address"] = r[0]["address"].as<std::string>();
            profile["username"] = r[0]["username"].as<std::string>();
            profile["role"] = "admin";

            auto resp = HttpResponse::newHttpJsonResponse(profile);
            callback(resp);
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "DB error: " << e.base().what();
            Json::Value ret;
            ret["error"] = "Database error";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        username);
}

// Update Admin Profile
void AdminProfile::updateProfile(const HttpRequestPtr& req,
                                 std::function<void(const HttpResponsePtr&)>&& callback) {
    std::string role = req->getCookie("user_role");
    if (role != "admin") {
        LOG_ERROR << "Access denied for role: " << role;
        Json::Value ret;
        ret["error"] = "Admin access required";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k403Forbidden);
        callback(resp);
        return;
    }

    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        Json::Value err;
        err["error"] = "Invalid JSON";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }
    const Json::Value& body = *jsonPtr;

    // Extract fields
    std::string full_name = body.get("full_name", "").asString();

    // Split first_name and last_name
    std::string first_name, last_name;
    size_t spacePos = full_name.find(' ');
    if (spacePos != std::string::npos) {
        first_name = full_name.substr(0, spacePos);
        last_name = full_name.substr(spacePos + 1);
    } else {
        first_name = full_name;
        last_name = "";
    }

    std::string email = body.get("email", "").asString();
    std::string phone = body.get("phone", "").asString();
    std::string address = body.get("address", "").asString();
    std::string password = body.get("password", "").asString();

    // Basic validation
    if (full_name.empty() || email.empty()) {
        Json::Value error;
        error["error"] = "Name and email are required";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // Get username
    std::string username = req->getCookie("username");
    if (username.empty()) {
        auto sess = req->session();
        if (sess) {
            username = sess->get<std::string>("username");
        }
    }

    if (username.empty()) {
        Json::Value error;
        error["error"] = "User not authenticated";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k401Unauthorized);
        callback(resp);
        return;
    }

    auto client = app().getDbClient();

    // Handle password update separately if provided
    if (!password.empty()) {
        Hasher hasher(HashConfig{3, 64ull * 1024 * 1024});
        std::string hashed = hasher.hash(password);
        password.clear();  // Clear password from memory

        std::string sql =
            "UPDATE Users SET first_name = ?, last_name = ?, email = ?, phone = ?, address = ?, "
            "hashed_password = ? WHERE username = ? AND role = 'admin'";

        LOG_DEBUG << "SQL Query (with password): " << sql;

        client->execSqlAsync(
            sql,
            [callback](const drogon::orm::Result& r) {
                if (r.affectedRows() == 0) {
                    Json::Value err;
                    err["error"] = "Admin user not found or no changes made";
                    auto resp = HttpResponse::newHttpJsonResponse(err);
                    resp->setStatusCode(k404NotFound);
                    callback(resp);
                    return;
                }
                Json::Value ok;
                ok["message"] = "Profile updated successfully";
                auto resp = HttpResponse::newHttpJsonResponse(ok);
                callback(resp);
            },
            [callback](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Update error: " << e.base().what();
                Json::Value err;
                err["error"] = std::string("Database error: ") + e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            first_name, last_name, email, phone, address, hashed, username);
    } else {
        // Update without password
        std::string sql =
            "UPDATE Users SET first_name = ?, last_name = ?, email = ?, phone = ?, address = ? "
            "WHERE username = ? AND role = 'admin'";

        LOG_DEBUG << "SQL Query (without password): " << sql;

        client->execSqlAsync(
            sql,
            [callback](const drogon::orm::Result& r) {
                if (r.affectedRows() == 0) {
                    Json::Value err;
                    err["error"] = "Admin user not found or no changes made";
                    auto resp = HttpResponse::newHttpJsonResponse(err);
                    resp->setStatusCode(k404NotFound);
                    callback(resp);
                    return;
                }
                Json::Value ok;
                ok["message"] = "Profile updated successfully";
                auto resp = HttpResponse::newHttpJsonResponse(ok);
                callback(resp);
            },
            [callback](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Update error: " << e.base().what();
                Json::Value err;
                err["error"] = std::string("Database error: ") + e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            first_name, last_name, email, phone, address, username);
    }
}