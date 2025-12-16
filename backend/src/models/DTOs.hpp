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

    int aircraftId;
    int pilotId;
    int copilotId;
    string depTimeIso; 
    string arrTimeIso;
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

template <typename T>
struct PaginatedResponse {
    vector<T> data;
    int total;
    int page;
    int pageSize;
};

struct FlightRequestDTO {
    string flightNumber;
    string originCode;
    string destinationCode;
    string depTime;        
    string arrTime;
    string status;
    string gate;
    int aircraftId;         
    int pilotId;
    int copilotId;
};

struct ResourceItem {
    string id;              
    string name;            
};

struct ResourcesDTO {
    vector<ResourceItem> airports;
    vector<ResourceItem> aircrafts;
    vector<ResourceItem> pilots;
};

struct CreateUserDTO {
    string username;
    string password; 
    string lastName;
    string firstName;
    string middleName;
    string role;
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

    x["aircraft_id"] = f.aircraftId;
    x["pilot_id"] = f.pilotId;
    x["copilot_id"] = f.copilotId;
    x["dep_time_iso"] = f.depTimeIso;
    x["arr_time_iso"] = f.arrTimeIso;

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

inline crow::json::wvalue resourcesToJson(const ResourcesDTO& r) {
    crow::json::wvalue x;
    
    vector<crow::json::wvalue> air;
    for(const auto& i : r.airports) { crow::json::wvalue v; v["id"]=i.id; v["name"]=i.name; air.push_back(v); }
    x["airports"] = std::move(air);

    vector<crow::json::wvalue> ac;
    for(const auto& i : r.aircrafts) { crow::json::wvalue v; v["id"]=i.id; v["name"]=i.name; ac.push_back(v); }
    x["aircrafts"] = std::move(ac);

    vector<crow::json::wvalue> pil;
    for(const auto& i : r.pilots) { crow::json::wvalue v; v["id"]=i.id; v["name"]=i.name; pil.push_back(v); }
    x["pilots"] = std::move(pil);

    return x;
}

inline crow::json::wvalue paginatedFlightsToJson(const PaginatedResponse<PrivateFlightDTO>& p) {
    crow::json::wvalue x;
    x["total"] = p.total;
    x["page"] = p.page;
    x["limit"] = p.pageSize;
    
    vector<crow::json::wvalue> list;
    for (const auto& item : p.data) {
        list.push_back(privateFlightToJson(item));
    }
    x["data"] = std::move(list);
    
    return x;
}