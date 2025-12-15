#pragma once
#include "../repositories/UserRepository.hpp"
#include "../utils/CryptoUtils.hpp"
#include <optional>

using namespace std;

class AuthService {
private:
    shared_ptr<UserRepository> userRepo;

public:
    AuthService(shared_ptr<UserRepository> repo) : userRepo(repo) {}

    optional<LoginResponse> login(const string& username, const string& password) {
        auto user = userRepo->findByUsername(username);
        if (!user) return nullopt;

        string hashedInput = CryptoUtils::hashPassword(password);
        
        if (hashedInput != user->password_hash) {
            return nullopt;
        }

        return LoginResponse{
            user->role + ":" + user->username,
            user->role,
            user->last_name + " " + user->first_name
        };
    }
    
    vector<UserDTO> getAllUsers() {
        return userRepo->getAllUsers();
    }
};