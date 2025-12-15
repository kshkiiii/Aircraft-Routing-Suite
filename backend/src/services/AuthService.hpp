#pragma once
#include "../repositories/UserRepository.hpp"
#include "../repositories/AuditRepository.hpp"
#include "../utils/CryptoUtils.hpp"
#include <optional>

using namespace std;

class AuthService {
private:
    shared_ptr<UserRepository> userRepo;
    shared_ptr<AuditRepository> audit;

public:
    AuthService(shared_ptr<UserRepository> repo, shared_ptr<AuditRepository> auditRepo = nullptr) 
        : userRepo(repo), audit(auditRepo) {}

    optional<LoginResponse> login(const string& username, const string& password) { 
        auto user = userRepo->findByUsername(username);
        if (!user) return nullopt;
        string hashedInput = CryptoUtils::hashPassword(password);
        if (hashedInput != user->password_hash) return nullopt;
        return LoginResponse{ user->role + ":" + user->username, user->role, user->last_name + " " + user->first_name };
    }
    
    vector<UserDTO> getAllUsers() { return userRepo->getAllUsers(); }

    void createUser(const CreateUserDTO& dto, const string& adminName) {
        string hash = CryptoUtils::hashPassword(dto.password);
        int newId = userRepo->create(dto, hash);
        
        if (audit) {
            audit->saveLog(adminName, "CREATE", "users", newId, "Создан пользователь ID: " + to_string(newId));
        }
    }

    void updateUser(int id, const CreateUserDTO& dto, const string& adminName) {
        string hash = "";
        if (!dto.password.empty()) {
            hash = CryptoUtils::hashPassword(dto.password);
        }
        userRepo->update(id, dto, hash);
        
        if (audit) {
            audit->saveLog(adminName, "UPDATE", "users", id, "Обновлен пользователь ID: " + to_string(id));
        }
    }

    void deleteUser(int id, const string& adminName) {
        userRepo->remove(id);
        if (audit) {
            audit->saveLog(adminName, "DELETE", "users", id, "Удален пользователь ID: " + to_string(id));
        }
    }
};