cmake_minimum_required(VERSION 3.10)
project(drogon_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/proccontroller.h
    src/proccontroller.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)