#include "StudentController.h"

#include <drogon/utils/Utilities.h>

#include <ctime>
#include <sstream>
#include <numeric>

#include "../handle-dashboard/CalculateGPA.h"
#include "../handle-dashboard/Dashboard.h"
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
            std::string lastName = studentJson.get("lastName", "").asString();
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
            storeCredential.storeToDB(req, [](const HttpResponsePtr &) {});
        }

        // Respond with a summary
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setBody("Multiple students created successfully");
        LOG_INFO << "Multiple students created successfully" << "\n";
        callback(resp);
        return;
    }

    // --- Single student logic continues exactly as before ---
    // Do NOT redeclare json
    LOG_INFO << "Received JSON: " << json->toStyledString();

    // Validate required fields
    if (!json->isMember("firstName") || !json->isMember("lastName") || !json->isMember("email") ||
        !json->isMember("phone") || !json->isMember("dateofbirth") || !json->isMember("address") ||
        !json->isMember("gender") || !json->isMember("password"))
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

    // converts role to lower case using std::transform and lambda function
    std::transform(role.begin(), role.end(), role.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    LOG_INFO << "Converted role to lowercase: " << role << "\n";

    // Basic validation for empty fields
    if (firstName.empty() || lastName.empty() || email.empty() ||
        (role == "student" && studentId.empty()) || phone.empty() || dateOfBirth.empty() ||
        address.empty() || gender.empty() || role.empty() || password.empty())
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
        LOG_INFO << "Password hashing took: " << std::chrono::duration<double>(end - start).count()
                 << " seconds";
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
        "SELECT id, first_name, last_name, dob, email, phone, address, gender, student_id, "
        "enrollment_status FROM Users WHERE role = 'student'",

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
                student["enrollmentStatus"] =
                    row["enrollment_status"].as<std::string>(); // May be null
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
        lastName = input.substr(spacePos + 1);

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
                    resp->setBody("Student " + firstName + " " + lastName +
                                  " removed successfully.");
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
void StudentController::getStudentById(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::string studentId)
{
    // Get database client
    auto client = drogon::app().getDbClient("default");

    // Execute SQL query to fetch student data
    client->execSqlAsync(
        "SELECT first_name, last_name, dob, email, phone, address, gender, student_id, "
        "enrollment_status FROM Users WHERE student_id = ?",
        [callback](const drogon::orm::Result &result)
        {
            // If no student found, return 404
            if (result.empty())
            {
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
        [callback](const drogon::orm::DrogonDbException &e)
        {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        studentId // SQL parameter
    );
}

// Handles PUT request to update a student's enrollment status
void StudentController::updateEnrollmentStatus(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::string studentId)
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
    std::set<std::string> validStatuses = {"active", "graduated", "probation", "suspended",
                                           "inactive"};
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
        newStatus, studentId // SQL parameters
    );
}

void StudentController::enrollStudentInCourse(
    const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
    std::string studentId)
{
    auto json = req->getJsonObject();

    // ---------------------
    // Validate required fields
    // ---------------------
    if (!json || !json->isMember("course_id") || !json->isMember("term"))
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields: course_id, term");
        callback(resp);
        return;
    }

    int courseId = (*json)["course_id"].asInt();
    std::string term = (*json)["term"].asString();

    // ---------------------
    // Parse and normalize term
    // ---------------------
    std::string season;
    int year = 0;
    term.erase(0, term.find_first_not_of(" \t\n\r"));
    term.erase(term.find_last_not_of(" \t\n\r") + 1);
    std::istringstream iss(term);
    iss >> season >> year;
    if (year <= 0)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid term format. Must be: Spring|Summer|Fall 2025");
        callback(resp);
        return;
    }

    std::transform(season.begin(), season.end(), season.begin(), ::tolower);
    if (season == "fall")
        season = "Fall";
    else if (season == "spring")
        season = "Spring";
    else if (season == "summer")
        season = "Summer";

    if (season != "Fall" && season != "Spring" && season != "Summer")
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid term format. Must be: Spring|Summer|Fall 2025");
        callback(resp);
        return;
    }

    // ---------------------
    // Determine current term
    // ---------------------
    time_t t = time(nullptr);
    tm now{};
    localtime_r(&t, &now);
    int currentYear = now.tm_year + 1900;
    int month = now.tm_mon + 1;
    std::string currentSeason;
    if (month >= 1 && month <= 4)
        currentSeason = "Spring";
    else if (month >= 5 && month <= 7)
        currentSeason = "Summer";
    else
        currentSeason = "Fall";

    auto seasonOrder = [](const std::string &s) -> int
    {
        if (s == "Spring")
            return 1;
        if (s == "Summer")
            return 2;
        return 3; // Fall
    };

    bool isPastTerm = false;
    if (year < currentYear)
        isPastTerm = true;
    else if (year == currentYear && seasonOrder(season) < seasonOrder(currentSeason))
        isPastTerm = true;

    // ---------------------
    // Extract grade and Postman override
    // ---------------------
    std::string grade = json->get("grade", "").asString();
    bool isPostmanOverride = json->get("allow_past", false).asBool();

    if (!isPostmanOverride && isPastTerm)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Cannot register a course into past terms.");
        callback(resp);
        return;
    }

    if (isPostmanOverride && !isPastTerm && !grade.empty())
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Cannot assign a grade for current/future courses.");
        callback(resp);
        return;
    }

    // ---------------------
    // Determine status
    // ---------------------
    std::string status = isPastTerm ? "completed" : "current";
    if (isPostmanOverride && isPastTerm && json->isMember("status"))
        status = (*json)["status"].asString();

    int majorId = json->isMember("major_id") ? (*json)["major_id"].asInt() : 0;
    int minorId = json->isMember("minor_id") ? (*json)["minor_id"].asInt() : 0;

    // ---------------------
    // Insert enrollment ONLY if it doesn't exist
    // ---------------------
    auto client = app().getDbClient("default");
    client->execSqlAsync(
        R"(INSERT INTO Enrollments (user_id, course_id, term, status, grade)
            SELECT u.id, ?, ?, ?, ?
            FROM Users u
            WHERE u.student_id = ?
              AND NOT EXISTS (
                  SELECT 1 FROM Enrollments e
                  WHERE e.user_id = u.id
                    AND e.course_id = ?
                    AND e.term = ?
              ))",
        [client, callback, status, courseId, term, studentId](const orm::Result &r)
        {
            if (r.affectedRows() == 0)
            {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k400BadRequest);
                resp->setBody("Student is already enrolled in this course for this term.");
                callback(resp);
                return;
            }

            // For completed courses, pull credits and insert into course_credits by overwriting the
            // previous one
            if (status == "completed")
            {
                // Create a shared_ptr to hold the callback so it can be shared across multiple
                // lambdas
                auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr &)>>(
                    std::move(callback));

                client->execSqlAsync(
                    "SELECT credits FROM Courses WHERE course_id = ?",
                    [client, callbackPtr, studentId, courseId,
                     term](const orm::Result &creditResult)
                    {
                        if (creditResult.empty())
                        {
                            auto resp = HttpResponse::newHttpResponse();
                            resp->setStatusCode(k404NotFound);
                            resp->setBody("Course not found when updating credits.");
                            (*callbackPtr)(resp);
                            return;
                        }
                        int credits = creditResult[0]["credits"].as<int>();
                        client->execSqlAsync(
                            R"(UPDATE Enrollments e
               JOIN Users u ON e.user_id = u.id
               SET e.course_credits = ?
               WHERE u.student_id = ?
                 AND e.course_id = ?
                 AND e.term = ?
                 AND e.status = 'completed')",
                            [callbackPtr, studentId](const orm::Result &)
                            {
                                // Create a mock request with the student ID parameter
                                auto mockReq = HttpRequest::newHttpRequest();
                                mockReq->setParameter("id", studentId);

                                // Call the getStudentCurrentGrades function
                                Dashboard dashboardInstance;
                                dashboardInstance.getStudentCurrentGrades(mockReq,
                                                                          std::move(*callbackPtr));
                            },
                            [callbackPtr](const orm::DrogonDbException &e)
                            {
                                auto resp = HttpResponse::newHttpResponse();
                                resp->setStatusCode(k500InternalServerError);
                                resp->setBody("Database error while updating credits: " +
                                              std::string(e.base().what()));
                                (*callbackPtr)(resp);
                            },
                            credits, studentId, courseId, term);
                    },
                    [callbackPtr](const orm::DrogonDbException &e)
                    {
                        auto resp = HttpResponse::newHttpResponse();
                        resp->setStatusCode(k500InternalServerError);
                        resp->setBody("Database error while fetching credits: " +
                                      std::string(e.base().what()));
                        (*callbackPtr)(resp);
                    },
                    courseId);
                return;
            }

            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k200OK);
            resp->setBody("Enrollment created successfully.");
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            std::string msg = std::string(e.base().what());
            if (msg.find("Duplicate entry") != std::string::npos)
                msg = "Student is already enrolled in this course for this term.";
            resp->setStatusCode(k400BadRequest);
            resp->setBody(msg);
            callback(resp);
        },
        courseId, term, status, grade, studentId, courseId, term);

    // ---------------------
    // Update major/minor if provided
    // ---------------------
    if (majorId > 0)
    {
        client->execSqlAsync(
            "UPDATE Users SET major_id = ? WHERE student_id = ?", [](const orm::Result &) {},
            [](const orm::DrogonDbException &e)
            {
                LOG_ERROR << "Failed to update major: " << e.base().what();
            },
            majorId, studentId);
    }

    if (minorId > 0)
    {
        client->execSqlAsync(
            "UPDATE Users SET minor_id = ? WHERE student_id = ?", [](const orm::Result &) {},
            [](const orm::DrogonDbException &e)
            {
                LOG_ERROR << "Failed to update minor: " << e.base().what();
            },
            minorId, studentId);
    }
}

