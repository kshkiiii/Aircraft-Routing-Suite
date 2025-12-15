#pragma once

#include <string>
#include <memory>
#include <pqxx/pqxx>

using namespace std;

class Database {
private:
    string connection_string;

public:
    Database();
    unique_ptr<pqxx::connection> get_connection();
};