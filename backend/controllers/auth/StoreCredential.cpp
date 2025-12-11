#include "StoreCredential.h"

// Non-default constructor
// TODO: Initialize all data from add-student form to this constructor
StoreCredential::StoreCredential(const std::string& firstname, const std::string& lastname,
                                 const std::string& DOB, const std::string& email,
                                 const std::string& phone_no, const std::string& address,
                                 const std::string& gender, const std::string& studentID,
                                 const std::string& username, const std::string& hashed_pass,
                                 std::string& role)
    : firstname(firstname),
      lastname(lastname),
      DOB(DOB),
      email(email),
      phone(phone_no),
      address(address),
      gender(gender),
      studentId(studentID),
      username(username),
      hashed_password(hashed_pass),
      role(role) {}

const std::string& StoreCredential::getFirstName() const { return firstname; }

const std::string& StoreCredential::getLastName() const { return lastname; }

const std::string& StoreCredential::getDOB() const { return DOB; }

const std::string& StoreCredential::getEmail() const { return email; }

const std::string& StoreCredential::getPhone() const { return phone; }

const std::string& StoreCredential::getAddress() const { return address; }

const std::string& StoreCredential::getGender() const { return gender; }

const std::string& StoreCredential::getStudentID() const { return studentId; }

const std::string& StoreCredential::getUsername() const { return username; }

const std::string& StoreCredential::getHashedPassword() const { return hashed_password; }

const std::string& StoreCredential::getRole() const { return role; }

// stores Info to Database
void StoreCredential::storeToDB(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) const {
    auto client = drogon::app().getDbClient("default");
    // optional<std::string> to handle NULL for empty studentId
    std::optional<std::string> studentIdValue;
    if (!studentId.empty()) {
        studentIdValue = studentId;
    }
    try {
        client->execSqlAsync(
            "INSERT INTO Users (first_name, last_name, dob, email, phone, address, gender, "
            "student_id, username, hashed_password, role) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            [this, callback](const drogon::orm::Result& result) {
                LOG_INFO << "Successfully inserted user: " << email;
                LOG_INFO << "Student record stored in database\n";
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k200OK);
                if (role == "admin") {
                    LOG_INFO << "Admin created successfully" << "\n";
                    resp->setBody("Admin created successfully");
                    callback(resp);
                    return;
                }
                LOG_INFO << "Student created successfully" << "\n";
                resp->setBody("Student created successfully");
                callback(resp);
            },
            [callback](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Database error: " << e.base().what();

                auto resp = drogon::HttpResponse::newHttpResponse();
                if (std::string(e.base().what()).find("Duplicate entry") != std::string::npos) {
                    // Duplicate detected (e.g., unique email or username)
                    resp->setStatusCode(drogon::k409Conflict);
                    resp->setBody(
                        "Error: Duplicate entry detected (email or username already exists)");
                } else {
                    // Other database errors
                    resp->setStatusCode(drogon::k500InternalServerError);
                    resp->setBody("Error: Failed to insert user into database");
                }
                callback(resp);
                throw std::runtime_error("Failed to insert user into database");
            },
            firstname, lastname, DOB, email, phone, address, gender, studentIdValue, username,
            hashed_password, role);

    } catch (const std::exception& e) {
        LOG_ERROR << "Exception occurred while storing to DB: " << e.what();
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setBody("Error: Server error while processing request");
        callback(resp);
        throw;
    }
}