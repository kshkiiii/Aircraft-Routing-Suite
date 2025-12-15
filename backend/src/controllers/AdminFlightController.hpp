#pragma once
#include "crow_all.h"
#include "../services/FlightService.hpp"
#include "../services/AuthService.hpp"
#include "../repositories/AuditRepository.hpp" 

using namespace std;

class AdminFlightController {
private:
    shared_ptr<FlightService> flightService;
    shared_ptr<AuthService> authService;
    shared_ptr<AuditRepository> auditRepo; 

public:
    AdminFlightController(
        shared_ptr<FlightService> fs, 
        shared_ptr<AuthService> as,
        shared_ptr<AuditRepository> ar) 
        : flightService(fs), authService(as), auditRepo(ar) {}

    void registerRoutes(crow::SimpleApp& app) {
        
        CROW_ROUTE(app, "/api/admin/flights")
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.empty()) return crow::response(401);
            if (token.find("admin:") != 0 && token.find("operator:") != 0) return crow::response(403);

            auto flights = flightService->getPrivateFlights();
            
            vector<crow::json::wvalue> list;
            for (const auto& f : flights) list.push_back(privateFlightToJson(f));
            crow::json::wvalue result;
            result["flights"] = std::move(list);
            return crow::response(200, result);
        });

        CROW_ROUTE(app, "/api/admin/users")
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403);

            auto users = authService->getAllUsers();
            
            vector<crow::json::wvalue> list;
            for (const auto& u : users) list.push_back(userToJson(u));
            crow::json::wvalue result;
            result["users"] = std::move(list);
            return crow::response(200, result);
        });

        CROW_ROUTE(app, "/api/admin/audit")
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403);

            auto logs = auditRepo->getAllLogs();
            
            vector<crow::json::wvalue> list;
            for (const auto& l : logs) list.push_back(auditToJson(l));
            crow::json::wvalue result;
            result["logs"] = std::move(list);
            return crow::response(200, result);
        });
    }
};