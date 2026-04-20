#include "SharingService.h"
#include <iostream>

SharingService::SharingService(const std::string& serviceName,
                               const std::vector<std::string>& supportedFormats,
                               OpenFileCallback callback)
    : serviceName_(serviceName),
      formats_(supportedFormats),
      callback_(callback)
{
}

void SharingService::registerToMainService() {
    try {
        auto proxy = sdbus::createProxy(*connection_, "com.system.sharing", "/");

        auto method = proxy->createMethodCall("com.system.sharing", "RegisterService");
        method << serviceName_ << formats_;

        proxy->callMethod(method);

        std::cout << "Registered in main sharing service\n";

    } catch (const std::exception& e) {
        std::cerr << "Failed to register in main service: " << e.what() << std::endl;
        throw;
    }
}

int SharingService::start() {
    try {
        connection_ = sdbus::createSessionBusConnection();

        connection_->requestName(serviceName_);

        object_ = sdbus::createObject(*connection_, "/");

        object_->registerMethod("OpenFile")
            .onInterface(serviceName_)
            .withInputParamNames("path")
            .implementedAs([this](const std::string& path) {

                try {
                    // создаём фейковый Request без MethodCall
                    Request req(path);

                    callback_(req);

                } catch (const std::exception& e) {
                    throw sdbus::Error("com.system.sharing.Error", e.what());
                }
            });

        object_->finishRegistration();


        registerToMainService();

        std::cout << "SharingService started: " << serviceName_ << std::endl;

        connection_->enterEventLoop();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}