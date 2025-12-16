#pragma once
#include "../config/Database.hpp"
#include "../models/DTOs.hpp"
#include <vector>
#include <memory>
#include <string>
#include <optional> 

using namespace std;

class AuditRepository {
private:
    shared_ptr<Database> db;

public:
    AuditRepository(shared_ptr<Database> database) : db(database) {}

    vector<AuditDTO> getAllLogs() {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        string sql = "SELECT a.id, u.username, a.action_type, a.table_name, "
                     "to_char(a.created_at, 'DD.MM.YYYY HH24:MI'), a.changes::text "
                     "FROM audit_logs a LEFT JOIN users u ON a.user_id = u.id "
                     "ORDER BY a.created_at DESC LIMIT 100";
        pqxx::result res = txn.exec(sql);
        vector<AuditDTO> logs;
        for (auto row : res) {
            logs.push_back({
                row[0].as<int>(), 
                row[1].is_null() ? "System" : row[1].c_str(),
                row[2].c_str(), 
                row[3].c_str(), 
                row[4].c_str(), 
                row[5].c_str()
            });
        }
        return logs;
    }

    void saveLog(const string& username, const string& action, const string& table, int recordId, const string& message) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);

        pqxx::result userRes = txn.exec_params("SELECT id FROM users WHERE username = $1", username);
        
        std::optional<int> userId;
        if (!userRes.empty()) {
            userId = userRes[0][0].as<int>();
        }

        string jsonDetails = "{\"message\": \"" + message + "\"}";

        string sql = "INSERT INTO audit_logs (user_id, action_type, table_name, record_id, changes) "
                     "VALUES ($1, $2, $3, $4, $5)";
        
        txn.exec_params(sql, userId, action, table, recordId, jsonDetails);
        
        txn.commit();
    }
};