// Handles GET request to fetch all courses for a student (chronologically ordered)
void StudentController::getStudentCourses(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback,
                                          std::string studentId)
{
    auto client = app().getDbClient("default");

    client->execSqlAsync(
        R"(SELECT c.course_id, c.course_name, c.credits, e.term, e.status
           FROM Enrollments e
           JOIN Users u ON e.user_id = u.id
           JOIN Courses c ON e.course_id = c.course_id
           WHERE u.student_id = ?
           ORDER BY 
               CAST(SUBSTRING_INDEX(e.term, ' ', -1) AS UNSIGNED),  -- Year
               FIELD(SUBSTRING_INDEX(e.term, ' ', 1), 'Spring', 'Summer', 'Fall'),  -- Semester
               c.course_name
        )",
        [callback](const orm::Result &result)
        {
            Json::Value courses(Json::arrayValue);
            for (const auto &row : result)
            {
                Json::Value course;
                course["course_id"] = row["course_id"].as<int>();
                course["course_name"] = row["course_name"].as<std::string>();
                course["credits"] = row["credits"].as<int>();
                course["term"] = row["term"].as<std::string>();
                course["status"] = row["status"].as<std::string>();
                courses.append(course);
            }

            auto resp = HttpResponse::newHttpJsonResponse(courses);
            resp->setStatusCode(k200OK);
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        studentId);
}

