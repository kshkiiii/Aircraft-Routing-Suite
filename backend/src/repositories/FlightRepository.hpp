#pragma once
#include "../config/Database.hpp"
#include "../models/DTOs.hpp"
#include <vector>
#include <memory>

using namespace std;

class FlightRepository {
private:
    shared_ptr<Database> db;

public:
    FlightRepository(shared_ptr<Database> database) : db(database) {}

    vector<PublicFlightDTO> getPublicFlights(bool isArrival) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        string timeField = isArrival ? "f.scheduled_arrival" : "f.scheduled_departure";
        
        string sql = "SELECT f.flight_number, dep.city, arr.city, "
                     "to_char(" + timeField + ", 'HH24:MI'), f.status, COALESCE(f.gate, '---') "
                     "FROM flights f "
                     "JOIN airports dep ON f.departure_airport_code = dep.code "
                     "JOIN airports arr ON f.arrival_airport_code = arr.code "
                     "ORDER BY " + timeField + " ASC";

        pqxx::result res = txn.exec(sql);
        vector<PublicFlightDTO> flights;
        for (auto row : res) {
            flights.push_back({row[0].c_str(), row[1].c_str(), row[2].c_str(), row[3].c_str(), row[4].c_str(), row[5].c_str()});
        }
        return flights;
    }

    vector<PrivateFlightDTO> getAllPrivateFlights() {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        
        string sql = "SELECT f.id, f.flight_number, dep.city, arr.city, "
                     "to_char(f.scheduled_departure, 'DD.MM HH24:MI'), "
                     "to_char(f.scheduled_arrival, 'DD.MM HH24:MI'), " 
                     "f.status, COALESCE(f.gate, '---'), "
                     "COALESCE(a.model, 'Не назначен'), "
                     "COALESCE(p1.last_name || ' ' || p1.first_name, 'Не назначен'), " 
                     "COALESCE(p2.last_name || ' ' || p2.first_name, 'Не назначен'), " 
                     "f.passengers_count "
                     "FROM flights f "
                     "JOIN airports dep ON f.departure_airport_code = dep.code "
                     "JOIN airports arr ON f.arrival_airport_code = arr.code "
                     "LEFT JOIN aircrafts a ON f.aircraft_id = a.id "
                     "LEFT JOIN pilots p1 ON f.pilot_id = p1.id "
                     "LEFT JOIN pilots p2 ON f.copilot_id = p2.id "
                     "ORDER BY f.scheduled_departure DESC";

        pqxx::result res = txn.exec(sql);
        vector<PrivateFlightDTO> flights;
        for (auto row : res) {
            PrivateFlightDTO dto;
            dto.id = row[0].as<int>();
            dto.flightNumber = row[1].c_str();
            dto.originCity = row[2].c_str();
            dto.destinationCity = row[3].c_str();
            dto.scheduledTime = row[4].c_str(); 
            dto.arrivalTime = row[5].c_str();  
            dto.status = row[6].c_str();
            dto.gate = row[7].c_str();
            dto.aircraftModel = row[8].c_str();
            dto.pilotName = row[9].c_str();
            dto.copilotName = row[10].c_str();
            dto.passengersCount = row[11].as<int>();
            flights.push_back(dto);
        }
        return flights;
    }
};