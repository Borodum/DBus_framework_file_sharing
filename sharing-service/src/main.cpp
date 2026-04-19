#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct ServiceInfo {
    std::string name;
    std::vector<std::string> formats;
};

int main() {
    try {
        auto connection = sdbus::createSessionBusConnection();

        const char* serviceName = "com.system.sharing";
        const char* interfaceName = "com.system.sharing";

        connection->requestName(serviceName);

        auto object = sdbus::createObject(*connection, "/");

        std::vector<ServiceInfo> services;

        // RegisterService
        object->registerMethod("RegisterService")
            .onInterface(interfaceName)
            .withInputParamNames("name", "supportedFormats")
            .implementedAs([&services](const std::string& name, const std::vector<std::string>& formats) {

                std::cout << "[RegisterService] " << name << std::endl;

                // 🔁 Проверяем, не зарегистрирован ли уже
                auto it = std::find_if(services.begin(), services.end(),
                    [&](const ServiceInfo& s) { return s.name == name; });

                if (it != services.end()) {
                    throw sdbus::Error("com.system.sharing.Error", "Service already registered");
                }

                ServiceInfo info;
                info.name = name;
                info.formats = formats;

                services.push_back(info);

                std::cout << "Registered formats: ";
                for (const auto& f : formats) {
                    std::cout << f << " ";
                }
                std::cout << std::endl;
            });

        // OpenFile
        object->registerMethod("OpenFile")
            .onInterface(interfaceName)
            .withInputParamNames("path")
            .implementedAs([](const std::string& path) {
                std::cout << "[OpenFile] path=" << path << std::endl;
            });

        // OpenFileUsingService
        object->registerMethod("OpenFileUsingService")
            .onInterface(interfaceName)
            .withInputParamNames("path", "service")
            .implementedAs([](const std::string& path, const std::string& service) {
                std::cout << "[OpenFileUsingService] path=" << path
                          << " service=" << service << std::endl;
            });

        object->finishRegistration();

        std::cout << "Service registry initialized..." << std::endl;

        connection->enterEventLoop();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}