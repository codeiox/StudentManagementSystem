#include "StoreCredential.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Exception.h>

#include <iostream>
#include <stdexcept>
#include <string>

// Non-default constructor
// TODO: Initialize all data from add-student form to this constructor
StoreCredential::StoreCredential(const std::string& firstname, const std::string& lastname,
                                 const std::string& DOB, const std::string& email,
                                 const std::string& phone_no, const std::string& address,
                                 const std::string& gender, const std::string& studentID,
                                 const std::string& username, const std::string& hashed_pass,
                                 const std::string& enrollmentStatus)
    : firstname(firstname),
      lastname(lastname),
      DOB(DOB),
      email(email),
      phone(phone_no),
      address(address),
      gender(gender),
      studentID(studentID),
      username(username),
      hashed_password(hashed_pass),
      enrollmentStatus(enrollmentStatus) {}

const std::string& StoreCredential::getFirstName() const { return firstname; }
const std::string& StoreCredential::getLastName() const { return lastname; }
const std::string& StoreCredential::getDOB() const { return DOB; }
const std::string& StoreCredential::getEmail() const { return email; }
const std::string& StoreCredential::getPhone() const { return phone; }
const std::string& StoreCredential::getAddress() const { return address; }
const std::string& StoreCredential::getGender() const { return gender; }

const std::string& StoreCredential::getStudentID() const { return studentID; }

const std::string& StoreCredential::getUsername() const { return username; }
const std::string& StoreCredential::getHashedPassword() const { return hashed_password; }

// stores Info to Database
void StoreCredential::storeToDB() const {
    auto client = drogon::app().getDbClient();
    // Use async exec; throw on sync for simplicity, or handle async
    try {
        //TODO: add timestamp
        auto result = client->execSqlSync(
            "INSERT INTO Students (first_name, last_name, dob, email, phone, address, gender, "
            "student_id, username, password_hash, enrollment_status) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            firstname, lastname, DOB, email, phone, address, gender, studentID, username,
            hashed_password, enrollmentStatus);
    } catch (const drogon::orm::DrogonDbException& e) {
        throw std::runtime_error("DB storage failed: " + std::string(e.base().what()));
    }
}