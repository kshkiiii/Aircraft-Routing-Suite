#pragma once
#include "../config/Database.hpp"
#include "../models/DTOs.hpp"
#include <memory>
#include <optional>
#include <vector>

using namespace std;

struct UserEntity { int id; string username; string password_hash; string role; string first_name; string last_name; };

class UserRepository {
private:
    shared_ptr<Database> db;

public:
    UserRepository(shared_ptr<Database> database) : db(database) {}

    optional<UserEntity> findByUsername(const string& username) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        
        string sql = "SELECT id, username, password_hash, role, first_name, last_name FROM users WHERE username = $1";
        pqxx::result res = txn.exec_params(sql, username);
        
        if (res.empty()) return nullopt;
        auto row = res[0];
        return UserEntity{row[0].as<int>(), row[1].c_str(), row[2].c_str(), row[3].c_str(), row[4].c_str(), row[5].c_str()};
    }

    vector<UserDTO> getAllUsers() { 
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        string sql = "SELECT id, username, last_name, first_name, COALESCE(middle_name, ''), role FROM users ORDER BY id";
        pqxx::result res = txn.exec(sql);
        vector<UserDTO> users;
        for (auto row : res) users.push_back({row[0].as<int>(), row[1].c_str(), row[2].c_str(), row[3].c_str(), row[4].c_str(), row[5].c_str()});
        return users;
    }

    int create(const CreateUserDTO& dto, const string& passwordHash) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        
        string sql = "INSERT INTO users (username, password_hash, last_name, first_name, middle_name, role) "
                     "VALUES ($1, $2, $3, $4, $5, $6) RETURNING id";
                     
        pqxx::result res = txn.exec_params(sql, 
            dto.username, passwordHash, dto.lastName, dto.firstName, dto.middleName, dto.role
        );
        
        txn.commit();
        return res[0][0].as<int>();
    }

    void update(int id, const CreateUserDTO& dto, const string& passwordHash) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);

        if (!passwordHash.empty()) {
            string sql = "UPDATE users SET password_hash = $1, username = $2, last_name = $3, "
                         "first_name = $4, middle_name = $5, role = $6 WHERE id = $7";
            txn.exec_params(sql, passwordHash, dto.username, dto.lastName, dto.firstName, dto.middleName, dto.role, id);
        } else {
            string sql = "UPDATE users SET username = $1, last_name = $2, first_name = $3, "
                         "middle_name = $4, role = $5 WHERE id = $6";
            txn.exec_params(sql, dto.username, dto.lastName, dto.firstName, dto.middleName, dto.role, id);
        }
        
        txn.commit();
    }

    void remove(int id) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        txn.exec_params("DELETE FROM users WHERE id = $1", id);
        txn.commit();
    }
};