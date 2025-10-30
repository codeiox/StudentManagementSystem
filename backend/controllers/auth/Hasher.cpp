#include "Hasher.h"
#include <array>
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

    std::array<char, crypto_pwhash_STRBYTES> out;
    if (crypto_pwhash_str(out.data(), password.c_str(), password.length(),
                          cfg_.opslimit, cfg_.memlimit_bytes) != 0) {
        throw std::runtime_error("Hashing failed: out of memory or invalid parameters");
    }
    return std::string(out.data());
}

bool Hasher::verify(const std::string& password, const std::string& encoded) const {
    ensureSodiumReady();  // Ensure libsodium is initialized and must be called before any other
                          // libsodium function.
    if (password.empty() || encoded.empty()) {
        return false;
    }
    return crypto_pwhash_str_verify(encoded.c_str(), password.c_str(), password.length()) == 0;  // 0 = match, -1 = no match
}

bool Hasher::needsRehash(const std::string& encoded) const {
    ensureSodiumReady();
    if (encoded.empty()) {
        return true;
    }
    return crypto_pwhash_str_needs_rehash(encoded.c_str(), cfg_.opslimit, cfg_.memlimit_bytes) > 0;
}