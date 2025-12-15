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
        string sql = "SELECT f.flight_number, dep.city, arr.city, to_char(" + timeField + ", 'HH24:MI'), f.status, COALESCE(f.gate, '---') "
                     "FROM flights f JOIN airports dep ON f.departure_airport_code = dep.code "
                     "JOIN airports arr ON f.arrival_airport_code = arr.code ORDER BY " + timeField + " ASC";
        pqxx::result res = txn.exec(sql);
        vector<PublicFlightDTO> flights;
        for (auto row : res) flights.push_back({row[0].c_str(), row[1].c_str(), row[2].c_str(), row[3].c_str(), row[4].c_str(), row[5].c_str()});
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
                     "f.passengers_count, "
                     "COALESCE(f.aircraft_id, 0), "           
                     "COALESCE(f.pilot_id, 0), "             
                     "COALESCE(f.copilot_id, 0), "             
                     "to_char(f.scheduled_departure, 'YYYY-MM-DD\"T\"HH24:MI'), " 
                     "to_char(f.scheduled_arrival, 'YYYY-MM-DD\"T\"HH24:MI') "   
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
            
            dto.aircraftId = row[12].as<int>();
            dto.pilotId = row[13].as<int>();
            dto.copilotId = row[14].as<int>();
            dto.depTimeIso = row[15].c_str();
            dto.arrTimeIso = row[16].c_str();

            flights.push_back(dto);
        }
        return flights;
    }

    ResourcesDTO getResources() {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        ResourcesDTO res;
        for (auto row : txn.exec("SELECT code, city || ' (' || code || ')' FROM airports")) res.airports.push_back({row[0].c_str(), row[1].c_str()});
        for (auto row : txn.exec("SELECT id, model || ' - ' || registration_number FROM aircrafts")) res.aircrafts.push_back({row[0].c_str(), row[1].c_str()});
        for (auto row : txn.exec("SELECT id, last_name || ' ' || first_name || ' (' || rank || ')' FROM pilots")) res.pilots.push_back({row[0].c_str(), row[1].c_str()});
        return res;
    }

    string getFlightNumberById(int id) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec("SELECT flight_number FROM flights WHERE id = " + to_string(id));
        if (res.empty()) return "UNKNOWN";
        return res[0][0].c_str();
    }

    int create(const FlightRequestDTO& dto) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);

        string aircraft = (dto.aircraftId <= 0) ? "NULL" : to_string(dto.aircraftId);
        string pilot = (dto.pilotId <= 0) ? "NULL" : to_string(dto.pilotId);
        string copilot = (dto.copilotId <= 0) ? "NULL" : to_string(dto.copilotId);
        
        string dep = dto.depTime.empty() ? "NULL" : txn.quote(dto.depTime);
        string arr = dto.arrTime.empty() ? "NULL" : txn.quote(dto.arrTime);

        string sql = "INSERT INTO flights (flight_number, departure_airport_code, arrival_airport_code, "
                     "scheduled_departure, scheduled_arrival, status, gate, aircraft_id, pilot_id, copilot_id) "
                     "VALUES (" +
                     txn.quote(dto.flightNumber) + ", " + txn.quote(dto.originCode) + ", " + txn.quote(dto.destinationCode) + ", " +
                     dep + ", " + arr + ", " + txn.quote(dto.status) + ", " + txn.quote(dto.gate) + ", " +
                     aircraft + ", " + pilot + ", " + copilot + ") RETURNING id";

        pqxx::result res = txn.exec(sql);
        txn.commit();
        return res[0][0].as<int>();
    }

    void update(int id, const FlightRequestDTO& dto) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);

        string aircraft = (dto.aircraftId <= 0) ? "NULL" : to_string(dto.aircraftId);
        string pilot = (dto.pilotId <= 0) ? "NULL" : to_string(dto.pilotId);
        string copilot = (dto.copilotId <= 0) ? "NULL" : to_string(dto.copilotId);

        string dep = dto.depTime.empty() ? "NULL" : txn.quote(dto.depTime);
        string arr = dto.arrTime.empty() ? "NULL" : txn.quote(dto.arrTime);

        string sql = "UPDATE flights SET "
                     "flight_number = " + txn.quote(dto.flightNumber) + ", "
                     "departure_airport_code = " + txn.quote(dto.originCode) + ", "
                     "arrival_airport_code = " + txn.quote(dto.destinationCode) + ", "
                     "scheduled_departure = " + dep + ", "
                     "scheduled_arrival = " + arr + ", "
                     "status = " + txn.quote(dto.status) + ", "
                     "gate = " + txn.quote(dto.gate) + ", "
                     "aircraft_id = " + aircraft + ", "
                     "pilot_id = " + pilot + ", "
                     "copilot_id = " + copilot + " "
                     "WHERE id = " + to_string(id);

        txn.exec(sql);
        txn.commit();
    }

    void remove(int id) {
        auto conn = db->get_connection();
        pqxx::work txn(*conn);
        txn.exec("DELETE FROM flights WHERE id = " + to_string(id));
        txn.commit();
    }
};