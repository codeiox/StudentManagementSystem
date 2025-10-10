//"copyright 2025"

#ifndef NEWCXXTEST_H
#define NEWCXXTEST_H

/* include this path when using vscode: /opt/homebrew/opt/cxxtest/include */

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../controllers/auth/Hasher.h"

// Use modest defaults so tests run quickly.
// You can bump mem/ops for a heavier test build later.
static HashConfig testCfg() {
    HashConfig cfg;
    cfg.opslimit = 2;                          // lighter for CI
    cfg.memlimit_bytes = 32ull * 1024 * 1024;  // 32 MB
    return cfg;
}

class newCxxTest : public CxxTest::TestSuite {
   public:
    // test

    void testHashAndVerifyOk() {
        Hasher hasher(testCfg());
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
        Hasher hasher(testCfg());
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
        Hasher hasher(testCfg());
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
        Hasher hasher(testCfg());
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
};
#endif /* NEWCXXTEST_H */
