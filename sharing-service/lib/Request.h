#pragma once

#include <string>

class Request {
public:
    Request(const std::string& path);

    std::string getPath() const;

    void sendSuccessResponse();
    void sendErrorResponse(const std::string& message);

private:
    std::string path_;
};