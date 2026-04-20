add_executable(example-service main.cpp)

target_link_libraries(example-service
    sharing-lib
    sdbus-c++
)