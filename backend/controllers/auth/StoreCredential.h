#ifndef STORE_CREDENTIAL_H
#define STORE_CREDENTIAL_H

#include <iostream>
#include <string>
#include <chrono>
#include "Hasher.h"
#include <iomanip>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <drogon/drogon.h>
#include <drogon/orm/Exception.h>
#include <optional>
#include <stdexcept>

class StoreCredential {
private:
    std::string firstname, lastname, DOB, email, phone, address, gender, studentId, username,
            hashed_password, role;

public:
    // non-default constructor
    StoreCredential(
            const std::string &firstname, const std::string &lastname,
            const std::string &DOB, const std::string &email, const std::string &phone_no,
            const std::string &address, const std::string &gender,
            const std::string &studentID, const std::string &username,
            const std::string &password, std::string &role);

    // Getter & Setter for unit-testing
    const std::string &getFirstName() const;

    const std::string &getLastName() const;

    const std::string &getDOB() const;

    const std::string &getEmail() const;

    const std::string &getPhone() const;

    const std::string &getAddress() const;

    const std::string &getGender() const;

    const std::string &getStudentID() const;

    const std::string &getUsername() const;

    const std::string &getHashedPassword() const;

    const std::string &getRole() const;

    // store username and hashed password into database
    void storeToDB(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};

#endif  // STORE_CREDENTIAL_H
