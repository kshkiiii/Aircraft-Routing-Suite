#pragma once
#include "crow_all.h"
#include "../services/AuthService.hpp"

using namespace std;

class AuthController {
private:
    shared_ptr<AuthService> service;

public:
    AuthController(shared_ptr<AuthService> s) : service(s) {}

    void registerRoutes(crow::SimpleApp& app) {
        
        CROW_ROUTE(app, "/api/login").methods("POST"_method)
        ([this](const crow::request& req) {
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400, "Некорректный JSON");

            string user = x["username"].s();
            string pass = x["password"].s();

            auto result = service->login(user, pass);
            
            if (result) {
                crow::json::wvalue json;
                json["token"] = result->token;
                json["role"] = result->role;
                json["full_name"] = result->fullName;
                return crow::response(200, json);
            } else {
                return crow::response(401, "Неверный логин или пароль");
            }
        });
    }
};