// Handles GET request to fetch grades for a student
void StudentController::getStudentGrades(const HttpRequestPtr &req,
                                         std::function<void(const HttpResponsePtr &)> &&callback,
                                         std::string studentId)
{
    auto client = app().getDbClient("default");

    client->execSqlAsync(
        "SELECT c.course_name, e.term, e.status, e.grade "
        "FROM Enrollments e "
        "JOIN Users u ON e.user_id = u.id "
        "JOIN Courses c ON e.course_id = c.course_id "
        "WHERE u.student_id = ? "
        "ORDER BY "
        "  CAST(SUBSTRING_INDEX(e.term, ' ', -1) AS UNSIGNED), "
        "  FIELD(SUBSTRING_INDEX(e.term, ' ', 1), 'Spring', 'Summer', 'Fall')",

        [callback](const orm::Result &result)
        {
            Json::Value grades(Json::arrayValue);
            for (const auto &row : result)
            {
                Json::Value g;
                g["course_name"] = row["course_name"].as<std::string>();
                g["term"] = row["term"].as<std::string>();
                g["status"] = row["status"].as<std::string>();
                std::string grade = row["grade"].isNull() ? "" : row["grade"].as<std::string>();
                std::string status = row["status"].as<std::string>();

                if (status == "current" && grade.empty())
                {
                    grade = "IP"; // Show "In Progress"
                }
                g["grade"] = grade;
                grades.append(g);
            }

            auto resp = HttpResponse::newHttpJsonResponse(grades);
            resp->setStatusCode(k200OK);
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        studentId);
}

// GET /api/admin/students/{studentId}/program
void StudentController::getStudentProgram(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback,
                                          std::string studentId)
{
    auto client = app().getDbClient("default");

    client->execSqlAsync(
        "SELECT m1.name AS major_name, m2.name AS minor_name "
        "FROM Users u "
        "LEFT JOIN Majors m1 ON u.major_id = m1.major_id "
        "LEFT JOIN Majors m2 ON u.minor_id = m2.major_id "
        "WHERE u.student_id = ?",
        [callback](const orm::Result &result)
        {
            Json::Value program;
            if (!result.empty())
            {
                program["major"] = result[0]["major_name"].isNull()
                                       ? "Undeclared"
                                       : result[0]["major_name"].as<std::string>();
                program["minor"] = result[0]["minor_name"].isNull()
                                       ? ""
                                       : result[0]["minor_name"].as<std::string>();
            }

            auto resp = HttpResponse::newHttpJsonResponse(program);
            resp->setStatusCode(k200OK);
            callback(resp);
        },
        [callback](const orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error: " + std::string(e.base().what()));
            callback(resp);
        },
        studentId);
}

// fetch enrollment details including history
void StudentController::getEnrollmentDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string studentId)
{
    auto client = app().getDbClient("default");

    // Query Users as primary row, LEFT JOIN EnrollmentDetails so we always get user's enrollment_status
    client->execSqlAsync(
        "SELECT u.enrollment_status AS status, e.startDate, e.graduation "
        "FROM Users u "
        "LEFT JOIN EnrollmentDetails e ON u.student_id = e.student_id "
        "WHERE u.student_id = ?",
        [client, callback, studentId](const orm::Result &result)
        {
            Json::Value enrollmentDetails;

            if (!result.empty())
            {
                // replace nulls with empty strings
                const auto &row = result[0];
                enrollmentDetails["startDate"] =
                    row["startDate"].isNull() ? "" : row["startDate"].as<std::string>();
                // This is now guaranteed to come from Users.enrollment_status (may be empty)
                enrollmentDetails["status"] =
                    row["status"].isNull() ? "" : row["status"].as<std::string>();
                enrollmentDetails["graduation"] =
                    row["graduation"].isNull() ? "" : row["graduation"].as<std::string>();
            }
            else
            {
                // Defensive fallback; unlikely since Users row should exist
                enrollmentDetails["startDate"] = "";
                enrollmentDetails["status"] = "";
                enrollmentDetails["graduation"] = "";
            }

            // Fetch enrollment history (may be empty)
            client->execSqlAsync(
                "SELECT entry FROM EnrollmentHistory WHERE student_id = ? ORDER BY id ASC",
                [callback, enrollmentDetails](const orm::Result &historyResult) mutable
                {
                    Json::Value history(Json::arrayValue);
                    for (const auto &row : historyResult)
                    {
                        history.append(row["entry"].as<std::string>());
                    }

                    enrollmentDetails["history"] = history;

                    // return JSON response with all details
                    auto resp = HttpResponse::newHttpJsonResponse(enrollmentDetails);
                    resp->setStatusCode(k200OK);
                    callback(resp);
                },
                // error handler for history query
                [callback](const orm::DrogonDbException &e)
                {
                    auto resp = HttpResponse::newHttpResponse();
                    resp->setStatusCode(k500InternalServerError);
                    resp->setBody("Database error loading history: " + std::string(e.base().what()));
                    callback(resp);
                },
                studentId);
        },
        // error handler for main details query
        [callback](const orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error loading details: " + std::string(e.base().what()));
            callback(resp);
        },
        studentId);
}

