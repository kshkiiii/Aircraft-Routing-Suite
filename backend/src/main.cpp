#include "crow_all.h"
#include <iostream>

using namespace std;

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/health")([](){
        return crow::response(200, "Сервер работает корректно");
    });

    cout << "Запуск сервера на порту 8080..." << endl;
    
    app.port(8080).multithreaded().run();
}