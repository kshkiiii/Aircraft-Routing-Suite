#include "crow_all.h"
#include "config/Database.hpp"
#include "repositories/FlightRepository.hpp"
#include "services/FlightService.hpp"
#include "controllers/FlightController.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main() {
    auto db = make_shared<Database>();
    int max_retries = 10;
    
    for (int i = 0; i < max_retries; ++i) {
        try {
            auto conn = db->get_connection();
            if (conn->is_open()) {
                cout << "БД подключена!" << endl;
                break;
            }
        } catch (const exception& e) {
            if (i == max_retries - 1) return 1;
            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    auto flightRepo = make_shared<FlightRepository>(db);
    auto flightService = make_shared<FlightService>(flightRepo);
    auto flightController = make_shared<FlightController>(flightService);

    crow::SimpleApp app;

    flightController->registerRoutes(app);

    CROW_ROUTE(app, "/")([](const crow::request&, crow::response& res){
        res.set_static_file_info("static/index.html");
        res.end();
    });

    cout << "Сервер запущен на порту 8080" << endl;
    app.port(8080).multithreaded().run();
}