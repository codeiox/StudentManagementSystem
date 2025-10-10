#include "Hasher.h"

#include <iostream>
#include <string>
void Hasher::ensureSodiumReady() {
    static bool sodium_initialized = false;
    if (!sodium_initialized) {
        if (sodium_init() < 0) {
            throw std::runtime_error("Failed to initialize libsodium");
        }
        sodium_initialized = true;
    }
}

// Provide the password by calling this function. It returns an encoded
std::string Hasher::hash(const std::string& password) const {
    ensureSodiumReady();  // Ensure libsodium is initialized and must be called before any other
                          // libsodium function.

    // Libsodium outputs a PHC-like encoded string into fixed-size buffer.
    std::array<char, crypto_pwhash_STRBYTES> encoded{};
    const int rc = crypto_pwhash_str(encoded.data(),      // output buffer
                                     password.c_str(),    // password input
                                     password.size(),     // password length
                                     cfg_.opslimit,       // opslimit (time)
                                     cfg_.memlimit_bytes  // memlimit (memory)
    );
    if (rc != 0) {
        // typically means memlimit too high for the machine/container
        throw std::runtime_error("crypto_pwhash_str failed (insufficient memory resources?)");
    }
    return std::string(encoded.data());  // PHC string is NULL-terminated
}

bool Hasher::verify(const std::string& password, const std::string& encoded) const {
    ensureSodiumReady();  // Ensure libsodium is initialized and must be called before any other
                          // libsodium function.

    // Verify password against the stored hash
    const int rc = crypto_pwhash_str_verify(encoded.c_str(),   // stored hash (PHC string)
                                            password.c_str(),  // password input
                                            password.size()    // password length
    );
    return rc == 0;  // 0 = success; -1 = failure libsodium uses constant-time comparison internally
}

bool Hasher::needsRehash(const std::string& encoded) const {
    ensureSodiumReady();

    const int rc = crypto_pwhash_str_needs_rehash(encoded.c_str(),     // stored hash (PHC string)
                                                  cfg_.opslimit,       // current opslimit
                                                  cfg_.memlimit_bytes  // current memlimit
    );

    if (rc < 0) {
        // Malformed/unknown string -> treat as needs rehash ( or reject at higher layer )
        return true;
    }
    return rc == 1;  // 1 = needs rehash; 0 = ok
}