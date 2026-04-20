#include "../lib/SharingService.h"
#include <iostream>

int main() {
    SharingService service(
        "com.system.sharing.text",
        {"txt", "md"},
        [](Request& req) {
            std::string path = req.getPath();

            std::cout << "Opening file: " << path << std::endl;

            if (path.empty()) {
                req.sendErrorResponse("Empty path");
                return;
            }

            std::cout << "File opened successfully\n";
            req.sendSuccessResponse();
        }
    );

    return service.start();
}