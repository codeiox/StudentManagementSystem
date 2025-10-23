#ifndef HASHER_H
#define HASHER_H

#include <sodium.h>

#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>


// This is the recommended config for better security
struct HashConfig {
    unsigned long long opslimit = 3;                     // 2 ops limit
    size_t memlimit_bytes = 64ull * 1024 * 1024;  // 19 MiB
};

class Hasher {
   public:
    // Public methods
    explicit Hasher(HashConfig cfg = {}) : cfg_(cfg) {}

    // Returns an encoded Argon2id string (salt + params + hash inside).
    std::string hash(const std::string& password) const;

    // Verifies password against encoded string.
    bool verify(const std::string& password, const std::string& encoded) const;

    // Returns true if the stored hash should be upgraded to stronger params.
    bool needsRehash(const std::string& encoded) const;

   private:
    HashConfig cfg_;
    static void ensureSodiumReady();  // Ensure libsodium is initialized and must be called before
                                      // any other libsodium function.
};

#endif  // HASHER_H
