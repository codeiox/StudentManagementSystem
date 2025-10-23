#include "StudentController.h"

using namespace drogon;

void StudentController::createStudent(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid JSON");
        callback(resp);
        return;
    }
    //     // for debugging
    LOG_INFO << "Received JSON: " << json->toStyledString();

    // Validate required fields
    // !important- TODO: Needs improvement! Doesn't handle the multi data model
    if (!json->isMember("firstName") || !json->isMember("lastName") || !json->isMember("email") ||
        !json->isMember("studentId") || !json->isMember("phone") ||
        !json->isMember("dateofbirth") || !json->isMember("address") || !json->isMember("gender"))
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields");
        callback(resp);
        return;
    }
    // Otherwise response with 200 OK
    // auto resp = HttpResponse::newHttpResponse();
    // resp->setStatusCode(k200OK);
    // resp->setBody("Request received successfully");
    // callback(resp);

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
    std::string enrollmentStatus = "Pending"; // Default status

    // Basic validation for empty fields
    if (firstName.empty() || lastName.empty() || email.empty() || studentId.empty() ||
        phone.empty() || dateOfBirth.empty() || address.empty() || gender.empty() || (json->get("password", "").asString()).empty())
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Fields cannot be empty");
        callback(resp);
        return;
    }
    try
    {

        auto start = std::chrono::steady_clock::now();
        // Hash password here as soon as received from client-side
        Hasher hasher(HashConfig{1, crypto_pwhash_MEMLIMIT_MIN});
        // Hasher hasher(HashConfig{4, 1ull * 1024 * 1024 * 1024});
        std::string hashedPassword = hasher.hash(json->get("password", "").asString());
        auto end = std::chrono::steady_clock::now();
        LOG_INFO << "Operation took: " << std::chrono::duration<double>(end - start).count() << " seconds";


        if (hashedPassword.empty())
        {
            throw std::runtime_error("Password hashing failed");
        }

        //  Call the StoreCredential class to construct object and store in DB
        StoreCredential storeCredential(firstName, lastName, dateOfBirth, email, phone, address,
                                        gender, studentId, username, hashedPassword, enrollmentStatus);

        storeCredential.storeToDB(); // call storeToDB method to store info to database

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setBody("Student created successfully");
        callback(resp);
    }
    catch (const std::exception &e)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Error: " + std::string(e.what()));
        callback(resp);
    }
}

// Handles GET request to retrieve all student records from the database
void StudentController::getAllStudents(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback)
{
    // Get the default database client instance
    auto client = app().getDbClient("default");

    // Execute SQL query asynchronously to fetch student data
    client->execSqlAsync(
        "SELECT id, first_name, last_name, dob, email, phone, address, gender, student_id, enrollment_status FROM Students",

        // Success callback: transform DB result into JSON array
        [callback](const drogon::orm::Result &result)
        {
            Json::Value jsonResponse(Json::arrayValue);

            // Iterate through each row and build a JSON object for each student
            for (const auto &row : result)
            {
                Json::Value student;
                student["first_name"] = row["first_name"].as<std::string>();
                student["last_name"] = row["last_name"].as<std::string>();
                student["dob"] = row["dob"].as<std::string>();
                student["email"] = row["email"].as<std::string>();
                student["phone"] = row["phone"].as<std::string>();
                student["address"] = row["address"].as<std::string>();
                student["gender"] = row["gender"].as<std::string>();
                student["student_id"] = row["student_id"].as<std::string>();
                student["enrollmentStatus"] = row["enrollment_status"].as<std::string>(); // May be null
                jsonResponse.append(student);
            }

            // Return JSON response with HTTP 200 OK
            auto resp = HttpResponse::newHttpJsonResponse(jsonResponse);
            resp->setStatusCode(k200OK);
            LOG_INFO << "Fetched " << result.size() << " students from database.";
            callback(resp);
        },

        // Error callback: return HTTP 500 with error message
        [callback](const drogon::orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        });
}

// Handles GET request to fetch student details by ID
void StudentController::getStudentById(const drogon::HttpRequestPtr &req,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                       std::string studentId)
{
    // Get database client
    auto client = drogon::app().getDbClient("default");

    // Execute SQL query to fetch student data
    client->execSqlAsync(
        "SELECT first_name, last_name, dob, email, phone, address, gender, student_id, enrollment_status FROM Students WHERE student_id = ?",
        [callback](const drogon::orm::Result &result) {
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
            const auto &row = result[0];
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
        [callback](const drogon::orm::DrogonDbException &e) {
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
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string studentId)
{
    // Parse JSON body
    auto json = req->getJsonObject();
    if (!json || !json->isMember("enrollmentStatus"))
    {
        // Missing field — return 400 Bad Request
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Missing enrollmentStatus field");
        callback(resp);
        return;
    }

    // Extract and validate new status
    std::string newStatus = json->get("enrollmentStatus", "").asString();
    std::set<std::string> validStatuses = {"active", "graduated", "probation", "suspended", "inactive"};

    if (validStatuses.find(newStatus) == validStatuses.end())
    {
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
        "UPDATE Students SET enrollment_status = ? WHERE student_id = ?",
        [callback, newStatus](const drogon::orm::Result &result)
        {
            // Return success response
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k200OK);
            resp->setBody("Enrollment status updated to: " + newStatus);
            callback(resp);
        },
        // Handle database error
        [callback](const drogon::orm::DrogonDbException &e)
        {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        newStatus, studentId  // SQL parameters
    );
}
