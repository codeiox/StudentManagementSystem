#include "StudentController.h"

using namespace drogon;

void StudentController::createStudent(const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid JSON");
        callback(resp);
        return;
    }
    // for debugging
    LOG_INFO << "Received JSON: " << json->toStyledString();

    // Validate required fields
    // !important- TODO: Needs improvement! Doesn't handle the multi data model
    if (!json->isMember("firstName") || !json->isMember("lastName") || !json->isMember("email") ||
        !json->isMember("studentId") || !json->isMember("phone") ||
        !json->isMember("dateofbirth") || !json->isMember("address") || !json->isMember("sex")) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields");
        callback(resp);
        return;
    }
    // Otherwise response with 200 OK
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k200OK);
    resp->setBody("Request received successfully");
    callback(resp);

    // Extract and validate fields
    std::string firstName = json->get("firstName", "").asString();
    std::string lastName = json->get("lastName", "").asString();
    std::string email = json->get("email", "").asString();
    std::string studentId = json->get("studentId", "").asString();
    std::string phone = json->get("phone", "").asString();
    std::string dateOfBirth = json->get("dateofbirth", "").asString();
    std::string address = json->get("address", "").asString();
    std::string sex = json->get("sex", "").asString();
    std::string username = json->get("username", "").asString();
    //std::string password = json->get("password", "").asString();

    // Basic validation for empty fields
    if (firstName.empty() || lastName.empty() || email.empty() || studentId.empty() ||
        phone.empty() || dateOfBirth.empty() || address.empty() || sex.empty() || (json->get("password", "").asString()).empty()) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Fields cannot be empty");
        callback(resp);
        return;
    }
    try {
        // Hash password here as soon as received from client-side
        //Hasher hasher(HashConfig{1, crypto_pwhash_MEMLIMIT_MIN});
        Hasher hasher(HashConfig{4, 1ull * 1024 * 1024 * 1024});
        std::string hashedPassword = hasher.hash(json->get("password", "").asString());
        if (hashedPassword.empty()) {
            throw std::runtime_error("Password hashing failed");
        }

        //  Call the StoreCredential class to construct object and store in DB
        StoreCredential storeCredential(firstName, lastName, dateOfBirth, email, phone, address,
                                        sex, studentId, username, hashedPassword);

        storeCredential.storeToDB();  // call storeToDB method to store info to database

    } catch (const std::exception& e) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Error: " + std::string(e.what()));
        callback(resp);
    }
}
