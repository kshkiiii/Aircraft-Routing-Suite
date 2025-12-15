#include "Database.hpp"
#include <iostream>
#include <cstdlib>

using namespace std;

Database::Database() {
    const char* host = getenv("DB_HOST");
    const char* port = getenv("DB_PORT");
    const char* user = getenv("DB_USER");
    const char* pass = getenv("DB_PASS");
    const char* name = getenv("DB_NAME");

    if (!host || !port || !user || !pass || !name) {
        cerr << "Ошибка: Переменные окружения БД не заданы!" << endl;
        exit(1);
    }

    connection_string = "host=" + string(host) +
                        " port=" + string(port) +
                        " user=" + string(user) +
                        " password=" + string(pass) +
                        " dbname=" + string(name);
}

unique_ptr<pqxx::connection> Database::get_connection() {
    try {
        return make_unique<pqxx::connection>(connection_string);
    } catch (const exception& e) {
        cerr << "Ошибка подключения к БД: " << e.what() << endl;
        throw;
    }
}