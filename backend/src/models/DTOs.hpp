#pragma once
#include <string>
#include <vector>
#include "crow_all.h"

using namespace std;

struct PublicFlightDTO {
    string flightNumber;
    string originCity;
    string destinationCity;
    string scheduledTime; // отформатированное время
    string status;
    string gate;
};

// конвертация JSON Crow
crow::json::wvalue flightToJson(const PublicFlightDTO& f) {
    crow::json::wvalue x;
    x["flight_number"] = f.flightNumber;
    x["origin"] = f.originCity;
    x["destination"] = f.destinationCity;
    x["time"] = f.scheduledTime;
    x["status"] = f.status;
    x["gate"] = f.gate;
    return x;
}