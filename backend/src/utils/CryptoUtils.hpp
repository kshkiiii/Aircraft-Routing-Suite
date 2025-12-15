#pragma once
#include <string>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

using namespace std;

namespace CryptoUtils {
    inline string hashPassword(const string& password) {
        string salt = "FlightSystem_v1"; 
        string input = salt + password;

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

        stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            ss << hex << setw(2) << setfill('0') << (int)hash[i];
        }
        return ss.str();
    }
}