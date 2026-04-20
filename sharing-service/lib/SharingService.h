#pragma once

#include <sdbus-c++/sdbus-c++.h>
#include <functional>
#include <string>
#include <vector>

class SharingService {
public:
    using OpenFileCallback = std::function<void(const std::string& path)>;

    SharingService(const std::string& serviceName,
                   const std::vector<std::string>& supportedFormats,
                   OpenFileCallback callback);

    int start();

private:
    std::string serviceName_;
    std::vector<std::string> formats_;
    OpenFileCallback callback_;

    std::unique_ptr<sdbus::IConnection> connection_;
    std::unique_ptr<sdbus::IObject> object_;

    void registerToMainService();
};