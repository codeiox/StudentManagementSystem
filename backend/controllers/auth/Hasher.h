#ifndef HASHER_H
#define HASHER_H

#include <sodium.h>

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

struct HashConfig {
    // libsodium “opslimit” (time) and “memlimit” (memory) knobs
    // Start here; you can raise them after benchmarking your server.
    unsigned long long opslimit = 3;              // crypto_pwhash_OPSLIMIT_MODERATE-ish
    size_t memlimit_bytes = 64ull * 1024 * 1024;  // 64 MB
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
