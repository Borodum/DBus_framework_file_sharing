#include "Request.h"
#include <iostream>

Request::Request(const std::string& path)
    : path_(path)
{
}

std::string Request::getPath() const {
    return path_;
}

void Request::sendSuccessResponse() {
}

void Request::sendErrorResponse(const std::string& message) {
    throw std::runtime_error(message);
}