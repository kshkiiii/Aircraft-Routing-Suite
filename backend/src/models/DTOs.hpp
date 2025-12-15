#pragma once
#include <string>
#include <vector>
#include "crow_all.h"

using namespace std;

struct PublicFlightDTO {
    string flightNumber;
    string originCity;
    string destinationCity;
    string scheduledTime;
    string status;
    string gate;
};

struct PrivateFlightDTO : public PublicFlightDTO {
    int id;
    string arrivalTime;     
    string aircraftModel;
    string pilotName;      
    string copilotName;     
    int passengersCount;
};

struct LoginResponse {
    string token;
    string role;
    string fullName;
};

struct UserDTO {
    int id;
    string username;
    string lastName;   
    string firstName;
    string middleName;
    string role;
};

struct AuditDTO {
    int id;
    string username;   
    string action;      
    string table;       
    string time;
    string details;     
};

inline crow::json::wvalue flightToJson(const PublicFlightDTO& f) {
    crow::json::wvalue x;
    x["flight_number"] = f.flightNumber;
    x["origin"] = f.originCity;
    x["destination"] = f.destinationCity;
    x["time"] = f.scheduledTime;
    x["status"] = f.status;
    x["gate"] = f.gate;
    return x;
}

inline crow::json::wvalue privateFlightToJson(const PrivateFlightDTO& f) {
    crow::json::wvalue x;
    x["id"] = f.id;
    x["flight_number"] = f.flightNumber;
    x["origin"] = f.originCity;
    x["destination"] = f.destinationCity;
    x["dep_time"] = f.scheduledTime;    
    x["arr_time"] = f.arrivalTime;      
    x["status"] = f.status;
    x["gate"] = f.gate;
    x["aircraft"] = f.aircraftModel;
    x["pilot"] = f.pilotName;
    x["copilot"] = f.copilotName;
    x["passengers"] = f.passengersCount;
    return x;
}

inline crow::json::wvalue userToJson(const UserDTO& u) {
    crow::json::wvalue x;
    x["id"] = u.id;
    x["username"] = u.username;
    x["last_name"] = u.lastName;
    x["first_name"] = u.firstName;
    x["middle_name"] = u.middleName;
    x["role"] = u.role;
    return x;
}

inline crow::json::wvalue auditToJson(const AuditDTO& a) {
    crow::json::wvalue x;
    x["id"] = a.id;
    x["user"] = a.username;
    x["action"] = a.action;
    x["table"] = a.table;
    x["time"] = a.time;
    x["details"] = a.details;
    return x;
}