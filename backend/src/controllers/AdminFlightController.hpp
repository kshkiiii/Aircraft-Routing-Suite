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

    string getUsername(const string& token) {
        size_t pos = token.find(':');
        return (pos != string::npos) ? token.substr(pos + 1) : "unknown";
    }

public:
    AdminFlightController(shared_ptr<FlightService> fs, shared_ptr<AuthService> as, shared_ptr<AuditRepository> ar) 
        : flightService(fs), authService(as), auditRepo(ar) {}

    void registerRoutes(crow::SimpleApp& app) {
        
        CROW_ROUTE(app, "/api/admin/resources")
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.empty()) return crow::response(401);
            
            auto res = flightService->getResources();
            
            crow::json::wvalue json = resourcesToJson(res);
            return crow::response(200, json);
        });

        CROW_ROUTE(app, "/api/admin/flights").methods("POST"_method)
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403, "Только админ может создавать");

            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400);

            FlightRequestDTO dto;
            dto.flightNumber = x["flight_number"].s();
            dto.originCode = x["origin"].s();
            dto.destinationCode = x["destination"].s();
            dto.depTime = x["dep_time"].s();
            dto.arrTime = x["arr_time"].s();
            dto.status = x["status"].s();
            dto.gate = x["gate"].s();
            dto.aircraftId = x["aircraft_id"].i();
            dto.pilotId = x["pilot_id"].i();
            dto.copilotId = x["copilot_id"].i();

            try {
                flightService->createFlight(dto, getUsername(token));
                return crow::response(201, "Created");
            } catch (const exception& e) {
                return crow::response(400, e.what());
            }
        });

        CROW_ROUTE(app, "/api/admin/flights/<int>").methods("PUT"_method)
        ([this](const crow::request& req, int id) {
            string token = req.get_header_value("Authorization");
            if (token.empty()) return crow::response(401);

            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400);

            FlightRequestDTO dto;
            dto.flightNumber = x["flight_number"].s();
            dto.originCode = x["origin"].s();
            dto.destinationCode = x["destination"].s();
            dto.depTime = x["dep_time"].s();
            dto.arrTime = x["arr_time"].s();
            dto.status = x["status"].s();
            dto.gate = x["gate"].s();
            dto.aircraftId = x["aircraft_id"].i();
            dto.pilotId = x["pilot_id"].i();
            dto.copilotId = x["copilot_id"].i();

            try {
                flightService->updateFlight(id, dto, getUsername(token));
                return crow::response(200, "Updated");
            } catch (const exception& e) {
                return crow::response(400, e.what());
            }
        });

        CROW_ROUTE(app, "/api/admin/flights/<int>").methods("DELETE"_method)
        ([this](const crow::request& req, int id) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403, "Только админ может удалять");

            flightService->deleteFlight(id, getUsername(token));
            return crow::response(200, "Deleted");
        });

        CROW_ROUTE(app, "/api/admin/flights")
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.empty()) return crow::response(401);
            if (token.find("admin:") != 0 && token.find("operator:") != 0) return crow::response(403);

            int page = 1;
            int limit = 20;
            
            if (req.url_params.get("page") != nullptr) {
                try { page = stoi(req.url_params.get("page")); } catch(...) {}
            }
            if (req.url_params.get("limit") != nullptr) {
                try { limit = stoi(req.url_params.get("limit")); } catch(...) {}
            }

            auto paginated = flightService->getPrivateFlights(page, limit);
            
            crow::json::wvalue json = paginatedFlightsToJson(paginated);
            
            return crow::response(200, json);
        });

        CROW_ROUTE(app, "/api/admin/users")
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403);

            auto users = authService->getAllUsers();
            
            vector<crow::json::wvalue> list;
            for (const auto& u : users) {
                list.push_back(userToJson(u));
            }
            
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
            for (const auto& l : logs) {
                list.push_back(auditToJson(l));
            }
            
            crow::json::wvalue result;
            result["logs"] = std::move(list);
            
            return crow::response(200, result);
        });

        CROW_ROUTE(app, "/api/admin/users").methods("POST"_method)
        ([this](const crow::request& req) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403);

            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400);

            CreateUserDTO dto;
            dto.username = x["username"].s();
            dto.password = x["password"].s();
            dto.lastName = x["last_name"].s();
            dto.firstName = x["first_name"].s();
            dto.middleName = x["middle_name"].s();
            dto.role = x["role"].s();

            authService->createUser(dto, getUsername(token));
            return crow::response(201);
        });

        CROW_ROUTE(app, "/api/admin/users/<int>").methods("PUT"_method)
        ([this](const crow::request& req, int id) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403);

            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400);

            CreateUserDTO dto;
            dto.username = x["username"].s();
            dto.password = x["password"].s(); 
            dto.lastName = x["last_name"].s();
            dto.firstName = x["first_name"].s();
            dto.middleName = x["middle_name"].s();
            dto.role = x["role"].s();

            authService->updateUser(id, dto, getUsername(token));
            return crow::response(200);
        });

        CROW_ROUTE(app, "/api/admin/users/<int>").methods("DELETE"_method)
        ([this](const crow::request& req, int id) {
            string token = req.get_header_value("Authorization");
            if (token.find("admin:") != 0) return crow::response(403);
            authService->deleteUser(id, getUsername(token));
            return crow::response(200);
        });

        CROW_ROUTE(app, "/api/validate")
        ([this](const crow::request& req) {
             string token = req.get_header_value("Authorization");
             if (token.empty()) return crow::response(401);
             return crow::response(200);
        });
    }
};