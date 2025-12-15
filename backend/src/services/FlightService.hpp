#pragma once
#include "../repositories/FlightRepository.hpp"

using namespace std;

class FlightService {
private:
    shared_ptr<FlightRepository> repo;

public:
    FlightService(shared_ptr<FlightRepository> r) : repo(r) {}

    vector<PublicFlightDTO> getDepartures() {
        return repo->getPublicFlights(false);
    }

    vector<PublicFlightDTO> getArrivals() {
        return repo->getPublicFlights(true);
    }

    vector<PrivateFlightDTO> getPrivateFlights() {
        return repo->getAllPrivateFlights();
    }
};