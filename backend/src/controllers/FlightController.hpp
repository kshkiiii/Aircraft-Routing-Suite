#pragma once
#include "crow_all.h"
#include "../services/FlightService.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

class FlightController {
private:
    shared_ptr<FlightService> service;

    string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
        return ss.str();
    }

public:
    FlightController(shared_ptr<FlightService> s) : service(s) {}

    void registerRoutes(crow::SimpleApp& app) {
        
        CROW_ROUTE(app, "/api/public/departures")
        ([this](const crow::request& req) {
            string date = req.url_params.get("date") ? req.url_params.get("date") : getCurrentDate();
            
            auto flights = service->getDepartures(date);
            
            vector<crow::json::wvalue> jsonList;
            for (const auto& f : flights) {
                jsonList.push_back(flightToJson(f));
            }
            crow::json::wvalue finalJson;
            finalJson["flights"] = std::move(jsonList);
            return finalJson;
        });

        CROW_ROUTE(app, "/api/public/arrivals")
        ([this](const crow::request& req) {
            string date = req.url_params.get("date") ? req.url_params.get("date") : getCurrentDate();

            auto flights = service->getArrivals(date);
            
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