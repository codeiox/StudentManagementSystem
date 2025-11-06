#include "StudentController.h"

using namespace drogon;

void StudentController::createStudent(const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        LOG_ERROR << "Failed to parse JSON from request body\n";
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
        !json->isMember("phone") || !json->isMember("dateofbirth") || !json->isMember("address") ||
        !json->isMember("gender") || !json->isMember("password")) {
        LOG_ERROR << "Missing required fields in JSON\n";
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields");
        callback(resp);
        return;
    }
    // Extract and validate fields
    std::string firstName = json->get("firstName", "").asString();
    std::string lastName = json->get("lastName", "").asString();
    std::string dateOfBirth = json->get("dateofbirth", "").asString();
    std::string email = json->get("email", "").asString();
    std::string phone = json->get("phone", "").asString();
    std::string address = json->get("address", "").asString();
    std::string gender = json->get("gender", "").asString();
    std::string studentId = json->get("studentId", "").asString();
    std::string username = json->get("username", "").asString();
    std::string role = json->get("role", "").asString();
    std::string password = json->get("password", "").asString();
    LOG_INFO << "Extracted all fields from JSON\n";

    // converts role to lower case using std::transform and lambda function
    std::transform(role.begin(), role.end(), role.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    LOG_INFO << "Converted role to lowercase: " << role << "\n";

    // Basic validation for empty fields
    if (firstName.empty() || lastName.empty() || email.empty() ||
        (role == "student" && studentId.empty()) || phone.empty() || dateOfBirth.empty() ||
        address.empty() || gender.empty() || role.empty() || password.empty()) {
        LOG_INFO << "One or more required fields are empty\n";
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Fields cannot be empty");
        callback(resp);
        return;
    }
    LOG_INFO << "All required fields are present and non-empty\n";

    // Validate role value
    if (role != "student" && role != "admin") {
        LOG_INFO << "Invalid role value: " << role << "\n";
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid role value");
        callback(resp);
        return;
    }
    LOG_INFO << "Role value is valid: " << role << "\n";

    try {
        auto start = std::chrono::steady_clock::now();
        // Hash password
        LOG_INFO << "Starting password hashing with opslimit=3, memlimit=64 bytes";
        Hasher hasher(HashConfig{3, 64ull * 1024 * 1024});
        std::string hashedPassword;
        try {
            hashedPassword = hasher.hash(password);
            password = "";  // Clear plaintext password from memory
        } catch (const std::exception& e) {
            LOG_ERROR << "Password hashing failed: " << e.what();
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Error: Password hashing failed");
            callback(resp);
            return;
        }
        auto end = std::chrono::steady_clock::now();
        LOG_INFO << "Password hashing took: " << std::chrono::duration<double>(end - start).count()
                 << " seconds";
        if (hashedPassword.empty()) {
            LOG_ERROR << "Password hashing produced empty result";
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Error: Password hashing failed");
            callback(resp);
            return;
        }
        LOG_INFO << "Password hashed successfully";

        // Create StoreCredential object
        StoreCredential storeCredential(firstName, lastName, dateOfBirth, email, phone, address,
                                        gender, studentId, username, hashedPassword, role);
        LOG_INFO << "Successfully created StoreCredential object";

        // Store in database
        LOG_INFO << "Calling storeToDB for user: " << email;
        storeCredential.storeToDB(req, std::move(callback));
    } catch (const std::exception& e) {
        LOG_ERROR << "Exception occurred while creating student: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Error creating student: " + std::string(e.what()));
        callback(resp);
    }
}

