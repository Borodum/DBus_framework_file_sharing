#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct ServiceInfo {
    std::string name;
    std::vector<std::string> formats;
};

void throwDbusError(const std::string& msg) {
    throw sdbus::Error("com.system.sharing.Error", msg);
}


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


#include <cstdlib>
#include <ctime>

std::string getFileExtension(const std::string& path) {
    auto pos = path.find_last_of('.');
    if (pos == std::string::npos) return "";
    return path.substr(pos + 1);
}


int main() {
    try {
        auto connection = sdbus::createSessionBusConnection();
        std::srand(std::time(nullptr));

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
                try{
                    if (name.empty()) {
                        throwDbusError("Service name cannot be empty");
                    }

                    auto it = std::find_if(services.begin(), services.end(),
                        [&](const ServiceInfo& s) { return s.name == name; });

                    if (it != services.end()) {
                        throwDbusError("Service already registered");
                    }

                    ServiceInfo info;
                    info.name = name;
                    info.formats = formats;

                    services.push_back(info);
                    saveServices(services);

                    std::cout << "[RegisterService] " << name << std::endl;

                    std::cout << "Registered formats: ";
                    for (const auto& f : formats) {
                        std::cout << f << " ";
                    }
                    std::cout << std::endl;
                } catch (const sdbus::Error&) {
                    throw;
                } catch (const std::exception& e) {
                    throwDbusError(std::string("Internal error: ") + e.what());
                }
            });

        // OpenFile
        object->registerMethod("OpenFile")
            .onInterface(interfaceName)
            .withInputParamNames("path")
            .implementedAs([&services, &connection](const std::string& path) {
                try{
                    if (path.empty()) {
                        throwDbusError("Path is empty");
                    }

                    std::string ext = getFileExtension(path);

                    if (ext.empty()) {
                        throwDbusError("File has no extension");
                    }

                    std::vector<std::string> candidates;

                    for (const auto& s : services) {
                        if (std::find(s.formats.begin(), s.formats.end(), ext) != s.formats.end()) {
                            candidates.push_back(s.name);
                        }
                    }

                    if (candidates.empty()) {
                        throwDbusError("No service supports this file type");
                    }

                    std::vector<std::string> available;

                    for (const auto& serviceName : candidates) {
                        try {
                            auto proxy = sdbus::createProxy(*connection, serviceName, "/");

                            proxy->finishRegistration();
                            available.push_back(serviceName);

                        } catch (...) {
                            // service isnot avalible
                        }
                    }

                    if (available.empty()) {
                        throwDbusError("No available services running");
                    }

                    int index = std::rand() % available.size();
                    std::string chosen = available[index];

                    std::cout << "[OpenFile] path=" << path << std::endl;

                    std::cout << "Chosen service: " << chosen << std::endl;

                    try {
                        auto proxy = sdbus::createProxy(*connection, chosen, "/");

                        auto method = proxy->createMethodCall(chosen, "OpenFile");
                        method << path;

                        proxy->callMethod(method);

                    } catch (const std::exception& e) {
                        throw sdbus::Error("com.system.sharing.Error",
                                        std::string("Failed to call chosen service: ") + e.what());
                    }
                } catch (const sdbus::Error&) {
                    throw;
                } catch (const std::exception& e) {
                    throwDbusError(std::string("Internal error: ") + e.what());
                }
            });

        // OpenFileUsingService
        object->registerMethod("OpenFileUsingService")
            .onInterface(interfaceName)
            .withInputParamNames("path", "service")
            .implementedAs([&services, &connection](const std::string& path, const std::string& serviceName) {
                try{

                    auto it = std::find_if(services.begin(), services.end(),
                        [&](const ServiceInfo& s) { return s.name == serviceName; });

                    if (it == services.end()) {
                        throwDbusError("Service not registered");
                    }

                    try {
                        auto proxy = sdbus::createProxy(*connection, serviceName, "/");
                        auto method = proxy->createMethodCall(serviceName, "OpenFile");

                        method << path;
                        proxy->callMethod(method);

                    } catch (const std::exception& e) {
                        throw sdbus::Error("com.system.sharing.Error",
                                        std::string("Failed to call service: ") + e.what());
                    }

                    std::cout << "[OpenFileUsingService] path=" << path << " service=" << serviceName << std::endl;
                    std::cout << "[OpenFileUsingService] success" << std::endl;

                } catch (const sdbus::Error&) {
                    throw;
                } catch (const std::exception& e) {
                    throwDbusError(std::string("Internal error: ") + e.what());
                }
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