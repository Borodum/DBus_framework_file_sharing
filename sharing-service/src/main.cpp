#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

int main() {
    try {
        auto connection = sdbus::createSessionBusConnection();

        const char* serviceName = "com.system.sharing";
        connection->requestName(serviceName);

        auto object = sdbus::createObject(*connection, "/");

        object->registerMethod("Dummy")
            .onInterface("com.system.sharing")
            .implementedAs([]() {
            });

        object->finishRegistration();

        std::cout << "DBus service '" << serviceName << "' started..." << std::endl;

        connection->enterEventLoop();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}