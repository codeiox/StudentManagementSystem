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
                                 const std::string& sex, const std::string& studentID,
                                 const std::string& username, const std::string& hashed_pass)
    : firstname(firstname),
      lastname(lastname),
      DOB(DOB),
      email(email),
      phone(phone_no),
      address(address),
      sex(sex),
      studentID(studentID),
      username(username),
      hashed_password(hashed_pass) {}

const std::string& StoreCredential::getFirstName() const { return firstname; }
const std::string& StoreCredential::getLastName() const { return lastname; }
const std::string& StoreCredential::getDOB() const { return DOB; }
const std::string& StoreCredential::getEmail() const { return email; }
const std::string& StoreCredential::getPhone() const { return phone; }
const std::string& StoreCredential::getAddress() const { return address; }
const std::string& StoreCredential::getSex() const { return sex; }

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
            "INSERT INTO Students (first_name, last_name, dob, email, phone, address, sex, "
            "student_id, username, password_hash) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            firstname, lastname, DOB, email, phone, address, sex, studentID, username,
            hashed_password);
    } catch (const drogon::orm::DrogonDbException& e) {
        throw std::runtime_error("DB storage failed: " + std::string(e.base().what()));
    }
}