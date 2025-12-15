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

        // если прилет, сортируем по прибытию, иначе по вылету
        string timeField = isArrival ? "f.scheduled_arrival" : "f.scheduled_departure";
        
        string sql = "SELECT f.flight_number, dep.city, arr.city, "
                     "to_char(" + timeField + ", 'HH24:MI') as formatted_time, "
                     "f.status, COALESCE(f.gate, '---') "
                     "FROM flights f "
                     "JOIN airports dep ON f.departure_airport_code = dep.code "
                     "JOIN airports arr ON f.arrival_airport_code = arr.code "
                     "ORDER BY " + timeField + " ASC";

        pqxx::result res = txn.exec(sql);
        
        vector<PublicFlightDTO> flights;
        for (auto row : res) {
            flights.push_back({
                row[0].c_str(), 
                row[1].c_str(), 
                row[2].c_str(), 
                row[3].c_str(), 
                row[4].c_str(), 
                row[5].c_str()
            });
        }
        return flights;
    }
};