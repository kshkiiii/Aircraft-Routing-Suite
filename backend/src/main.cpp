#include "crow_all.h"
#include "config/Database.hpp"
#include "repositories/FlightRepository.hpp"
#include "repositories/UserRepository.hpp"
#include "repositories/AuditRepository.hpp" 
#include "services/FlightService.hpp"
#include "services/AuthService.hpp"
#include "controllers/FlightController.hpp"
#include "controllers/AuthController.hpp"
#include "controllers/AdminFlightController.hpp"
#include "utils/CryptoUtils.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main() {
    auto db = make_shared<Database>();
    
    for (int i = 0; i < 10; ++i) {
        try {
            if (db->get_connection()->is_open()) {
                cout << "База данных подключена" << endl;
                break;
            }
        } catch (...) {
            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    auto flightRepo = make_shared<FlightRepository>(db);
    auto userRepo = make_shared<UserRepository>(db);
    auto auditRepo = make_shared<AuditRepository>(db);

    auto flightService = make_shared<FlightService>(flightRepo, auditRepo);
    auto authService = make_shared<AuthService>(userRepo, auditRepo);

    auto flightController = make_shared<FlightController>(flightService);
    auto authController = make_shared<AuthController>(authService);
    auto adminController = make_shared<AdminFlightController>(flightService, authService, auditRepo); 

    crow::SimpleApp app;

    flightController->registerRoutes(app);
    authController->registerRoutes(app);
    adminController->registerRoutes(app);

    CROW_ROUTE(app, "/")([](const crow::request&, crow::response& res){
        res.set_static_file_info("static/index.html");
        res.end();
    });

    CROW_ROUTE(app, "/login")([](const crow::request&, crow::response& res){
        res.set_static_file_info("static/login.html");
        res.end();
    });
    
    CROW_ROUTE(app, "/admin")([](const crow::request&, crow::response& res){
        res.set_static_file_info("static/admin.html");
        res.end();
    });

    CROW_ROUTE(app, "/css/<string>")
    ([](const crow::request&, crow::response& res, string filename){
        res.set_static_file_info("static/css/" + filename);
        res.end();
    });

    CROW_ROUTE(app, "/js/<string>")
    ([](const crow::request&, crow::response& res, string filename){
        res.set_static_file_info("static/js/" + filename);
        res.end();
    });

    cout << "Сервер запущен на порту 8080" << endl;
    app.port(8080).multithreaded().run();
}