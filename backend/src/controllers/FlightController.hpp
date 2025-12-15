#pragma once
#include "crow_all.h"
#include "../services/FlightService.hpp"

using namespace std;

class FlightController {
private:
    shared_ptr<FlightService> service;

public:
    FlightController(shared_ptr<FlightService> s) : service(s) {}

    void registerRoutes(crow::SimpleApp& app) {
        
        CROW_ROUTE(app, "/api/public/departures")
        ([this]() {
            auto flights = service->getDepartures();
            vector<crow::json::wvalue> jsonList;
            for (const auto& f : flights) {
                jsonList.push_back(flightToJson(f));
            }
            crow::json::wvalue finalJson;
            finalJson["flights"] = std::move(jsonList);
            return finalJson;
        });

        CROW_ROUTE(app, "/api/public/arrivals")
        ([this]() {
            auto flights = service->getArrivals();
            vector<crow::json::wvalue> jsonList;
            for (const auto& f : flights) {
                jsonList.push_back(flightToJson(f));
            }
            crow::json::wvalue finalJson;
            finalJson["flights"] = std::move(jsonList);
            return finalJson;
        });
    }
};