// Handles GET request to retrieve all student records from the database
void StudentController::getAllStudents(const HttpRequestPtr& req,
                                       std::function<void(const HttpResponsePtr&)>&& callback) {
    LOG_INFO << "Getting ready to Fetch students from DB..\n";
    // Get the default database client instance
    auto client = app().getDbClient("default");

    // Execute SQL query asynchronously to fetch student data
    client->execSqlAsync(
        "SELECT id, first_name, last_name, dob, email, phone, address, gender, student_id, "
        "enrollment_status FROM Users WHERE role = 'student'",

        // Success callback: transform DB result into JSON array
        [callback](const drogon::orm::Result& result) {
            Json::Value jsonResponse(Json::arrayValue);

            // Iterate through each row and build a JSON object for each student
            for (const auto& row : result) {
                Json::Value student;
                student["first_name"] = row["first_name"].as<std::string>();
                student["last_name"] = row["last_name"].as<std::string>();
                student["dob"] = row["dob"].as<std::string>();
                student["email"] = row["email"].as<std::string>();
                student["phone"] = row["phone"].as<std::string>();
                student["address"] = row["address"].as<std::string>();
                student["gender"] = row["gender"].as<std::string>();
                student["student_id"] = row["student_id"].as<std::string>();
                student["enrollmentStatus"] =
                    row["enrollment_status"].as<std::string>();  // May be null
                jsonResponse.append(student);
            }

            // Return JSON response with HTTP 200 OK
            auto resp = HttpResponse::newHttpJsonResponse(jsonResponse);
            resp->setStatusCode(k200OK);
            LOG_INFO << "Fetched " << result.size() << " students from database.";
            callback(resp);
            LOG_INFO << "Fetched students and sent response.\n";
        },

        // Error callback: return HTTP 500 with error message
        [callback](const drogon::orm::DrogonDbException& e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        });
}

// Handles GET request to fetch student details by ID
void StudentController::getStudentById(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string studentId) {
    // Get database client
    auto client = drogon::app().getDbClient("default");

    // Execute SQL query to fetch student data
    client->execSqlAsync(
        "SELECT first_name, last_name, dob, email, phone, address, gender, student_id, "
        "enrollment_status FROM Users WHERE student_id = ?",
        [callback](const drogon::orm::Result& result) {
            // If no student found, return 404
            if (result.empty()) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k404NotFound);
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->setBody(R"({"error":"Student not found"})");
                callback(resp);
                return;
            }

            // Convert result row to JSON
            const auto& row = result[0];
            Json::Value student;
            student["first_name"] = row["first_name"].as<std::string>();
            student["last_name"] = row["last_name"].as<std::string>();
            student["dob"] = row["dob"].as<std::string>();
            student["email"] = row["email"].as<std::string>();
            student["phone"] = row["phone"].as<std::string>();
            student["address"] = row["address"].as<std::string>();
            student["gender"] = row["gender"].as<std::string>();
            student["student_id"] = row["student_id"].as<std::string>();
            student["enrollmentStatus"] = row["enrollment_status"].as<std::string>();

            // Return student JSON with 200 OK
            auto resp = drogon::HttpResponse::newHttpJsonResponse(student);
            callback(resp);
        },
        // Handle database error
        [callback](const drogon::orm::DrogonDbException& e) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        studentId  // SQL parameter
    );
}

// Handles PUT request to update a student's enrollment status
void StudentController::updateEnrollmentStatus(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string studentId) {
    // Parse JSON body
    auto json = req->getJsonObject();
    if (!json || !json->isMember("enrollmentStatus")) {
        // Missing field — return 400 Bad Request
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Missing enrollmentStatus field");
        callback(resp);
        return;
    }

    // Extract and validate new status
    std::string newStatus = json->get("enrollmentStatus", "").asString();
    std::set<std::string> validStatuses = {"active", "graduated", "probation", "suspended",
                                           "inactive"};

    if (validStatuses.find(newStatus) == validStatuses.end()) {
        // Invalid value — return 400 Bad Request
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Invalid status value");
        callback(resp);
        return;
    }

    // Get database client
    auto client = drogon::app().getDbClient("default");

    // Execute SQL update
    client->execSqlAsync(
        "UPDATE Users SET enrollment_status = ? WHERE student_id = ?",
        [callback, newStatus](const drogon::orm::Result& result) {
            // Return success response
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k200OK);
            resp->setBody("Enrollment status updated to: " + newStatus);
            callback(resp);
        },
        // Handle database error
        [callback](const drogon::orm::DrogonDbException& e) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        newStatus, studentId  // SQL parameters
    );
}
