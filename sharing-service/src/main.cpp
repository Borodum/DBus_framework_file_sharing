#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct ServiceInfo {
    std::string name;
    std::vector<std::string> formats;
};


#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

void saveServices(const std::vector<ServiceInfo>& services) {
    json j = json::array();

    for (const auto& s : services) {
        j.push_back({
            {"name", s.name},
            {"formats", s.formats}
        });
    }

    std::ofstream file("services.json");
    file << j.dump(4);
}


std::vector<ServiceInfo> loadServices() {
    std::vector<ServiceInfo> services;

    std::ifstream file("services.json");
    if (!file.is_open()) {
        return services;
    }

    json j;
    file >> j;

    for (const auto& item : j) {
        ServiceInfo s;
        s.name = item.at("name").get<std::string>();
        s.formats = item.at("formats").get<std::vector<std::string>>();
        services.push_back(s);
    }

    return services;
}


int main() {
    try {
        auto connection = sdbus::createSessionBusConnection();

        const char* serviceName = "com.system.sharing";
        const char* interfaceName = "com.system.sharing";

        connection->requestName(serviceName);

        auto object = sdbus::createObject(*connection, "/");

        std::vector<ServiceInfo> services = loadServices();
        std::cout << "Loaded services: " << services.size() << std::endl;

        // RegisterService
        object->registerMethod("RegisterService")
            .onInterface(interfaceName)
            .withInputParamNames("name", "supportedFormats")
            .implementedAs([&services](const std::string& name, const std::vector<std::string>& formats) {

                std::cout << "[RegisterService] " << name << std::endl;

                auto it = std::find_if(services.begin(), services.end(),
                    [&](const ServiceInfo& s) { return s.name == name; });

                if (it != services.end()) {
                    throw sdbus::Error("com.system.sharing.Error", "Service already registered");
                }

                ServiceInfo info;
                info.name = name;
                info.formats = formats;

                services.push_back(info);
                saveServices(services);

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