#pragma once
#include "../repositories/FlightRepository.hpp"
#include "../repositories/AuditRepository.hpp"
#include <stdexcept>

using namespace std;

class FlightService {
private:
    shared_ptr<FlightRepository> repo;
    shared_ptr<AuditRepository> audit;

    void validateFlightRules(const FlightRequestDTO& dto, int excludeId = -1) {
        if (dto.originCode == dto.destinationCode) {
            throw runtime_error("Аэропорт вылета и прилета не могут совпадать");
        }

        if (dto.depTime >= dto.arrTime) {
            throw runtime_error("Дата вылета должна быть раньше даты прилета");
        }

        if (dto.pilotId > 0 && dto.pilotId == dto.copilotId) {
             throw runtime_error("Пилот и второй пилот не могут быть одним человеком");
        }

        if (repo->isAircraftBusy(dto.aircraftId, dto.depTime, dto.arrTime, excludeId)) {
            throw runtime_error("Выбранный самолет уже задействован в другом рейсе в это время");
        }

        if (repo->isPilotBusy(dto.pilotId, dto.depTime, dto.arrTime, excludeId)) {
            throw runtime_error("Командир судна (КВС) занят на другом рейсе в это время");
        }

        if (repo->isPilotBusy(dto.copilotId, dto.depTime, dto.arrTime, excludeId)) {
            throw runtime_error("Второй пилот занят на другом рейсе в это время");
        }
    }

public:
    FlightService(shared_ptr<FlightRepository> r, shared_ptr<AuditRepository> a = nullptr) 
        : repo(r), audit(a) {}

    vector<PublicFlightDTO> getDepartures(const string& date) { return repo->getPublicFlights(false, date); }
    vector<PublicFlightDTO> getArrivals(const string& date) { return repo->getPublicFlights(true, date); }
    
    PaginatedResponse<PrivateFlightDTO> getPrivateFlights(int page, int limit) {
        if (page < 1) page = 1;
        if (limit < 1) limit = 10;
        int offset = (page - 1) * limit;
        auto data = repo->getAllPrivateFlights(limit, offset);
        int total = repo->countAllFlights();
        return { data, total, page, limit };
    }

    ResourcesDTO getResources() {
        return repo->getResources();
    }

    void createFlight(const FlightRequestDTO& dto, const string& username) {
        validateFlightRules(dto);
        int id = repo->create(dto);
        if (audit) audit->saveLog(username, "CREATE", "flights", id, "Создан рейс " + dto.flightNumber);
    }

    void updateFlight(int id, const FlightRequestDTO& dto, const string& username) {
        validateFlightRules(dto, id);
        repo->update(id, dto);
        if (audit) audit->saveLog(username, "UPDATE", "flights", id, "Обновлен рейс " + dto.flightNumber);
    }

    void deleteFlight(int id, const string& username) {
        string flightNum = repo->getFlightNumberById(id);
        repo->remove(id);
        if (audit) audit->saveLog(username, "DELETE", "flights", id, "Удален рейс " + flightNum);
    }
};