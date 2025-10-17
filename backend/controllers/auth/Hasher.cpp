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

    if (password.empty()) {
        throw std::invalid_argument("Password cannot be empty");
    }
    std::cerr << "Hashing with opslimit=" << cfg_.opslimit << ", memlimit=" << cfg_.memlimit_bytes
              << " bytes\n";
    std::string encoded(crypto_pwhash_STRBYTES, '\0');
    if (crypto_pwhash_str(encoded.data(), password.c_str(), password.size(), cfg_.opslimit,
                          cfg_.memlimit_bytes) != 0) {
        throw std::runtime_error("crypto_pwhash_str failed (insufficient memory resources?)");
    }
    encoded.resize(std::strlen(encoded.c_str()));
    return encoded;
}

bool Hasher::verify(const std::string& password, const std::string& encoded) const {
    ensureSodiumReady();  // Ensure libsodium is initialized and must be called before any other
                          // libsodium function.
    if (password.empty() || encoded.empty()) {
        return false;
    }
    return crypto_pwhash_str_verify(encoded.c_str(), password.c_str(), password.size()) ==
           0;  // 0 = match, -1 = no match
}

bool Hasher::needsRehash(const std::string& encoded) const {
    ensureSodiumReady();
    if (encoded.empty()) {
        return true;
    }
    // Rehash if params are weaker
    size_t mem_pos = encoded.find("m=");
    size_t ops_pos = encoded.find("t=");
    if (mem_pos == std::string::npos || ops_pos == std::string::npos) {
        return true;
    }
    size_t mem_end = encoded.find(',', mem_pos);
    size_t ops_end = encoded.find(',', ops_pos);
    unsigned long long stored_mem = std::stoull(encoded.substr(mem_pos + 2, mem_end - mem_pos - 2));
    unsigned long long stored_ops = std::stoull(encoded.substr(ops_pos + 2, ops_end - ops_pos - 2));
    return (stored_mem < cfg_.memlimit_bytes) || (stored_ops < cfg_.opslimit);
}