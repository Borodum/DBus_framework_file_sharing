cmake_minimum_required(VERSION 3.14)
project(sharing-service)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDBUSCPP REQUIRED sdbus-c++)

include_directories(${SDBUSCPP_INCLUDE_DIRS})
link_directories(${SDBUSCPP_LIBRARY_DIRS})

add_executable(sharing-service
    src/main.cpp
)

target_link_libraries(sharing-service
    ${SDBUSCPP_LIBRARIES}
)

find_package(nlohmann_json REQUIRED)

target_link_libraries(sharing-service
    ${SDBUSCPP_LIBRARIES}
    nlohmann_json::nlohmann_json
)

add_library(sharing-lib
    lib/SharingService.cpp
    lib/Request.cpp
)

target_include_directories(sharing-lib PUBLIC lib)

target_link_libraries(sharing-lib
    ${SDBUSCPP_LIBRARIES}
)

target_link_libraries(sharing-service
    ${SDBUSCPP_LIBRARIES}
    nlohmann_json::nlohmann_json
)