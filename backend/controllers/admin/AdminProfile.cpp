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

// Update admin profile
void AdminProfile::updateProfile(const HttpRequestPtr& req,
                                 std::function<void(const HttpResponsePtr&)>&& callback) {
    std::string role = req->getCookie("user_role");
    if (role != "admin") {
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
        callback(HttpResponse::newHttpJsonResponse(err));
        return;
    }

    const Json::Value& body = *jsonPtr;

    // Extract fields
    std::string full_name = body.get("full_name", "").asString();
    std::string email = body.get("email", "").asString();
    std::string phone = body.get("phone", "").asString();
    std::string address = body.get("address", "").asString();
    std::string password = body.get("password", "").asString();

    // Validation
    if (full_name.empty() || email.empty()) {
        Json::Value err;
        err["error"] = "Name and email are required";
        callback(HttpResponse::newHttpJsonResponse(err));
        return;
    }

    if (phone.empty()) {
        Json::Value err;
        err["error"] = "Phone number cannot be empty";
        callback(HttpResponse::newHttpJsonResponse(err));
        return;
    }

    // Split name
    std::string first_name, last_name;
    size_t spacePos = full_name.find(' ');
    if (spacePos != std::string::npos) {
        first_name = full_name.substr(0, spacePos);
        last_name = full_name.substr(spacePos + 1);
    } else {
        first_name = full_name;
        last_name = "";
    }

    // Username from cookie/session
    std::string username = req->getCookie("username");
    if (username.empty()) {
        auto sess = req->session();
        if (sess) username = sess->get<std::string>("username");
    }

    if (username.empty()) {
        Json::Value err;
        err["error"] = "User not authenticated";
        callback(HttpResponse::newHttpJsonResponse(err));
        return;
    }

    auto client = app().getDbClient();

    // SQL depending on password
    if (!password.empty()) {
        Hasher hasher(HashConfig{3, 64ull * 1024 * 1024});
        std::string hashed = hasher.hash(password);

        std::string sql =
            "UPDATE Users SET first_name=?, last_name=?, email=?, phone=?, address=?, "
            "hashed_password=? "
            "WHERE username=? AND role='admin'";

        client->execSqlAsync(
            sql,
            [callback](const Result& r) {
                Json::Value ok;
                ok["message"] = (r.affectedRows() == 0) ? "No changes were made"
                                                        : "Profile updated successfully";
                callback(HttpResponse::newHttpJsonResponse(ok));
            },
            [callback](const DrogonDbException& e) {
                Json::Value err;
                err["error"] = std::string("Database error: ") + e.base().what();
                callback(HttpResponse::newHttpJsonResponse(err));
            },
            first_name, last_name, email, phone, address, hashed, username);
    } else {
        std::string sql =
            "UPDATE Users SET first_name=?, last_name=?, email=?, phone=?, address=? "
            "WHERE username=? AND role='admin'";

        client->execSqlAsync(
            sql,
            [callback](const Result& r) {
                Json::Value ok;
                ok["message"] = (r.affectedRows() == 0) ? "No changes were made"
                                                        : "Profile updated successfully";
                callback(HttpResponse::newHttpJsonResponse(ok));
            },
            [callback](const DrogonDbException& e) {
                Json::Value err;
                err["error"] = std::string("Database error: ") + e.base().what();
                callback(HttpResponse::newHttpJsonResponse(err));
            },
            first_name, last_name, email, phone, address, username);
    }
}