// create or update enrollment details for a student
void StudentController::createEnrollmentDetails(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string studentId)
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

    // Read fields defensively
    std::string status = json->isMember("status") ? (*json)["status"].asString() : "";
    std::string startDate = json->isMember("startDate") ? (*json)["startDate"].asString() : "";
    std::string graduation = json->isMember("graduation") ? (*json)["graduation"].asString() : "";
    Json::Value history = json->isMember("history") ? (*json)["history"] : Json::Value();

    // Validate status if provided
    if (!status.empty())
    {
        std::set<std::string> validStatuses = {
            "active", "inactive", "graduated", "probation", "suspended"
        };
        if (validStatuses.find(status) == validStatuses.end())
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Invalid status value");
            callback(resp);
            return;
        }
    }

    auto client = app().getDbClient("default");

    // 1. Insert or update EnrollmentDetails
    client->execSqlAsync(
        "INSERT INTO EnrollmentDetails (student_id, startDate, graduation) "
        "VALUES (?, ?, ?) "
        "ON DUPLICATE KEY UPDATE startDate = VALUES(startDate), graduation = VALUES(graduation)",
        [client, callback, studentId, history, status](const orm::Result &) mutable
        {
            auto appendHistory = [client, callback, studentId, history]()
            {
                // Fetch existing history first
                client->execSqlAsync(
                    "SELECT id, entry FROM EnrollmentHistory WHERE student_id = ? ORDER BY id ASC",
                    [client, callback, studentId, history](const orm::Result &existing)
                    {
                        // Store existing entries and their IDs
                        std::vector<std::string> existingEntries;
                        std::vector<int> existingIds;
                        for (const auto &row : existing)
                        {
                            existingEntries.push_back(row["entry"].as<std::string>());
                            existingIds.push_back(row["id"].as<int>());
                        }

                        // Compare JSON history with existing entries
                        Json::ArrayIndex i = 0;
                        for (; i < history.size() && i < existingEntries.size(); ++i)
                        {
                            if (!history[i].isString()) continue;

                            if (history[i].asString() != existingEntries[i])
                            {
                                // Update existing entry to match JSON
                                client->execSqlAsync(
                                    "UPDATE EnrollmentHistory SET entry = ? WHERE id = ?",
                                    [](const orm::Result &) {},
                                    [](const orm::DrogonDbException &) {},
                                    history[i].asString(), existingIds[i]
                                );
                            }
                        }

                        // Insert any new entries from JSON that extend past existing
                        for (; i < history.size(); ++i)
                        {
                            if (!history[i].isString()) continue;

                            client->execSqlAsync(
                                "INSERT INTO EnrollmentHistory (student_id, entry) VALUES (?, ?)",
                                [](const orm::Result &) {},
                                [](const orm::DrogonDbException &) {},
                                studentId, history[i].asString()
                            );
                        }

                        // Return success
                        auto resp = HttpResponse::newHttpResponse();
                        resp->setStatusCode(k200OK);
                        resp->setBody("Enrollment details saved successfully");
                        callback(resp);
                    },
                    [callback](const orm::DrogonDbException &e)
                    {
                        auto resp = HttpResponse::newHttpResponse();
                        resp->setStatusCode(k500InternalServerError);
                        resp->setBody("Database error fetching existing history: " + std::string(e.base().what()));
                        callback(resp);
                    },
                    studentId
                );
            };

            if (!status.empty())
            {
                // Update Users.enrollment_status first
                client->execSqlAsync(
                    "UPDATE Users SET enrollment_status = ? WHERE student_id = ?",
                    [appendHistory](const orm::Result &) mutable { appendHistory(); },
                    [callback](const orm::DrogonDbException &e)
                    {
                        auto resp = HttpResponse::newHttpResponse();
                        resp->setStatusCode(k500InternalServerError);
                        resp->setBody("Database error updating student status: " + std::string(e.base().what()));
                        callback(resp);
                    },
                    status, studentId
                );
            }
            else
            {
                // No status update; just append history
                appendHistory();
            }

        },
        [callback](const orm::DrogonDbException &e)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database error saving enrollment details: " + std::string(e.base().what()));
            callback(resp);
        },
        studentId, startDate, graduation
    );
}
