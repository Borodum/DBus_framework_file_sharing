#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <vector>

int main() {
    try {
        auto connection = sdbus::createSessionBusConnection();

        const char* serviceName = "com.system.sharing";
        const char* interfaceName = "com.system.sharing";

        connection->requestName(serviceName);

        auto object = sdbus::createObject(*connection, "/");

        // RegisterService(name: String, supportedFormats: Array<String>)
        object->registerMethod("RegisterService")
            .onInterface(interfaceName)
            .withInputParamNames("name", "supportedFormats")
            .implementedAs([](const std::string& name, const std::vector<std::string>& formats) {
                std::cout << "[RegisterService] name=" << name << "\nformats: ";
                for (const auto& f : formats) {
                    std::cout << f << " ";
                }
                std::cout << std::endl;
            });

        // OpenFile(path: String)
        object->registerMethod("OpenFile")
            .onInterface(interfaceName)
            .withInputParamNames("path")
            .implementedAs([](const std::string& path) {
                std::cout << "[OpenFile] path=" << path << std::endl;
            });

        // OpenFileUsingService(path: String, service: String)
        object->registerMethod("OpenFileUsingService")
            .onInterface(interfaceName)
            .withInputParamNames("path", "service")
            .implementedAs([](const std::string& path, const std::string& service) {
                std::cout << "[OpenFileUsingService] path=" << path
                          << " service=" << service << std::endl;
            });

        object->finishRegistration();

        std::cout << "DBus service with methods started..." << std::endl;

        connection->enterEventLoop();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}