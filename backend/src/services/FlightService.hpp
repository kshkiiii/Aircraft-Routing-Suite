#pragma once
#include "../repositories/FlightRepository.hpp"
#include "../repositories/AuditRepository.hpp"

using namespace std;

class FlightService {
private:
    shared_ptr<FlightRepository> repo;
    shared_ptr<AuditRepository> audit;

public:
    FlightService(shared_ptr<FlightRepository> r, shared_ptr<AuditRepository> a = nullptr) 
        : repo(r), audit(a) {}

    vector<PublicFlightDTO> getDepartures() { return repo->getPublicFlights(false); }
    vector<PublicFlightDTO> getArrivals() { return repo->getPublicFlights(true); }
    vector<PrivateFlightDTO> getPrivateFlights() { return repo->getAllPrivateFlights(); }

    ResourcesDTO getResources() {
        return repo->getResources();
    }

    void createFlight(const FlightRequestDTO& dto, const string& username) {
        int id = repo->create(dto);
        if (audit) audit->saveLog(username, "CREATE", "flights", id, "Создан рейс " + dto.flightNumber);
    }

    void updateFlight(int id, const FlightRequestDTO& dto, const string& username) {
        repo->update(id, dto);
        if (audit) audit->saveLog(username, "UPDATE", "flights", id, "Обновлен рейс " + dto.flightNumber);
    }

    void deleteFlight(int id, const string& username) {
        string flightNum = repo->getFlightNumberById(id);
        
        repo->remove(id);

        if (audit) audit->saveLog(username, "DELETE", "flights", id, "Удален рейс " + flightNum);
    }
};