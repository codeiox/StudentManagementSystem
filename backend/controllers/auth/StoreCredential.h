#ifndef STORE_CREDENTIAL_H
#define STORE_CREDENTIAL_H

#include <iostream>
#include <string>

#include "Hasher.h"

class StoreCredential {
   private:
    std::string firstname, lastname, DOB, email, phone, address, sex, studentID, username,
        hashed_password;

   public:
    // non-default constructor
    StoreCredential(const std::string& firstname, const std::string& lastname,
                    const std::string& DOB, const std::string& email, const std::string& phone_no,
                    const std::string& address, const std::string& sex,
                    const std::string& studentID, const std::string& username,
                    const std::string& password);

    // Getter & Setter for unit-testing
    const std::string& getFirstName() const;
    const std::string& getLastName() const;
    const std::string& getDOB() const;
    const std::string& getEmail() const;
    const std::string& getPhone() const;
    const std::string& getAddress() const;
    const std::string& getSex() const;
    const std::string& getStudentID() const;
    const std::string& getUsername() const;
    const std::string& getHashedPassword() const;

    // store username and hashed password into database
    void storeToDB() const;
};

#endif  // STORE_CREDENTIAL_H
