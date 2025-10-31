#include "StudentController.h"
#include <drogon/utils/Utilities.h>

using namespace drogon;

void StudentController::createStudent(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    // So you can send in array of students through postman
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid JSON");
        callback(resp);
        return;
    }

    // --- Handle multiple students if the JSON is an array ---
    if (json->isArray())
    {
        for (const auto &studentJson : *json)
        {
            // Extract fields from each student
            std::string firstName = studentJson.get("firstName", "").asString();
            std::string lastName  = studentJson.get("lastName", "").asString();
            std::string dateOfBirth = studentJson.get("dateofbirth", "").asString();
            std::string email = studentJson.get("email", "").asString();
            std::string phone = studentJson.get("phone", "").asString();
            std::string address = studentJson.get("address", "").asString();
            std::string gender = studentJson.get("gender", "").asString();
            std::string studentId = studentJson.get("studentId", "").asString();
            std::string username = studentJson.get("username", "").asString();
            std::string role = studentJson.get("role", "").asString();
            std::string password = studentJson.get("password", "").asString();

            // Hash password (same as your existing logic)
            Hasher hasher(HashConfig{3, 64ull * 1024 * 1024});
            std::string hashedPassword = hasher.hash(password);
            password = "";

            // Store in database
            StoreCredential storeCredential(firstName, lastName, dateOfBirth, email, phone, address,
                                            gender, studentId, username, hashedPassword, role);
            storeCredential.storeToDB(req, [](const HttpResponsePtr &){});
        }

        // Respond with a summary
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setBody("Multiple students created successfully");
        callback(resp);
        return;
    }

    // --- Single student logic continues exactly as before ---
    // Do NOT redeclare json
    LOG_INFO << "Received JSON: " << json->toStyledString();

    // Validate required fields
    if (!json->isMember("firstName") || !json->isMember("lastName") || !json->isMember("email") || !json->isMember("phone") ||
        !json->isMember("dateofbirth") || !json->isMember("address") || !json->isMember("gender") || !json->isMember("password"))
    {
        LOG_ERROR << "Missing required fields in JSON\n";
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields");
        callback(resp);
        return;
    }

    // Extract fields
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

        // ---------------- Additional Validation Checks ----------------

// 1. Email validation (simple regex)
std::regex emailRegex(R"((\w+)(\.?\w+)*@(\w+)(\.\w+)+)");
if (!std::regex_match(email, emailRegex)) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid email format");
    callback(resp);
    return;
}

// 2. Date of Birth validation (age between 15 and 65)
try {
    std::tm tm = {};
    std::istringstream ss(dateOfBirth);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid date format. Use YYYY-MM-DD.");
        callback(resp);
        return;
    }

    auto birthTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - birthTime).count() / (24 * 365);
    
    if (age < 15 || age > 65) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Age must be between 15 and 65 years");
        callback(resp);
        return;
    }
}
catch (...) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Error processing date of birth");
    callback(resp);
    return;
}

// 3. Phone number validation and auto-formatting (###-###-####)
std::string digits;
for (char c : phone)
    if (std::isdigit(c)) digits += c;

if (digits.size() != 10) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("Invalid Phone number");
    callback(resp);
    return;
}

// Format as ###-###-####
phone = digits.substr(0,3) + "-" + digits.substr(3,3) + "-" + digits.substr(6,4);



    // converts role to lower case using std::transform and lambda function
    std::transform(role.begin(), role.end(), role.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    LOG_INFO << "Converted role to lowercase: " << role << "\n";

    // Basic validation for empty fields
    if (firstName.empty() || lastName.empty() || email.empty() || (role == "student" && studentId.empty()) ||
        phone.empty() || dateOfBirth.empty() || address.empty() || gender.empty() || role.empty()|| password.empty())
    {
        LOG_INFO << "One or more required fields are empty\n";
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Fields cannot be empty");
        callback(resp);
        return;
    }
    LOG_INFO << "All required fields are present and non-empty\n";

    // Validate role value
    if (role != "student" && role != "admin")
    {
        LOG_INFO << "Invalid role value: " << role << "\n";
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid role value");
        callback(resp);
        return;
    }
    LOG_INFO << "Role value is valid: " << role << "\n";

    try
    {
        auto start = std::chrono::steady_clock::now();
        // Hash password
        LOG_INFO << "Starting password hashing with opslimit=3, memlimit=64 bytes";
        Hasher hasher(HashConfig{3, 64ull * 1024 * 1024});
        std::string hashedPassword;
        try
        {
            hashedPassword = hasher.hash(password);
            password = ""; // Clear plaintext password from memory
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << "Password hashing failed: " << e.what();
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Error: Password hashing failed");
            callback(resp);
            return;
        }
        auto end = std::chrono::steady_clock::now();
        LOG_INFO << "Password hashing took: " << std::chrono::duration<double>(end - start).count() << " seconds";
        if (hashedPassword.empty())
        {
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
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Exception occurred while creating student: " << e.what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Error creating student: " + std::string(e.what()));
        callback(resp);

    }
}




// Handles GET request to retrieve all student records from the database
void StudentController::getAllStudents(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_INFO << "Getting ready to Fetch students from DB..\n";
    // Get the default database client instance
    auto client = app().getDbClient("default");

    // Execute SQL query asynchronously to fetch student data
    client->execSqlAsync(
        "SELECT id, first_name, last_name, dob, email, phone, address, gender, student_id, enrollment_status FROM Users WHERE role = 'student'",

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
            LOG_INFO << "Fetched students and sent response.\n";
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

void StudentController::deleteStudent(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback,
                                      std::string studentInput)
{
    auto client = app().getDbClient("default");

    // URL-decode the input in case it comes from a URL
    std::string input = drogon::utils::urlDecode(studentInput);
    std::string query;
    std::vector<std::string> args;

    std::string firstName, lastName;

    // Detect if input is a full name (contains a space)
    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos)
    {
        firstName = input.substr(0, spacePos);
        lastName  = input.substr(spacePos + 1);

        query = "DELETE FROM Users WHERE first_name = ? AND last_name = ? AND role = 'student'";
        args = {firstName, lastName};
    }
    else
    {
        query = "DELETE FROM Users WHERE student_id = ?";
        args = {input};
    }

    client->execSqlAsync(
        query,
        [callback, input, firstName, lastName](const drogon::orm::Result &r)
        {
            auto resp = HttpResponse::newHttpResponse();
            if (r.affectedRows() == 0)
            {
                resp->setStatusCode(k404NotFound);
                if (!firstName.empty())
                    resp->setBody("No student found with name: " + firstName + " " + lastName);
                else
                    resp->setBody("No student found with ID: " + input);
            }
            else
            {
                resp->setStatusCode(k200OK);
                if (!firstName.empty())
                    resp->setBody("Student " + firstName + " " + lastName + " removed successfully.");
                else
                    resp->setBody("Student with ID " + input + " removed successfully.");
            }
            callback(resp);
        },
        [callback](const drogon::orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        args[0], args.size() > 1 ? args[1] : "");
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
            "SELECT first_name, last_name, dob, email, phone, address, gender, student_id, enrollment_status FROM Users WHERE student_id = ?",
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
            "UPDATE Users SET enrollment_status = ? WHERE student_id = ?",
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
