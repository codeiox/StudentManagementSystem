//"copyright 2025"

#ifndef NEWCXXTEST_H
#define NEWCXXTEST_H

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../controllers/auth/Hasher.h"
#include "../controllers/auth/StoreCredential.h"
#include "../controllers/handle-dashboard/CalculateGPA.h"
class newCxxTest : public CxxTest::TestSuite {
   public:
    // ------------------------------------------------------------
    //                Test case for Hash class
    // ------------------------------------------------------------

    void testHashAndVerifyOk() {
        Hasher hasher(HashConfig{1, crypto_pwhash_MEMLIMIT_MIN});
        const std::string pwd = "example-password";
        const std::string enc = hasher.hash(pwd);

        // For debugging
        std::cout << std::endl;
        std::cout << "============================================================================="
                     "============================================================================="
                  << std::endl;
        std::ostringstream oss1;
        oss1 << "Original Password: " << pwd;
        std::cout << oss1.str() << std::endl;
        std::ostringstream oss2;
        oss2 << "Encoded Password: " << enc;
        std::cout << oss2.str() << std::endl;

        TS_ASSERT(!enc.empty());
        TS_ASSERT(hasher.verify(pwd, enc));
    }

    void testVerifyWrongPasswordFails() {
        Hasher hasher(HashConfig{1, crypto_pwhash_MEMLIMIT_MIN});
        const std::string enc = hasher.hash("secret-123");

        // For debugging
        std::cout << "============================================================================="
                     "============================================================================"
                  << std::endl;
        std::ostringstream oss1;
        oss1 << "Encoded Password: " << enc;
        std::cout << oss1.str() << std::endl;

        TS_ASSERT(!hasher.verify("wrong", enc));
    }

    void testDifferentSaltsDifferentHashes() {
        Hasher hasher(HashConfig{1, crypto_pwhash_MEMLIMIT_MIN});
        const std::string pwd = "same-password";
        const std::string a = hasher.hash(pwd);
        const std::string b = hasher.hash(pwd);

        std::cout << "============================================================================="
                     "============================================================================"
                  << std::endl;
        std::ostringstream oss1;
        oss1 << "Original Password: " << pwd;
        std::cout << oss1.str() << std::endl;

        std::ostringstream oss2;
        oss2 << "Encoded A: " << a;

        std::cout << oss2.str() << std::endl;
        std::ostringstream oss3;

        oss3 << "Encoded B: " << b;
        std::cout << oss3.str() << std::endl;

        TS_ASSERT_DIFFERS(a, b);  // probabilistically true because fresh salt
    }

    void testNeedsRehashWhenParamsIncrease() {
        // Create a hash with lower parameters...
        Hasher weak({.opslimit = 1, .memlimit_bytes = 16ull * 1024 * 1024});
        std::string enc = weak.hash("pw-abc");

        // For debugging
        std::cout << "============================================================================="
                     "============================================================================"
                  << std::endl;
        std::ostringstream oss1;

        oss1 << "Encoded with weak policy: " << enc;
        std::cout << oss1.str() << std::endl;

        // ...then check with stronger policy.
        Hasher strong({.opslimit = 3, .memlimit_bytes = 64ull * 1024 * 1024});
        TS_ASSERT(strong.needsRehash(enc));
    }

    void testEncodedLooksLikeArgon2id() {
        Hasher hasher(HashConfig{1, crypto_pwhash_MEMLIMIT_MIN});
        const std::string enc = hasher.hash("pw");

        // For debugging
        std::cout << "============================================================================="
                     "============================================================================"
                  << std::endl;
        std::ostringstream oss1;
        oss1 << "Encoded Password: " << enc;

        std::cout << oss1.str() << std::endl;

        // libsodium returns PHC-like strings starting with "$argon2id$"
        TS_ASSERT(enc.rfind("$argon2id$", 0) == 0);
        std::cout
            << "=========================================================================== End"
               " =========================================================================="
            << std::endl;
    }

    // *************************************************************

    // ------------------------------------------------------------
    //        Test case for CalculateGPA & Dashboard class
    // ------------------------------------------------------------

    static void testGradesCalculation_1() {
        std::vector<std::pair<std::string, int>> grade_list = {
            {"A", 3}, {"A", 4}, {"B+", 3}, {"A", 4}, {"B", 4}};

        // Manually computed: (12 + 16 + 9.9 + 16 + 12) / 18
        double expectedGPA = 65.9 / 18.0;
        TS_ASSERT_DELTA(calculateOverAllGPA(grade_list), expectedGPA, 0.0001);
    }

    static void testGradesCalculation_2() {
        std::vector<std::pair<std::string, int>> grade_list = {{"A", 3}, {"A-", 4}, {"B+", 2},
                                                               {"A", 3}, {"B", 4},  {"F", 3}};

        double expectedGPA = 57.4 / 19.0;

        TS_ASSERT_DELTA(calculateOverAllGPA(grade_list), expectedGPA, 0.0001);
    }

    static void testGradesCalculation_3() {
        std::vector<std::pair<std::string, int>> grade_list = {{"A", 4}};

        TS_ASSERT_DELTA(calculateOverAllGPA(grade_list), 4.0, 0.0001);
    }

    static void testGradesCalculation_4() {
        std::vector<std::pair<std::string, int>> grade_list = {};
        TS_ASSERT_THROWS_ANYTHING(calculateOverAllGPA(grade_list));
    }

    static void testGradesCalculation_5() {
        std::vector<std::pair<std::string, int>> grade_list = {{"F", 5}};

        TS_ASSERT_DELTA(calculateOverAllGPA(grade_list), 0.0, 0.00001);
    }
};
#endif /* NEWCXXTEST_H */
