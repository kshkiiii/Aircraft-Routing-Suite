#pragma once
#include "../config/Database.hpp"
#include "../models/DTOs.hpp"
#include <memory>
#include <optional>

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
        string sql = "SELECT id, username, password_hash, role, first_name, last_name FROM users WHERE username = " + txn.quote(username);
        pqxx::result res = txn.exec(sql);
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
        string sql = "INSERT INTO users (username, password_hash, last_name, first_name, middle_name, role) VALUES (" +
                     txn.quote(dto.username) + ", " + txn.quote(passwordHash) + ", " +
                     txn.quote(dto.lastName) + ", " + txn.quote(dto.firstName) + ", " + txn.quote(dto.middleName) + ", " +
                     txn.quote(dto.role) + ") RETURNING id";
        pqxx::result res = txn.exec(sql);
        txn.commit();
        return res[0][0].as<int>();
    }

    void update(int id, const CreateUserDTO& dto, const string& passwordHash) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        string passPart = "";
        if (!passwordHash.empty()) {
            passPart = "password_hash = " + txn.quote(passwordHash) + ", ";
        }
        string sql = "UPDATE users SET " + passPart +
                     "username = " + txn.quote(dto.username) + ", "
                     "last_name = " + txn.quote(dto.lastName) + ", "
                     "first_name = " + txn.quote(dto.firstName) + ", "
                     "middle_name = " + txn.quote(dto.middleName) + ", "
                     "role = " + txn.quote(dto.role) + " "
                     "WHERE id = " + to_string(id);
        txn.exec(sql);
        txn.commit();
    }

    void remove(int id) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        txn.exec("DELETE FROM users WHERE id = " + to_string(id));
        txn.commit();
    }
};